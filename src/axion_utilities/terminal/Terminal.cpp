#include "Terminal.hpp"

void Terminal::SetTextColor(int r, int g, int b)
{
    std::ostringstream ss;
    ss << "\x1b[38;2;" << (r < 0 ? 0 : (r > 255 ? 255 : r)) << ';'
       << (g < 0 ? 0 : (g > 255 ? 255 : g)) << ';'
       << (b < 0 ? 0 : (b > 255 ? 255 : b)) << 'm';
    std::cout << ss.str() << std::flush;
}

void Terminal::SetTextColor(TEXT_COLOR color)
{
    SetTextColor(color.r, color.g, color.b);
}

void Terminal::ResetTextColor()
{
    std::cout << "\x1b[0m" << std::flush;
}
