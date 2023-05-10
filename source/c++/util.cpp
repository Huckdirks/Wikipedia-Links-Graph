// Files to include
#include "include/util.hpp"


// Signal Handler for Ctrl+C
void termination_handler(int signal) {
    indicators::show_console_cursor(true);
    std::cout << termcolor::reset;
    exit(signal);
}