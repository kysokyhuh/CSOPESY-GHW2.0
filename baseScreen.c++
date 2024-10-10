#include "baseScreen.h"
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>

void displayGpuStatus(WINDOW* win) {
    int x, y;
    getmaxyx(win, y, x);
    wclear(win);
    box(win, 0, 0);
    
    mvwprintw(win, 1, 1, "GPU Status");

    int gpuTemp = 50 + rand() % 30;
    int gpuUsage = rand() % 100;

    mvwprintw(win, 3, 1, "Temperature: %d°C", gpuTemp);
    mvwprintw(win, 4, 1, "Usage: %d%%", gpuUsage);
    mvwprintw(win, y - 2, 1, "Press 'q' to quit");
    wrefresh(win);
}

void BaseScreen::gpuStatusScreen() {
    initscr();
    noecho();
    cbreak();

    WINDOW* win = newwin(10, 40, 10, 10);
    nodelay(win, TRUE);

    int ch;
    while ((ch = wgetch(win)) != 'q') {
        displayGpuStatus(win);
        usleep(500000); // Sleep for 0.5 seconds
    }

    delwin(win);
    endwin();
}
