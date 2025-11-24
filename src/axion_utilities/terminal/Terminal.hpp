#pragma once
#include <iostream>
#include <sstream>

struct TEXT_COLOR
{
    int r;
    int g;
    int b;
};

//FIXME: Adjust everything to use these predefined colors

// TEXT_COLOR TEXT_COLOR_RED   = {255, 0, 0};
// TEXT_COLOR TEXT_COLOR_GREEN = {0, 255, 0};
// TEXT_COLOR TEXT_COLOR_BLUE  = {0, 0, 255};

class Terminal
{
public:

    static void SetTextColor(int r, int g, int b);
    static void SetTextColor(TEXT_COLOR color);

    static void ResetTextColor();

private:
    Terminal() = default;
};