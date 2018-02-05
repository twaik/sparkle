#ifndef UTILITY_H
#define UTILITY_H

class PointA
{
public:
    PointA() : x(0), y(0) {}
    PointA(int x, int y) : x(x), y(y) {}
    int x;
    int y;
};

class RectangleA
{
public:
    RectangleA() {}
    RectangleA(const PointA &from, const PointA &to) : from(from), to(to) {}
    PointA from;
    PointA to;
    int width() {return to.x - from.x;}
    int height() {return to.y - from.y;}
};

class SizeA
{
public:
    SizeA() : width(0), height(0) {}
    SizeA(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

class CoordinateC
{
public:
    CoordinateC() : relative(0.0), absolute(0) {}
    CoordinateC(float relative, int absolute) : relative(relative), absolute(absolute) {}
    float relative;
    int absolute;
};

class PointC
{
public:
    PointC() {}
    PointC(const CoordinateC &x, const CoordinateC &y) : x(x), y(y) {}
    CoordinateC x;
    CoordinateC y;
};

class RectangleC
{
public:
    RectangleC() {}
    RectangleC(const PointC &from, const PointC &to) : from(from), to(to) {}
    PointC from;
    PointC to;
};

#endif //UTILITY_H

