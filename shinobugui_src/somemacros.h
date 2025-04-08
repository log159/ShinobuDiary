#pragma once
#define INITSTR ""
#define INITINT 0
#define INITDOUBLE 0.0
#define INITBOOL false
#define INITFILE_WAY "./Saves/userconfig.ini"
#define STYLE_WAY "./Saves/style.data"
#define TITLE_IMAGE_WAY "./Icon/su_t.png"
#define FONTS_BASE_PATH  "./Fonts/"
#define IMG_BASE_PATH  "./Icon/"
#define ZEROFLOAT (float(1e-6))
#define ZERODOUBLE (double(1e-15))
#define POS_0 uint32_t(0x1)
#define POS_1 uint32_t(0x2)
#define POS_2 uint32_t(0x4)
#define POS_3 uint32_t(0x8)

struct RectF {
    float x, y, w, h;
    RectF(float x, float y, float w, float h)
        : x(x), y(y), w(x), h(y) {
    }
};
