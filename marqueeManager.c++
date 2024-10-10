#include "marqueeManager.h"
#include <ncurses.h>
#include <string>
#include <chrono>
#include <thread>

// Function to display bouncing marquee text
void bouncingMarquee(const std::string& text, int speed) {
    int row, col;
    initscr();            // Initialize ncurses mode
    noecho();             // Don't echo input
    curs_set(1);          // Show the cursor for typing
    nodelay(stdscr, TRUE); // Non-blocking input
    keypad(stdscr, TRUE);  // Enable keypad input

    getmaxyx(stdscr, row, col);  // Get screen size

    int x = col / 2, y = row / 2;  // Start in the center
    int x_dir = 1, y_dir = 1;      // Initial movement directions
    int len = text.length();

    // Create the header with the "sign" design
    std::string headerText = "Displaying a marquee console";
    std::string borderTopBottom = "+" + std::string(headerText.length() + 2, '=') + "+";
    std::string borderSides = "||" + headerText + "||";

    std::string commandPrompt = "Enter a command for MARQUEE_CONSOLE: ";
    std::string commandProcessed = "Command processed in MARQUEE_CONSOLE: ";
    std::string userInput;  // To hold user input
    std::string lastCommand; // To hold the last command processed

    // Command input area boundaries
    int commandRow = row - 4;
    mvprintw(commandRow, 0, "+-----------------------------------------------------+");
    mvprintw(commandRow + 1, 0, "|                                                     |");
    mvprintw(commandRow + 2, 0, "+-----------------------------------------------------+");

    // Bounce height limits
    int maxBounceHeight = commandRow - 2;
    int minBounceHeight = 3;

    while (true) {
        int ch = getch();
        if (ch == 'q') {  // Exit if 'q' is pressed
            break;
        }
        if (ch == '\n') {  // Process command on Enter
            if (!userInput.empty()) {
                lastCommand = userInput;
                userInput.clear();
            }
        } else if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
            if (!userInput.empty()) {
                userInput.pop_back();
            }
        } else if (ch >= 32 && ch <= 126) {  // Add printable characters to input
            userInput += static_cast<char>(ch);
        }

        clear();  // Clear screen

        // Display header with borders
        mvprintw(0, (col - borderTopBottom.length()) / 2, "%s", borderTopBottom.c_str());
        mvprintw(1, (col - borderSides.length()) / 2, "%s", borderSides.c_str());
        mvprintw(2, (col - borderTopBottom.length()) / 2, "%s", borderTopBottom.c_str());

        // Print bouncing text
        mvprintw(y, x, "%s", text.c_str());

        // Move text
        x += x_dir;
        y += y_dir;

        // Bounce off screen edges
        if (x <= 0 || x + len >= col) x_dir = -x_dir;
        if (y <= minBounceHeight || y >= maxBounceHeight) y_dir = -y_dir;

        // Display command prompt with user input
        mvprintw(commandRow, 1, "%s%s", commandPrompt.c_str(), userInput.c_str());

        // Display last processed command
        if (!lastCommand.empty()) {
            mvprintw(commandRow + 1, 1, "%s%s", commandProcessed.c_str(), lastCommand.c_str());
        } else {
            mvprintw(commandRow + 1, 1, "%s", commandProcessed.c_str());
        }

        refresh();  // Update screen
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));  // Delay
    }

    endwin();  // End ncurses mode
}
