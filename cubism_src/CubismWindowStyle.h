#pragma once

#include <Windows.h>
class CubismWindowStyle;

enum WINSTYLE: int
{
    // 置顶不穿透
    WinTop,
    // 穿透不置顶
    WinApha,
    // 置顶并且穿透
    WinTopApha,
    // 不置顶并且不穿透
    WinNoTopNoApha
};


class CubismWindowStyle {
public:
    static WINSTYLE winStyle;
    static bool     canTrans;
public:
    static void Update(HWND hwnd);
    static void SetWinStyle(const WINSTYLE& ws);
    static WINSTYLE GetWinStyle();
private:
    static void WindowStyleUpdate(HWND hwnd);
    static void SetWindowTopApha(HWND hwnd, bool isTop, bool isApha);
    static BOOL GetPixelRGBA(HWND hwnd, int x, int y, BYTE& r, BYTE& g, BYTE& b, BYTE& a);
private:
    CubismWindowStyle();
    CubismWindowStyle(const CubismWindowStyle&);
};

