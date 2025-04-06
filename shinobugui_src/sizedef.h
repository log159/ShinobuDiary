#pragma once
#define DEFSIZE 256
#define DEFSIZE2 DEFSIZE*2
#define DEFSIZE4 DEFSIZE*4
#define DEFSIZE8 DEFSIZE*8
#define DEFSIZE16 DEFSIZE*16
#define DEFSIZEK 1024
#define DEFSIZEK8 DEFSIZEK * 8
#define DEFSIZEK16 DEFSIZEK * 16
#define DEFPATHSIZE DEFSIZEK16

struct RectF {
    float x, y, w, h;
    RectF(float x, float y, float w, float h)
        : x(x), y(y), w(x), h(y) {
    }
};
