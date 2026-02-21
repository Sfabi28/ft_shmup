#include "game.hpp"
#include <string>
#include <ncurses.h>

static const char *g_digits[10][3] = {
    {" _ ", "| |", "|_|"},
    {"   ", "  |", "  |"},
    {" _ ", " _|", "|_ "},
    {" _ ", " _|", " _|"},
    {"   ", "|_|", "  |"},
    {" _ ", "|_ ", " _|"},
    {" _ ", "|_ ", "|_|"},
    {" _ ", "  |", "  |"},
    {" _ ", "|_|", "|_|"},
    {" _ ", "|_|", " _|"}};

static const char *g_letters[26][3] = {
    {" _ ", "|_|", "| |"},
    {" _ ", "|_\\", "|_/"},
    {" _ ", "|  ", "|_ "},
    {" _ ", "| \\", "|_/"},
    {" _ ", "|_ ", "|_ "},
    {" _ ", "|_ ", "|  "},
    {" _ ", "| _", "|_|"},
    {"   ", "|_|", "| |"},
    {"   ", " | ", " | "},
    {"   ", "  |", "\\_|"},
    {"   ", "|/ ", "|\\ "},
    {"   ", "|  ", "|_ "},
    {"    ", "|\\/|", "|  |"},
    {"   ", "|\\|", "| |"},
    {" _ ", "| |", "|_|"},
    {" _ ", "|_|", "|  "},
    {" _ ", "| |", "|_\\"},
    {" _ ", "|_|", "| \\"},
    {" _ ", "|_ ", " _|"},
    {"___", " | ", " | "},
    {"   ", "| |", "|_|"},
    {"   ", "\\ /", " v "},
    {"     ", "| | |", "|_|_|"},
    {"   ", "\\/ ", "/\\ "},
    {"   ", "\\/ ", " | "},
    {" _ ", " / ", "/_ "}};

static const char *g_points[3] = {
    "   ",
    " o ",
    " o "};

void ascii_art(WINDOW *win, int y, int x, const std::string &text, int area_width, bool center)
{
    int spacing = 1;
    int total_width = 0;

    if (total_width > 0)
        total_width -= spacing;

    int start_x = x;

    if (center)
        start_x = x + (area_width / 2) - (total_width / 2);

    int start_y = y + 2;

    for (char c : text)
    {
        if (c >= '0' && c <= '9')
            total_width += std::string(g_digits[c - '0'][0]).length();
        else if (c >= 'A' && c <= 'Z')
            total_width += std::string(g_letters[c - 'A'][0]).length();
        else if (c >= 'a' && c <= 'z')
            total_width += std::string(g_letters[c - 'a'][0]).length();
        else if (c == ':')
            total_width += std::string(g_points[0]).length();
        else
            total_width += 3;

        total_width += spacing;
    }

    if (total_width > 0)
        total_width -= spacing;

    for (int row = 0; row < 3; row++)
    {
        int current_x = start_x;

        for (char c : text)
        {
            const char *segment = "   ";

            if (c >= '0' && c <= '9')
                segment = g_digits[c - '0'][row];
            else if (c >= 'A' && c <= 'Z')
                segment = g_letters[c - 'A'][row];
            else if (c >= 'a' && c <= 'z')
                segment = g_letters[c - 'a'][row];
            else if (c == ':')
                segment = g_points[row];

            mvwprintw(win, start_y + row, current_x, "%s", segment);

            current_x += std::string(segment).length() + spacing;
        }
    }
}