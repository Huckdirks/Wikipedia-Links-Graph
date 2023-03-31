// Files to include
#include "menu.hpp"

int main() {
    menu Menu;
    return 0;
}


// Initialize menu
int menu::init_menu() {
    bool flag{};
    system("clear");

    // Initialize python script
    // Change directory to the python script
    const fs::path main_dir{fs::current_path()};
    fs::current_path(main_dir.parent_path() / "Python");
    if (system("./init_python.sh")) {  // Returns 0 if successful ¯\_(ツ)_/¯
        std::cout << "\nError initializing python script\n";
        return 0;
    }
    fs::current_path(main_dir);

    if (load()) {
        // Keep the menu running as long as the user doesn't quit
        do flag = interface();
        while (!flag);
        return 1;
    }
    return 0;
}


// Menu interface
int menu::interface() {
    std::cout << "\n\n\nWelcome to the Wiki Graph\n\nPress 1 to find the info on a single page\n2 to find the most linked to pages\n3 to find all the pages linking to a page\n0 or Q to quit\n";
    const int choice{selector()};

    switch (choice) {
        case 1:
            display_page();
            break;

        case 2:
            display_top_n();
            break;

        case 3:
            display_linked_to();
            break;

        case 0:
            system("clear");
            std::cout << "\nGoodbye!!!\n";
            return 1;

        default:
            system("clear");
            std::cout << "\nInvalid input\n";
            break;
    }
    return 0;
}


// Menu selector
int menu::selector() {
    std::string num;
    int n{};
    bool flag{};

    do {
        flag = true;
        std::cin >> num;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (num.length() == 1 && (toupper(num[0]) == 'Q' || num[0] == '0'))
            return 0;

        for (const char &c : num) {
            if (!isdigit(c)) {
                std::cout << "\nPlease enter a number\nTry again\n";
                flag = false;
                break;
            }
        }

        if (flag) {
            n = std::stoi(num);
            if (n < 0 || n > 3) {
                std::cout << "\nPlease enter a number between 0 and 2\nTry again\n";
                flag = false;
            }
        }
    } while (!flag);

    return n;
}