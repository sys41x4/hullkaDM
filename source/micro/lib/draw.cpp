#include "draw.h"

void DRAW::charArr(WINDOW *win, int x, int y, int colorID, const char* arr){
    wattron(win, COLOR_PAIR(colorID));
    mvwprintw(win, y, x, arr);
    wattroff(win, COLOR_PAIR(colorID));
    wrefresh(win);
}

void DRAW::charArr(WINDOW *win, int x, int y, int colorID, char* arr){
    wattron(win, COLOR_PAIR(colorID));
    mvwprintw(win, y, x, arr);
    wattroff(win, COLOR_PAIR(colorID));
    wrefresh(win);
}