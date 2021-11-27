#pragma once

#include "../../Math/Math.h"

enum class CursorType {
    ARROW,
    TEXT_INPUT,  //テキスト入力時
    RESIZE_ALL,  //未使用
    RESIZE_NS,   //左右のボーダーラインにカーソルを合わせたとき
    RESIZE_EW,   //上下のボーダーラインにカーソルを合わせたとき
    RESIZE_NESW, //ウィンドウの左下にカーソルを合わせたとき
    RESIZE_NWSE, //ウィンドウの右下にカーソルを合わせるとき
    HAND,        //未使用
    NOT_ALLOWED, //インタラクションが無効なものをホバリングしようとしたとき
    COUNT
};

class GuiCursor {
public:
    GuiCursor();
    ~GuiCursor();
    GuiCursor(const GuiCursor&) = delete;
    GuiCursor& operator=(const GuiCursor&) = delete;

private:
    //マウスカーソルのテクスチャサイズ
    Vector2 mCursorsRect;
    //太い線のテクスチャサイズ
    Vector2 mThickLinesRect;

    //マウスカーソルのテクスチャデータの横幅
    static constexpr int CURSOR_TEX_WIDTH = 108;
    //マウスカーソルのテクスチャデータの縦幅
    static constexpr int CURSOR_TEX_HEIGHT = 27;
    //最大線幅
    static constexpr int LINE_WIDTH_MAX = 63;
    //マウスカーソルのテクスチャデータ
    static constexpr char CURSOR_TEX_PIXELS[CURSOR_TEX_WIDTH * CURSOR_TEX_HEIGHT + 1] = {
        "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
        "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
        "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
        "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
        "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
        "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
        "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
        "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
        "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
        "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
        "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
        "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
        "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
        "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
        "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
        "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
        "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
        "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
        "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
        "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
        "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
        "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
        "------------        -    X    -           X           -X.....................X-           ------------------"
        "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
        "                                                      -  X..X           X..X  -                             "
        "                                                      -   X.X           X.X   -                             "
        "                                                      -    XX           XX    -                             "
    };

    static inline const Vector2 CURSOR_DATA[static_cast<int>(CursorType::COUNT)][3] = {
        //Position             Size                 Offset
        { Vector2(0.f, 3.f),   Vector2(12.f, 19.f), Vector2(0.f, 0.f) },    //ARROW
        { Vector2(13.f, 0.f),  Vector2(7.f, 16.f),  Vector2(1.f, 8.f) },    //TEXT_INPUT
        { Vector2(31.f, 0.f),  Vector2(23.f, 23.f), Vector2(11.f, 11.f) },  //RESIZE_ALL
        { Vector2(21.f, 0.f),  Vector2(9.f, 23.f),  Vector2(4.f, 11.f) },   //RESIZE_NS
        { Vector2(55.f, 18.f), Vector2(23.f, 9.f),  Vector2(11.f, 4.f) },   //RESIZE_EW
        { Vector2(73.f, 0.f),  Vector2(17.f, 17.f), Vector2(8.f, 8.f) },    //RESIZE_NESW
        { Vector2(55.f, 0.f),  Vector2(17.f, 17.f), Vector2(8.f, 8.f) },    //RESIZE_NWSE
        { Vector2(91.f, 0.f),  Vector2(17.f, 22.f), Vector2(5.f, 0.f) },    //HAND
    };
};
