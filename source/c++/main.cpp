// Files to include
#include "include/menu.hpp"
#include "include/util.hpp"


// Initialize program
int main() {
    signal(SIGINT, termination_handler);
    // Runs the init_menu() function in menu_interface.cpp on initialization
    menu Menu;
    return 0;
}