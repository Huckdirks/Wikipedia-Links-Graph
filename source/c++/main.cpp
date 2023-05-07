// Files to include
#include "include/menu.hpp"

// Libraries
#include <csignal>

// Signal Handler for Ctrl+C
void termination_handler(int signal) {
    indicators::show_console_cursor(true);
    std::cout << termcolor::reset;
    exit(signal);
}

// Initialize program
int main() {
    signal(SIGINT, termination_handler);
    // Runs the init_menu() function in menu_interface.cpp on initialization
    menu Menu;
    return 0;
}