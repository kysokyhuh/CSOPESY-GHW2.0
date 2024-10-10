#include "baseScreen.h"
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>

void BaseScreen::gpuStatusScreen() {
    initscr();
    noecho();
    cbreak();

    WINDOW* win = newwin(10, 40, 10, 10);
    nodelay(win, TRUE);

    int ch;
    while ((ch = wgetch(win)) != 'q') {
        // Dummy GPU status display code
        mvwprintw(win, 1, 1, "GPU Status");
        int gpuTemp = 50 + rand() % 30;
        mvwprintw(win, 2, 1, "Temperature: %d°C", gpuTemp);
        wrefresh(win);
        usleep(500000);
    }

    delwin(win);
    endwin();
}
