#include "baseScreen.h"
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

// Function to simulate GPU status display using ncurses
void BaseScreen::gpuStatusScreen() {
    initscr();            // Start ncurses mode
    noecho();             // Disable echoing
    curs_set(FALSE);      // Hide the cursor
    int row, col;
    getmaxyx(stdscr, row, col);  // Get screen size

    mvprintw(row / 2, (col - 20) / 2, "GPU Status: Active");  // Display GPU status
    refresh();
    sleep(2);  // Pause for 2 seconds

    endwin();  // End ncurses mode
}
