// Files to include
#include "include/menu.hpp"

// Initialize menu
int menu::init_menu() {
    bool flag{};
    system("clear");

    // For some Goddamn reason when I try to call the python script with the official python/c api it runs on a separate thread which completely breaks literally everything (It doesn't even let me properly close out of my terminal), so I gotta go back to the shitty way of calling it ¯\_(ツ)_/¯
    // Get current directory
    const fs::path PYTHON_DIR{fs::current_path()/"source"/"python/main.py"};
    const std::string RUN_PYTHON{"python3 " + PYTHON_DIR.string()};
    if (system(RUN_PYTHON.c_str())) {
        std::cout << "\nError loading python program\n";
        return EXIT_FAILURE;
    }

    if (load()){
        std::cout << "\nError loading graph\n";
        return EXIT_FAILURE;
    }
    // Keep the menu running as long as the user doesn't quit
    do flag = interface();
    while (!flag);
    return EXIT_SUCCESS;
}


// Menu interface
int menu::interface() {
    std::cout << "\n\n\nWelcome to the Wiki Graph\n\nPress 1 to find the info on a single page\n2 to view info on the whole wiki\n3 to find the most linked to pages\n4 to find all the pages linking to a page\n0 or Q to quit\n";
    const int CHOICE{selector()};

    switch (CHOICE) {
        case 0:
            system("clear");
            std::cout << "\nGoodbye!!!\n";
            return EXIT_FAILURE;

        case 1:
            return display_page();

        case 2:
            return display_wiki_info();

        case 3:
            return display_top_n();

        case 4:
            return display_linked_to();

        default:
            system("clear");
            std::cout << "\nInvalid input\n";
            break;
    }
    return EXIT_SUCCESS;
}


// Menu selector
int menu::selector() {
    std::string num;
    unsigned int n{};
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
            if (n > 4) {
                std::cout << "\nPlease enter a number between 0 and 4\nTry again\n";
                flag = false;
            }
        }
    } while (!flag);

    return n;
}