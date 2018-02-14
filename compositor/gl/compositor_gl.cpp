#include "compositor_gl.h"
#include "texture.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <string>
#include <algorithm>


#include "common/sparkle_surface_file.h"
#include "common/sparkle_server.h"


#define USE_BLENDING

//==================================================================================================

static const char simpleVS[] =
        "attribute vec4 position;\n"
        "attribute vec2 texCoords;\n"
        "varying vec2 outTexCoords;\n"
        "\nvoid main(void) {\n"
        "    outTexCoords = texCoords;\n"
        "    gl_Position = position;\n"
        "}\n\n";

static const char simpleFS[] =
        "precision mediump float;\n\n"
        "varying vec2 outTexCoords;\n"
        "uniform sampler2D texture;\n"
        "\nvoid main(void) {\n"
        "    gl_FragColor = texture2D(texture, outTexCoords);\n"
        "}\n\n";

#if 0
const GLfloat triangleVerticesDataNormal[] = {
        // X, Y, Z, U, V
        -1.0f, -1.0f, 0,    0.f, 1.f,
         1.0f, -1.0f, 0,    1.f, 1.f,
        -1.0f,  1.0f, 0,    0.f, 0.f,
         1.0f,  1.0f, 0,    1.f, 0.f,
};

const GLfloat triangleVerticesDataRotated[] = {
        // X, Y, Z, U, V
        -1.0f, -1.0f, 0,    0.f, 0.f,
         1.0f, -1.0f, 0,    1.f, 0.f,
        -1.0f,  1.0f, 0,    0.f, 1.f,
         1.0f,  1.0f, 0,    1.f, 1.f,
};
#endif

//const GLint FLOAT_SIZE_BYTES = 4;
//const GLint TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 5 * FLOAT_SIZE_BYTES;
const GLint TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 20;

//==================================================================================================

class CompositorGL_EGL
{
public:
    ~CompositorGL_EGL();
    CompositorGL_EGL(NativeDisplayType nativeDisplay);
    
    EGLint getVID();
    
    EGLDisplay _display;
    EGLConfig _config;
};

CompositorGL_EGL::~CompositorGL_EGL()
{
    eglTerminate(_display);
}

CompositorGL_EGL::CompositorGL_EGL(NativeDisplayType nativeDisplay)
{
    _display = eglGetDisplay(nativeDisplay);
    if (_display == EGL_NO_DISPLAY)
        throw std::runtime_error("[CompositorGL_EGL::CompositorGL_EGL] Failed: eglGetDisplay.");

    EGLint majorVersion, minorVersion;

    if (eglInitialize(_display, &majorVersion, &minorVersion) != EGL_TRUE)
        throw std::runtime_error("[CompositorGL_EGL::CompositorGL_EGL] Failed: eglInitialize.");

    were_message("EGL_VERSION = %s\n",       eglQueryString(_display, EGL_VERSION));
    were_message("EGL_VENDOR = %s\n",        eglQueryString(_display, EGL_VENDOR));
    were_message("EGL_CLIENT_APIS = %s\n",   eglQueryString(_display, EGL_CLIENT_APIS));
    were_message("EGL_EXTENSIONS = %s\n",    eglQueryString(_display, EGL_EXTENSIONS));

    const EGLint configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE};

    EGLint numConfigs;

    if (eglChooseConfig(_display, configAttribs, &_config, 1, &numConfigs) != EGL_TRUE)
        throw std::runtime_error("[CompositorGL_EGL::CompositorGL_EGL] Failed: eglChooseConfig.");

    eglBindAPI(EGL_OPENGL_ES_API);
}

EGLint CompositorGL_EGL::getVID()
{
    EGLint vid;

    if (eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &vid) != EGL_TRUE)
        throw std::runtime_error("[CompositorGL_EGL::getVID] Failed: eglGetConfigAttrib.");

    return vid;
}

//==================================================================================================

class CompositorGL_GL
{
public:
    ~CompositorGL_GL();
    CompositorGL_GL(CompositorGL_EGL *egl, NativeWindowType window);
    
    static GLuint loadShader(GLenum shaderType, const char *pSource);
    
    CompositorGL_EGL *_egl;
    EGLSurface _surface;
    EGLContext _context;
    int _surfaceWidth;
    int _surfaceHeight;
    GLuint _vertexShader;
    GLuint _pixelShader;
    GLuint _textureProgram;
    GLuint _texturePositionHandle;
    GLuint _textureTexCoordsHandle;
    //GLuint _textureSamplerHandle;
};

CompositorGL_GL::~CompositorGL_GL()
{
    glDeleteProgram(_textureProgram);
    glDeleteShader(_pixelShader);
    glDeleteShader(_vertexShader);

    eglMakeCurrent(_egl->_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_egl->_display, _context);
    eglDestroySurface(_egl->_display, _surface);
}

CompositorGL_GL::CompositorGL_GL(CompositorGL_EGL *egl, NativeWindowType window)
{
    _egl = egl;
    
    _surface = eglCreateWindowSurface(_egl->_display, _egl->_config, window, NULL);
    if (_surface == EGL_NO_SURFACE)
        throw std::runtime_error("[CompositorGL_GL::CompositorGL_GL] Failed: eglCreateWindowSurface.");

    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

    _context = eglCreateContext(_egl->_display, _egl->_config, EGL_NO_CONTEXT, contextAttribs);
    if (_context == EGL_NO_CONTEXT)
        throw std::runtime_error("[CompositorGL_GL::CompositorGL_GL] Failed: eglCreateContext.");

    if (eglMakeCurrent(_egl->_display, _surface, _surface, _context) != EGL_TRUE)
        throw std::runtime_error("[CompositorGL_GL::CompositorGL_GL] Failed: eglMakeCurrent.");

    were_message("GL_VERSION = %s\n",    (char *) glGetString(GL_VERSION));
    were_message("GL_VENDOR = %s\n",     (char *) glGetString(GL_VENDOR));
    were_message("GL_RENDERER = %s\n",   (char *) glGetString(GL_RENDERER));
    were_message("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));

    _vertexShader = loadShader(GL_VERTEX_SHADER, simpleVS);
    _pixelShader = loadShader(GL_FRAGMENT_SHADER, simpleFS);

    _textureProgram = glCreateProgram();
    if (!_textureProgram)
        throw std::runtime_error("[CompositorGL_GL::CompositorGL_GL] Failed: glCreateProgram.");

    glAttachShader(_textureProgram, _vertexShader);
    glAttachShader(_textureProgram, _pixelShader);
    glLinkProgram(_textureProgram);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_textureProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
        throw std::runtime_error("[CompositorGL_GL::CompositorGL_GL] Failed: glLinkProgram.");

    _texturePositionHandle = glGetAttribLocation(_textureProgram, "position");
    _textureTexCoordsHandle = glGetAttribLocation(_textureProgram, "texCoords");
    //_textureSamplerHandle = glGetUniformLocation(_textureProgram, "texture");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    eglQuerySurface(_egl->_display, _surface, EGL_WIDTH, &_surfaceWidth);
    eglQuerySurface(_egl->_display, _surface, EGL_HEIGHT, &_surfaceHeight);
    glViewport(0, 0, _surfaceWidth, _surfaceHeight);

    eglSwapInterval(_egl->_display, 0);
}

GLuint CompositorGL_GL::loadShader(GLenum shaderType, const char *pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader)
        throw std::runtime_error("[CompositorGL_GL::loadShader] Failed: glCreateShader.");

    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE)
        throw std::runtime_error("[CompositorGL_GL::loadShader] Failed: glCompileShader.");

    return shader;
}

//==================================================================================================

class CompositorGLSurface
{
public:
    virtual ~CompositorGLSurface();
    CompositorGLSurface(const std::string &name);
    
    const std::string &name();
    Texture *texture();
    const RectangleA &position();
    int strata();
    
    void setPosition(int x1, int y1, int x2, int y2);
    void setStrata(int strata);
    void addDamage(int x1, int y1, int x2, int y2);
    
    virtual bool updateTexture() = 0;
    
protected:
    std::string _name;
    Texture _texture;
    RectangleA _position;
    int _strata;
    RectangleA _damage;
};

CompositorGLSurface::~CompositorGLSurface()
{
}

CompositorGLSurface::CompositorGLSurface(const std::string &name)
{
    _name = name;
    _strata = 0;
}

const std::string &CompositorGLSurface::name()
{
    return _name;
}

Texture *CompositorGLSurface::texture()
{
    return &_texture;
}

const RectangleA &CompositorGLSurface::position()
{
    return _position;
}

int CompositorGLSurface::strata()
{
    return _strata;
}

void CompositorGLSurface::setPosition(int x1, int y1, int x2, int y2)
{
    _position = RectangleA(PointA(x1, y1), PointA(x2, y2));
}

void CompositorGLSurface::setStrata(int strata)
{
    _strata = strata;
}

void CompositorGLSurface::addDamage(int x1, int y1, int x2, int y2)
{
    if (_damage.width() > 0 && _damage.height() > 0)
    {
        if (_damage.from.x > x1)
            _damage.from.x = x1;
        if (_damage.from.y > y1)
            _damage.from.y = y1;
        if (_damage.to.x < x2)
            _damage.to.x = x2;
        if (_damage.to.y < y2)
            _damage.to.y = y2;
    }
    else
        _damage = RectangleA(PointA(x1, y1), PointA(x2, y2));
}

//==================================================================================================

class CompositorGLSurfaceFile : public CompositorGLSurface
{
public:
    ~CompositorGLSurfaceFile();
    CompositorGLSurfaceFile(const std::string &name, const std::string &path, int width, int height);
    
    bool updateTexture();
    
private:
    SparkleSurfaceFile *_surface;
};

CompositorGLSurfaceFile::~CompositorGLSurfaceFile()
{
    delete _surface;
}

CompositorGLSurfaceFile::CompositorGLSurfaceFile(const std::string &name, const std::string &path, int width, int height) :
    CompositorGLSurface(name)
{
    _surface = new SparkleSurfaceFile(path, width, height, false);
}

bool CompositorGLSurfaceFile::updateTexture()
{
    bool result = false;
    
    if (_texture.width() != _surface->width() || _texture.height() != _surface->height())
    {
        texture()->resize(_surface->width(), _surface->height());
        _damage = RectangleA(PointA(0, 0), PointA(_texture.width(), _texture.height()));
        result = true;
    }
    
    if (_damage.width() > 0 && _damage.height() > 0)
    {
        unsigned char *data = _surface->data();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture.id());

#if 0
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, _texture.width(), _texture.height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
#else
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, _damage.from.y,
            _texture.width(), _damage.height(),
            GL_BGRA_EXT, GL_UNSIGNED_BYTE,
            &data[_damage.from.y * _texture.width() * 4]);
#endif
        
        _damage = RectangleA(PointA(0, 0), PointA(0, 0));
        result = true;
    }
    
    return result;
}

//==================================================================================================

class CompositorGL : public Compositor
{
public:
    ~CompositorGL();
    CompositorGL(WereEventLoop *loop, Platform *platform);

    int displayWidth();
    int displayHeight();

private:
    void initializeForNativeDisplay(NativeDisplayType nativeDisplay);
    void finishForNativeDisplay();
    void initializeForNativeWindow(NativeWindowType window);
    void finishForNativeWindow();

    void draw();

    void pointerDown(int slot, int x, int y);
    void pointerUp(int slot, int x, int y);
    void pointerMotion(int slot, int x, int y);
    void keyDown(int code);
    void keyUp(int code);

    void connection();
    void registerSurfaceFile(const std::string &name, const std::string &path, int width, int height);
    void unregisterSurface(const std::string &name);
    void setSurfacePosition(const std::string &name, int x1, int y1, int x2, int y2);
    void setSurfaceStrata(const std::string &name, int strata);
    void addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2);
    
    CompositorGLSurface *findSurface(const std::string &name);
    void transformCoordinates(int x, int y, CompositorGLSurface *surface, int *_x, int *_y);
    
    static bool sortFunction(CompositorGLSurface *a1, CompositorGLSurface *a2);

private:
    WereEventLoop *_loop;
    Platform *_platform;
    CompositorGL_EGL *_egl;
    CompositorGL_GL *_gl;

    SparkleServer *_server;

    std::vector<CompositorGLSurface *> _surfaces;
    
    GLfloat _plane[20];
    bool _redraw;
};

//==================================================================================================

CompositorGL::~CompositorGL()
{
    for (unsigned int i = 0; i < _surfaces.size(); ++i)
        delete _surfaces[i];
    
    delete _server;
    
    if (_gl)
        delete _gl;
    if (_egl)
        delete _egl;
}

CompositorGL::CompositorGL(WereEventLoop *loop, Platform *platform)
{
    _loop = loop;
    _platform = platform;
    
    _egl = 0;
    _gl = 0;
    
    _plane[0] = -1.0;
    _plane[1] = -1.0;
    _plane[2] = 0.0;
    _plane[3] = 0.0;
    _plane[4] = 0.0;

    _plane[5] = 1.0;
    _plane[6] = -1.0;
    _plane[7] = 0.0;
    _plane[8] = 1.0;
    _plane[9] = 0.0;

    _plane[10] = -1.0;
    _plane[11] = 1.0;
    _plane[12] = 0.0;
    _plane[13] = 0.0;
    _plane[14] = 1.0;

    _plane[15] = 1.0;
    _plane[16] = 1.0;
    _plane[17] = 0.0;
    _plane[18] = 1.0;
    _plane[19] = 1.0;
    
    _redraw = true;

    _platform->initializeForNativeDisplay.connect(_loop, std::bind(&CompositorGL::initializeForNativeDisplay, this, _1));
    _platform->initializeForNativeWindow.connect(_loop, std::bind(&CompositorGL::initializeForNativeWindow, this, _1));
    _platform->finishForNativeDisplay.connect(_loop, std::bind(&CompositorGL::finishForNativeDisplay, this));
    _platform->finishForNativeWindow.connect(_loop, std::bind(&CompositorGL::finishForNativeWindow, this));

    _platform->draw.connect(_loop, std::bind(&CompositorGL::draw, this));

    _platform->pointerDown.connect(_loop, std::bind(&CompositorGL::pointerDown, this, 
        _1, _2, _3));
    _platform->pointerUp.connect(_loop, std::bind(&CompositorGL::pointerUp, this,
        _1, _2, _3));
    _platform->pointerMotion.connect(_loop, std::bind(&CompositorGL::pointerMotion, this,
    _1, _2, _3));
    _platform->keyDown.connect(_loop, std::bind(&CompositorGL::keyDown, this, _1));
    _platform->keyUp.connect(_loop, std::bind(&CompositorGL::keyUp, this, _1));
    
    _server = new SparkleServer(_loop, "/dev/shm/sparkle.socket");
    _server->connection.connect(_loop, std::bind(&CompositorGL::connection, this));
    _server->registerSurfaceFile.connect(_loop, std::bind(&CompositorGL::registerSurfaceFile, this, _1, _2, _3, _4));
    _server->unregisterSurface.connect(_loop, std::bind(&CompositorGL::unregisterSurface, this, _1));
    _server->setSurfacePosition.connect(_loop, std::bind(&CompositorGL::setSurfacePosition, this, _1, _2, _3, _4, _5));
    _server->setSurfaceStrata.connect(_loop, std::bind(&CompositorGL::setSurfaceStrata, this, _1, _2));
    _server->addSurfaceDamage.connect(_loop, std::bind(&CompositorGL::addSurfaceDamage, this, _1, _2, _3, _4, _5));
}

int CompositorGL::displayWidth()
{
    return _gl->_surfaceWidth;
}

int CompositorGL::displayHeight()
{
    return _gl->_surfaceHeight;
}

//==================================================================================================

void CompositorGL::initializeForNativeDisplay(NativeDisplayType nativeDisplay)
{
    _egl = new CompositorGL_EGL(nativeDisplay);
    _platform->getVID.connect(std::bind(&CompositorGL_EGL::getVID, _egl));
}

void CompositorGL::finishForNativeDisplay()
{
    //XXX Disconnect getVID
    
    if (_egl != 0)
        delete _egl;
    _egl = 0;
}

void CompositorGL::initializeForNativeWindow(NativeWindowType window)
{
    _gl = new CompositorGL_GL(_egl, window);
}

void CompositorGL::finishForNativeWindow()
{
    if (_gl != 0)
        delete _gl;
    _gl = 0;
}

void CompositorGL::draw()
{
    if (_gl == 0)
        return;

#if 1
    int width;
    int height;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    eglQuerySurface(_egl->_display, _gl->_surface, EGL_WIDTH, &width);
    eglQuerySurface(_egl->_display, _gl->_surface, EGL_HEIGHT, &height);
    
    if (width != _gl->_surfaceWidth || height != _gl->_surfaceHeight)
    {
        _gl->_surfaceWidth = width;
        _gl->_surfaceHeight = height;
        glViewport(0, 0, _gl->_surfaceWidth, _gl->_surfaceHeight);
        _server->displaySize(_gl->_surfaceWidth, _gl->_surfaceHeight);
    }
    
#endif

    for (unsigned int i = 0; i < _surfaces.size(); ++i)
    {
        CompositorGLSurface *surface = _surfaces[i];
        _redraw |= surface->updateTexture();
    }
    
    if (_redraw)
    {
        _redraw = false;

        glClearColor(0.15f, 0.35f, 0.55f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(_gl->_textureProgram);

        for (unsigned int i = 0; i < _surfaces.size(); ++i)
        {
            CompositorGLSurface *surface = _surfaces[i];

            //XXX Ignore disabled layers

            //XXX Only recalculate when changed
            
            float x1r = 1.0 * surface->position().from.x / _gl->_surfaceWidth;
            float y1r = 1.0 * surface->position().from.y / _gl->_surfaceHeight;
            float x2r = 1.0 * surface->position().to.x / _gl->_surfaceWidth;
            float y2r = 1.0 * surface->position().to.y / _gl->_surfaceHeight;

            float x1 = x1r * 2 - 1.0;
            float y1 = - y1r * 2 + 1.0;
            float x2 = x2r * 2 - 1.0;
            float y2 = - y2r * 2 + 1.0;


            _plane[0] = x1;
            _plane[1] = y1;

            _plane[5] = x2;
            _plane[6] = y1;

            _plane[10] = x1;
            _plane[11] = y2;

            _plane[15] = x2;
            _plane[16] = y2;

            glVertexAttribPointer(_gl->_texturePositionHandle, 3, GL_FLOAT, GL_FALSE, TRIANGLE_VERTICES_DATA_STRIDE_BYTES, &_plane[0]);
            glVertexAttribPointer(_gl->_textureTexCoordsHandle, 2, GL_FLOAT, GL_FALSE, TRIANGLE_VERTICES_DATA_STRIDE_BYTES, &_plane[3]);

            glEnableVertexAttribArray(_gl->_texturePositionHandle);
            glEnableVertexAttribArray(_gl->_textureTexCoordsHandle);

#ifdef USE_BLENDING
            if (i >= 1)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
#endif
            glBindTexture(GL_TEXTURE_2D, surface->texture()->id());
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#ifdef USE_BLENDING
            glDisable(GL_BLEND);
#endif

            glDisableVertexAttribArray(_gl->_texturePositionHandle);
            glDisableVertexAttribArray(_gl->_textureTexCoordsHandle);
        }

        if (1)
            glFinish();

        eglSwapBuffers(_egl->_display, _gl->_surface);

        frame();
    }
}

//==================================================================================================

void CompositorGL::pointerDown(int slot, int x, int y)
{
    for (unsigned int i = _surfaces.size(); i > 0; --i)
    {
        CompositorGLSurface *surface = _surfaces[i - 1];
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _server->pointerDown(surface->name(), slot, _x, _y);
            return;
        }
    }
}

void CompositorGL::pointerUp(int slot, int x, int y)
{
    for (unsigned int i = _surfaces.size(); i > 0; --i)
    {
        CompositorGLSurface *surface = _surfaces[i - 1];
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _server->pointerUp(surface->name(), slot, _x, _y);
            return;
        }
    }
}

void CompositorGL::pointerMotion(int slot, int x, int y)
{
    for (unsigned int i = _surfaces.size(); i > 0; --i)
    {
        CompositorGLSurface *surface = _surfaces[i - 1];
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _server->pointerMotion(surface->name(), slot, _x, _y);
            return;
        }
    }
}

void CompositorGL::keyDown(int code)
{
    _server->keyDown(code);
}

void CompositorGL::keyUp(int code)
{
    _server->keyUp(code);
}

//==================================================================================================


void CompositorGL::connection()
{
    if (_gl != 0)
        _server->displaySize(_gl->_surfaceWidth, _gl->_surfaceHeight);
    
    were_debug("Connected\n");
}

void CompositorGL::registerSurfaceFile(const std::string &name, const std::string &path, int width, int height)
{
    //CompositorGLSurface *surface = findSurface(name);
        
    unregisterSurface(name);
        
    CompositorGLSurface *surface = new CompositorGLSurfaceFile(name, path, width, height);
    _surfaces.push_back(surface);
    std::sort (_surfaces.begin(), _surfaces.end(), sortFunction);
    were_debug("Surface [%s] registered.\n", name.c_str());

    _redraw = true;
}

void CompositorGL::unregisterSurface(const std::string &name)
{
    std::vector<CompositorGLSurface *>::iterator it = _surfaces.begin();
    while (it != _surfaces.end())
    {
        CompositorGLSurface *surface = *it;
        if (surface->name() == name)
        {
            delete surface;
            it = _surfaces.erase(it);
            were_debug("Surface [%s] unregistered.\n", name.c_str());
        }
        else
            ++it;
    }
        
    _redraw = true;
}

void CompositorGL::setSurfacePosition(const std::string &name, int x1, int y1, int x2, int y2)
{
    were_debug("Surface [%s]: position changed.\n", name.c_str());
    
    CompositorGLSurface *surface = findSurface(name);
    if (surface != 0)
    {
        surface->setPosition(x1, y1, x2, y2);
        _redraw = true;
    }
}

void CompositorGL::setSurfaceStrata(const std::string &name, int strata)
{
    were_debug("Surface [%s]: strata changed.\n", name.c_str());
    
    CompositorGLSurface *surface = findSurface(name);
    if (surface != 0)
    {
        surface->setStrata(strata);
        _redraw = true;
    }
}

void CompositorGL::addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2)
{
    CompositorGLSurface *surface = findSurface(name);
    if (surface != 0)
        surface->addDamage(x1, y1, x2, y2);
}

//==================================================================================================

CompositorGLSurface *CompositorGL::findSurface(const std::string &name)
{
    for (unsigned int i = 0; i < _surfaces.size(); ++i)
    {
        if (_surfaces[i]->name() == name)
            return _surfaces[i];
    }
    
    were_debug("Surface [%s]: not registered.\n", name.c_str());
    
    return 0;
}

void CompositorGL::transformCoordinates(int x, int y, CompositorGLSurface *surface, int *_x, int *_y)
{
    *_x = -1;
    *_y = -1;
    
    int x1a = surface->position().from.x;
    int y1a = surface->position().from.y;
    int x2a = surface->position().to.x;
    int y2a = surface->position().to.y;
    
    if (x < x1a || x > x2a || y < y1a || y > y2a)
        return;
    
    *_x = (x - x1a) * surface->texture()->width() / (x2a - x1a);
    *_y = (y - y1a) * surface->texture()->height() / (y2a - y1a);
}

bool CompositorGL::sortFunction(CompositorGLSurface *a1, CompositorGLSurface *a2)
{
    return a1->strata() < a2->strata();
}

//==================================================================================================

Compositor *compositor_gl_create(WereEventLoop *loop, Platform *platform)
{
    return new CompositorGL(loop, platform);
}

//==================================================================================================

