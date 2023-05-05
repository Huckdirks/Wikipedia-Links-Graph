// Files to include
#include "menu.hpp"

// Initialize menu
int menu::init_menu() {
    bool flag{};
    system("clear");

    // Initialize python script
    const std::string PYTHON_PATH{fs::current_path().parent_path().string() + "/python/"};
    const std::string MAIN_PY_PATH{PYTHON_PATH + "main.py"};
    FILE *python_file{};

    Py_Initialize();

    // Add any other files besides main.py in Wikipedia_Graph/source/python/ to the python path
    PyObject* sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString(PYTHON_PATH.c_str()));

    try {
        python_file = fopen(MAIN_PY_PATH.c_str(), "r");
    } catch (const std::exception &E) {
        std::cout << "\nError loading python script\n";
        return EXIT_FAILURE;
    }
    // Run the main python script & finalize python
    PyRun_SimpleFile(python_file, MAIN_PY_PATH.c_str());
    Py_Finalize();

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
            return 1;

        case 1:
            display_page();
            break;

        case 2:
            display_wiki_info();
            break;

        case 3:
            display_top_n();
            break;

        case 4:
            display_linked_to();
            break;

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