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

#include "common/utility.h"
#include "common/sparkle_surface_file.h"
#include "common/sparkle_server.h"
#include "common/sparkle_protocol.h"
#include "common/sparkle_connection.h"

#define ALWAYS_UPLOAD 0
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
#ifdef USE_BLENDING
        "uniform float alpha;\n"
#endif
        "\nvoid main(void) {\n"
        "    gl_FragColor = texture2D(texture, outTexCoords);\n"
#ifdef USE_BLENDING
        "    gl_FragColor.a = alpha;\n"
#endif
        "}\n\n";


const GLint FLOAT_SIZE_BYTES = sizeof(float);
const GLint TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 5 * FLOAT_SIZE_BYTES;

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
    were_debug("EGL destroyed.\n");
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
#ifdef USE_BLENDING
    GLuint _textureAlphaHandle;
#endif
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
    
    were_debug("GL destroyed.\n");
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
#ifdef USE_BLENDING
    _textureAlphaHandle = glGetUniformLocation(_textureProgram, "alpha");
#endif
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
    void destroyTexture(); //FIXME Temporary solution
    const RectangleA &position();
    int strata();
    float alpha();
    
    void setPosition(int x1, int y1, int x2, int y2);
    void setStrata(int strata);
    void setAlpha(float alpha);
    void addDamage(int x1, int y1, int x2, int y2);
    
    virtual bool updateTexture() = 0;
    
protected:
    std::string _name;
    Texture *_texture;
    RectangleA _position;
    int _strata;
    float _alpha;
    RectangleA _damage;
};

CompositorGLSurface::~CompositorGLSurface()
{
    destroyTexture();
}

CompositorGLSurface::CompositorGLSurface(const std::string &name)
{
    _name = name;
    _texture = 0;
    _strata = 0;
    _alpha = 1.0f;
}

const std::string &CompositorGLSurface::name()
{
    return _name;
}

Texture *CompositorGLSurface::texture()
{
    if (_texture == 0)
        _texture = new Texture();
    return _texture;
}

void CompositorGLSurface::destroyTexture()
{
    if (_texture != 0)
    {
        delete _texture;
        _texture = 0;
    }
}

const RectangleA &CompositorGLSurface::position()
{
    return _position;
}

int CompositorGLSurface::strata()
{
    return _strata;
}

float CompositorGLSurface::alpha()
{
    return _alpha;
}

void CompositorGLSurface::setPosition(int x1, int y1, int x2, int y2)
{
    _position = RectangleA(PointA(x1, y1), PointA(x2, y2));
}

void CompositorGLSurface::setStrata(int strata)
{
    _strata = strata;
}

void CompositorGLSurface::setAlpha(float alpha)
{
    _alpha = alpha;
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
    
    if (texture()->width() != _surface->width() || texture()->height() != _surface->height())
    {
        texture()->resize(_surface->width(), _surface->height());
        _damage = RectangleA(PointA(0, 0), PointA(texture()->width(), texture()->height()));
        result = true;
    }
    
    if ((_damage.width() > 0 && _damage.height() > 0) || ALWAYS_UPLOAD)
    {
        unsigned char *data = _surface->data();
        
        //were_debug("Uploading %d %d %d %d -> %d %d\n", _damage.from.x, _damage.from.y, _damage.to.x, _damage.to.y, texture()->width(), texture()->height());


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture()->id());

#if 0 || ALWAYS_UPLOAD
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, texture()->width(), texture()->height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
#else
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, _damage.from.y,
            texture()->width(), _damage.height(),
            GL_BGRA_EXT, GL_UNSIGNED_BYTE,
            &data[_damage.from.y * texture()->width() * 4]);
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

    void connection(std::shared_ptr <SparkleConnection> client);
    void packet(std::shared_ptr<SparkleConnection> client, std::shared_ptr<SparklePacket> packet);
    
    void registerSurfaceFile(const std::string &name, const std::string &path, int width, int height);
    void unregisterSurface(const std::string &name);
    void setSurfacePosition(const std::string &name, int x1, int y1, int x2, int y2);
    void setSurfaceStrata(const std::string &name, int strata);
    void setSurfaceAlpha(const std::string &name, float alpha);
    void addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2);

    std::shared_ptr<CompositorGLSurface> findSurface(const std::string &name);
    void transformCoordinates(int x, int y, std::shared_ptr<CompositorGLSurface> surface, int *_x, int *_y);
    
    static bool sortFunction(std::shared_ptr<CompositorGLSurface> a1, std::shared_ptr<CompositorGLSurface> a2);

private:
    WereEventLoop *_loop;
    Platform *_platform;
    CompositorGL_EGL *_egl;
    CompositorGL_GL *_gl;

    SparkleServer *_server;

    std::vector< std::shared_ptr<CompositorGLSurface> > _surfaces;

    std::array<float, 20> _plane;
    bool _redraw;
};

//==================================================================================================

CompositorGL::~CompositorGL()
{
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

    _plane = {{
    // X, Y, Z, U, V
        -1.0f, -1.0f, 0, 0.0f, 0.0f,
         1.0f, -1.0f, 0, 1.0f, 0.0f,
        -1.0f,  1.0f, 0, 0.0f, 1.0f,
         1.0f,  1.0f, 0, 1.0f, 1.0f,
    }};

    _platform->initializeForNativeDisplay.connect(WereSimpleQueuer(loop, &CompositorGL::initializeForNativeDisplay, this));
    _platform->initializeForNativeWindow.connect(WereSimpleQueuer(loop, &CompositorGL::initializeForNativeWindow, this));
    _platform->finishForNativeDisplay.connect(WereSimpleQueuer(loop, &CompositorGL::finishForNativeDisplay, this));
    _platform->finishForNativeWindow.connect(WereSimpleQueuer(loop, &CompositorGL::finishForNativeWindow, this));

    _platform->draw.connect(WereSimpleQueuer(loop, &CompositorGL::draw, this));

    _platform->pointerDown.connect(WereSimpleQueuer(loop, &CompositorGL::pointerDown, this));
    _platform->pointerUp.connect(WereSimpleQueuer(loop, &CompositorGL::pointerUp, this));
    _platform->pointerMotion.connect(WereSimpleQueuer(loop, &CompositorGL::pointerMotion, this));
    _platform->keyDown.connect(WereSimpleQueuer(loop, &CompositorGL::keyDown, this));
    _platform->keyUp.connect(WereSimpleQueuer(loop, &CompositorGL::keyUp, this));
    
    _server = new SparkleServer(_loop, "/dev/shm/sparkle.socket");
    
    _server->signal_connected.connect(WereSimpleQueuer(loop, &CompositorGL::connection, this));
    _server->signal_packet.connect(WereSimpleQueuer(loop, &CompositorGL::packet, this));
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
    
    _display_size_notification r1 = {_gl->_surfaceWidth, _gl->_surfaceHeight};
    _server->broadcast1(&display_size_notification, &r1);
    
    _redraw = true;
}

void CompositorGL::finishForNativeWindow()
{
    if (_gl != 0)
    {
        for (auto it = _surfaces.begin(); it != _surfaces.end(); ++it)
            (*it)->destroyTexture();
        
        delete _gl;
    }
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
        
        _display_size_notification r1 = {_gl->_surfaceWidth, _gl->_surfaceHeight};
        _server->broadcast1(&display_size_notification, &r1);
    }
    
#endif

    for (auto it = _surfaces.begin(); it != _surfaces.end(); ++it)
    {
        _redraw |= (*it)->updateTexture();
    }
    
    if (_redraw)
    {
        _redraw = false;

        glClearColor(0.15f, 0.35f, 0.55f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(_gl->_textureProgram);

        for (auto it = _surfaces.begin(); it != _surfaces.end(); ++it)
        {
            std::shared_ptr<CompositorGLSurface> surface = (*it);
            
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
            if (surface->alpha() != 1.0f)
            {
                glUniform1f(_gl->_textureAlphaHandle, surface->alpha());
                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
#endif

            glBindTexture(GL_TEXTURE_2D, surface->texture()->id());
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
#ifdef USE_BLENDING
            if (surface->alpha() != 1.0f)
            {
                glDisable(GL_BLEND);
            }
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
    for (auto rit = _surfaces.rbegin(); rit != _surfaces.rend(); ++rit)
    {
        std::shared_ptr<CompositorGLSurface> surface = (*rit);
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _pointer_down_notification r1 = {surface->name().c_str(), slot, _x, _y};
            _server->broadcast1(&pointer_down_notification, &r1);
            return;
        }
    }
}

void CompositorGL::pointerUp(int slot, int x, int y)
{
    for (auto rit = _surfaces.rbegin(); rit != _surfaces.rend(); ++rit)
    {
        std::shared_ptr<CompositorGLSurface> surface = (*rit);
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _pointer_up_notification r1 = {surface->name().c_str(), slot, _x, _y};
            _server->broadcast1(&pointer_up_notification, &r1);
            return;
        }
    }
}

void CompositorGL::pointerMotion(int slot, int x, int y)
{
    for (auto rit = _surfaces.rbegin(); rit != _surfaces.rend(); ++rit)
    {
        std::shared_ptr<CompositorGLSurface> surface = (*rit);
        int _x;
        int _y;
        transformCoordinates(x, y, surface, &_x, &_y);
        if (_x != -1 && _y != -1)
        {
            _pointer_motion_notification r1 = {surface->name().c_str(), slot, _x, _y};
            _server->broadcast1(&pointer_motion_notification, &r1);
            return;
        }
    }
}

void CompositorGL::keyDown(int code)
{
    _key_down_notification r1 = {code};
    _server->broadcast1(&key_down_notification, &r1);
}

void CompositorGL::keyUp(int code)
{
    _key_up_notification r1 = {code};
    _server->broadcast1(&key_up_notification, &r1);
}

//==================================================================================================

void CompositorGL::connection(std::shared_ptr <SparkleConnection> client)
{
    if (_gl != 0)
    {
        _display_size_notification r1 = {_gl->_surfaceWidth, _gl->_surfaceHeight};
        client->send1(&display_size_notification, &r1);
    }
}

void CompositorGL::packet(std::shared_ptr<SparkleConnection> client, std::shared_ptr<SparklePacket> packet)
{
    uint32_t operation = packet->header()->operation;
    
    if (operation == register_surface_file_request.code)
    {
        _register_surface_file_request r1;
        SparkleConnection::unpack1(&register_surface_file_request, packet.get(), &r1);
        registerSurfaceFile(r1.name, r1.path, r1.width, r1.height);
    }
    else if (operation == unregister_surface_request.code)
    {
        _unregister_surface_request r1;
        SparkleConnection::unpack1(&unregister_surface_request, packet.get(), &r1);
        unregisterSurface(r1.name);
    }
    else if (operation == set_surface_position_request.code)
    {
        _set_surface_position_request r1;
        SparkleConnection::unpack1(&set_surface_position_request, packet.get(), &r1);
        setSurfacePosition(r1.name, r1.x1, r1.y1, r1.x2, r1.y2);
    }
    else if (operation == set_surface_strata_request.code)
    {
        _set_surface_strata_request r1;
        SparkleConnection::unpack1(&set_surface_strata_request, packet.get(), &r1);
        setSurfaceStrata(r1.name, r1.strata);
    }
    else if (operation == set_surface_alpha_request.code)
    {
        _set_surface_alpha_request r1;
        SparkleConnection::unpack1(&set_surface_alpha_request, packet.get(), &r1);
        setSurfaceAlpha(r1.name, r1.alpha);
    }
    else if (operation == add_surface_damage_request.code)
    {
        _add_surface_damage_request r1;
        SparkleConnection::unpack1(&add_surface_damage_request, packet.get(), &r1);
        addSurfaceDamage(r1.name, r1.x1, r1.y1, r1.x2, r1.y2);
    }
    else if (operation == key_down_request.code)
    {
        _key_down_request r1;
        SparkleConnection::unpack1(&key_down_request, packet.get(), &r1);
        keyDown(r1.code);
    }
    else if (operation == key_up_request.code)
    {
        _key_up_request r1;
        SparkleConnection::unpack1(&key_up_request, packet.get(), &r1);
        keyUp(r1.code);
    }
}

void CompositorGL::registerSurfaceFile(const std::string &name, const std::string &path, int width, int height)
{
    unregisterSurface(name);
        
    std::shared_ptr<CompositorGLSurface> surface(new CompositorGLSurfaceFile(name, path, width, height));
    _surfaces.push_back(surface);
    std::sort (_surfaces.begin(), _surfaces.end(), sortFunction);
    were_debug("Surface [%s] registered.\n", name.c_str());

    _redraw = true;
}

void CompositorGL::unregisterSurface(const std::string &name)
{
    auto it = _surfaces.begin();
    while (it != _surfaces.end())
    {
        std::shared_ptr<CompositorGLSurface> surface = (*it);
        if (surface->name() == name)
        {
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
    were_debug("Surface [%s]: position changed (%d %d %d %d).\n", name.c_str(), x1, y1, x2, y2);
    
    std::shared_ptr<CompositorGLSurface> surface = findSurface(name);
    if (surface != 0)
    {
        surface->setPosition(x1, y1, x2, y2);
        _redraw = true;
    }
}

void CompositorGL::setSurfaceStrata(const std::string &name, int strata)
{
    were_debug("Surface [%s]: strata changed.\n", name.c_str());
    
    std::shared_ptr<CompositorGLSurface> surface = findSurface(name);
    if (surface != 0)
    {
        surface->setStrata(strata);
        std::sort (_surfaces.begin(), _surfaces.end(), sortFunction);
        _redraw = true;
    }
}

void CompositorGL::setSurfaceAlpha(const std::string &name, float alpha)
{
    were_debug("Surface [%s]: alpha changed.\n", name.c_str());
    
    std::shared_ptr<CompositorGLSurface> surface = findSurface(name);
    if (surface != 0)
    {
        surface->setAlpha(alpha);
        _redraw = true;
    }
}

void CompositorGL::addSurfaceDamage(const std::string &name, int x1, int y1, int x2, int y2)
{
    //were_debug("Surface [%s]: damage (%d %d %d %d).\n", name.c_str(), x1, y1, x2, y2);
    
    std::shared_ptr<CompositorGLSurface> surface = findSurface(name);
    if (surface != 0)
        surface->addDamage(x1, y1, x2, y2);
}

//==================================================================================================

std::shared_ptr<CompositorGLSurface> CompositorGL::findSurface(const std::string &name)
{
    for (auto it = _surfaces.begin(); it != _surfaces.end(); ++it)
    {
        if ((*it)->name() == name)
            return (*it);
    }
    
    were_debug("Surface [%s]: not registered.\n", name.c_str());
    
    return 0;
}

void CompositorGL::transformCoordinates(int x, int y, std::shared_ptr<CompositorGLSurface> surface, int *_x, int *_y)
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

bool CompositorGL::sortFunction(std::shared_ptr<CompositorGLSurface> a1, std::shared_ptr<CompositorGLSurface> a2)
{
    return a1->strata() < a2->strata();
}

//==================================================================================================

Compositor *compositor_gl_create(WereEventLoop *loop, Platform *platform)
{
    return new CompositorGL(loop, platform);
}

//==================================================================================================

