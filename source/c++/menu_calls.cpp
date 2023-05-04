// Files to include
#include "menu.hpp"

// Load data
int menu_calls::load() {
    return graph.load();
}


// Display info on a page
int menu_calls::display_page() {
    std::string title;

    system("clear");
    std::cout << "\nWhat page would you like to display?\n";
    try {
        std::getline(std::cin, title);
    } catch (const std::exception &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    //std::getline(std::cin, title);

    system("clear");
    const auto PAGE = graph.find(title);
    if (PAGE == nullptr) {
        std::cout << "\nPage not found\n\n";
        return EXIT_FAILURE;
    }

    std::cout << "\nDo you want to also display " << PAGE->title << "'s links to pages?\n";
    const bool RESPONSE{y_or_n()};
    system("clear");
    PAGE->display(RESPONSE);
    return EXIT_SUCCESS;
}


// Display top n
int menu_calls::display_top_n() {
    std::string num;
    unsigned int n{};
    bool is_digit{};
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nHow many pages would you like to display/save?\n";

    // Check if input is a number
    do {
        is_digit = true;
        try {
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::exception &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //std::cin >> num;
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        for (const char &C : num) {
            if (!isdigit(C)) {
                system("clear");
                std::cout << "\nPlease enter a number\nTry again\n";
                is_digit = false;
                break;
            }
        }
    } while (!is_digit);
    n = std::stoi(num);
    if (n > graph.size())   // Make sure it doesn't go out of bounds
        n = graph.size();

    system("clear");
    std::cout << "Would you like to display or save the top " << n << " linked to pages to a file?\nPress D for display and S for save\n";
    char response{};
    do {
        try {
            std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::exception &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //std::cin >> response;
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        response = toupper(response);
        if (response != 'D' && response != 'S'){
            system("clear");
            std::cout << "\nPlease enter D or S\n";
        }
    } while (response != 'D' && response != 'S');
    if (response == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?";
        csv = y_or_n();
    }

    system("clear");
    std::cout << "Finding top " << n << " most linked to pages...\n";
    const auto START_TIME{std::chrono::high_resolution_clock::now()};
    const auto TOP_N_LIST = graph.top_n(n);
    const auto END_TIME{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - START_TIME).count()};

    system("clear");
    if (!save) {
        std::cout << "\nTop " << n << " most linked to pages:" << "\n\n";
        for (const auto &PAGE : TOP_N_LIST)
            PAGE->display(false);
    } else {
        std::ofstream file_out;
        file_out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        // Change directory to Articles-p
        const fs::path MAIN_DIR{fs::current_path()};
        try {
            fs::current_path(MAIN_DIR.parent_path().parent_path() / "data/");
        } catch (const fs::filesystem_error &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //fs::current_path(MAIN_DIR.parent_path().parent_path() / "data/");
        /* if (!fs::exists("user_data"))
            fs::create_directory("user_data");
        fs::current_path("user_data"); */
        try {
            fs::current_path("user_data");
        } catch (const fs::filesystem_error &E) {
            try {
                fs::create_directory("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            //fs::create_directory("user_data");
            try {
                fs::current_path("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            //std::cerr << "\nError: " << E.what() << "\n\n";
            //return EXIT_FAILURE;
        }

        try {
            fs::current_path("top_linked_articles");
        } catch (const fs::filesystem_error &E) {
            try {
                fs::create_directory("top_linked_articles");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            //fs::create_directory("top_linked_articles");
            try {
                fs::current_path("top_linked_articles");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            //std::cerr << "\nError: " << E.what() << "\n\n";
            //return EXIT_FAILURE;
        }
        /* if (!fs::exists("top_linked_articles"))
            fs::create_directory("top_linked_articles");
        fs::current_path("top_linked_articles"); */

        if (csv) {
            std::string file_name{"top_" + std::to_string(n) + "_linked_articles.csv"};
            /* file_out.open(file_name);
            file_out << "Title,# Links To\n";
            for (const auto &PAGE : TOP_N_LIST)
                file_out << PAGE->title << ',' << PAGE->linked_to << '\n'; */
            try {
                file_out.open(file_name);
                file_out << "Title,# Links To\n";
                for (const auto &PAGE : TOP_N_LIST)
                    file_out << PAGE->title << ',' << PAGE->linked_to << '\n';
            } catch (const std::ofstream::failure &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                if (file_out.is_open())
                    file_out.close();
                return EXIT_FAILURE;
            }
        } else {
            std::string file_name{"top_" + std::to_string(n) + "_linked_articles.txt"};
            unsigned int place{1};
            try {
                file_out.open(file_name);
                for (const auto &PAGE : TOP_N_LIST) {
                    file_out << place << ") " << PAGE->title << ": " << PAGE->linked_to << '\n';
                    ++place;
                }
            } catch (const std::ofstream::failure &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                if (file_out.is_open())
                    file_out.close();
                return EXIT_FAILURE;
            }
            /* file_out.open(file_name);
            for (const auto &PAGE : TOP_N_LIST) {
                file_out << place << ") " << PAGE->title << ": " << PAGE->linked_to << '\n';
                ++place;
            } */
        }
        if (file_out.is_open())
            file_out.close();
        try {
            fs::current_path(MAIN_DIR);
        } catch (const fs::filesystem_error &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //fs::current_path(MAIN_DIR);
    }

    std::cout << "\nTop " << n << " most linked to pages found in " << (float)END_TIME / 1000 << " seconds, or " << ((double)(END_TIME / 1000) / 60) << " minutes\n\n";
    return EXIT_SUCCESS;
}


// Display all pages linking to a page
int menu_calls::display_linked_to() {
    std::string title;
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nWhat page would you like to find all the other pages linking to?\n";
    try {
        std::getline(std::cin, title);
    } catch (const std::ios_base::failure &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    //std::getline(std::cin, title);

    // Check if the page actually exists
    if (graph.find(title) == nullptr) {
        system("clear");
        std::cout << "Page not found\n\n";
        return EXIT_FAILURE;
    }

    system("clear");
    std::cout << "Would you like to display or save the pages linking to " << title << "?\nPress D for display and S for save\n";
    char response{};
    do {
        try {
            std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::ios_base::failure &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //std::cin >> response;
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        response = toupper(response);
        if (response != 'D' && response != 'S'){
            system("clear");
            std::cout << "\nPlease enter D or S\n";
        }
    } while (response != 'D' && response != 'S');
    if (response == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?\nPress Y for yes and N for no\n";
        csv = y_or_n();
    }

    system("clear");
    const auto LINKED_TO{graph.linked_to(title)};
    if (LINKED_TO.empty()) {
        std::cout << "\nNo pages link to " << title << ", or " << title << " isn't found\n";
        return EXIT_FAILURE;
    }

    if (!save) {
        std::cout << "\nPages that link to " << title << ":\n";
        for (unsigned int i{}; i < LINKED_TO.size(); ++i)
            std::cout << i + 1 << ") " << LINKED_TO[i]->title << '\n';
    } else {
        std::ofstream file_out;
        file_out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        // Change directory to Articles-p
        fs::path MAIN_DIR{fs::current_path()};
        fs::current_path(MAIN_DIR.parent_path().parent_path() / "data/");
        
        try {
            fs::current_path("user_data");
        } catch (const fs::filesystem_error &E) {
            fs::create_directory("user_data");
            try {
                fs::current_path("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            //std::cerr << "\nError: " << E.what() << "\n\n";
            //return EXIT_FAILURE;
        }

        /* if (!fs::exists("user_data"))
            fs::create_directory("user_data");
        fs::current_path("user_data"); */

        if (!fs::exists("pages_linking_to"))
            fs::create_directory("pages_linking_to");
        fs::current_path("pages_linking_to");

        std::string save_title{title};
        std::replace(save_title.begin(), save_title.end(), ' ', '_');  // Replace spaces with underscores in the title for ✨𝒻ℴ𝓇𝓂𝒶𝓉𝓉𝒾𝓃ℊ✨

        if (csv) {
            std::string file_name{"pages_linking_to_" + save_title + ".csv"};
            file_out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            try {
                file_out.open(file_name);
                file_out << "Page,Pages linking to page\n" << title << ',';
                for (const auto &PAGE : LINKED_TO)
                    file_out << PAGE->title << ',';
            } catch (const std::ofstream::failure &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                if (file_out.is_open())
                    file_out.close();
                return EXIT_FAILURE;
            }
        } else {
            std::string file_name{"pages_linking_to_" + save_title + ".txt"};
            unsigned int place{1};
            file_out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            try {
                file_out.open(file_name);
                file_out << "Pages that link to " << title << ":\n";
                for (const auto &PAGE : LINKED_TO) {
                    file_out << place << ") " << PAGE->title << '\n';
                    ++place;
                }
            } catch (const std::ofstream::failure &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                if (file_out.is_open())
                    file_out.close();
                return EXIT_FAILURE;
            }
        }
        /* if (csv) {
            std::string file_name{"pages_linking_to_" + save_title + ".csv"};
            file_out.open(file_name);
            file_out << "Page,Pages linking to page\n" << title << ',';
            for (const auto &PAGE : LINKED_TO)
                file_out << PAGE->title << ',';
        } else {
            std::string file_name{"pages_linking_to_" + save_title + ".txt"};
            unsigned int place{1};
            file_out.open(file_name);
            file_out << "Pages that link to " << title << ":\n";
            for (const auto &PAGE : LINKED_TO) {
                file_out << place << ") " << PAGE->title << '\n';
                ++place;
            }
        } */
        if (file_out.is_open())
            file_out.close();
        fs::current_path(MAIN_DIR);
        std::cout << "\nPages that link to " << title << " saved\n";
    }
    return EXIT_SUCCESS;
}


// Display info about the Wikipedia dump
void menu_calls::display_wiki_info(){
    system("clear");
    std::cout << "\nThis program uses the Wikipedia XML dump\n";
    std::cout << "The dump can be found at https://dumps.wikimedia.org/enwiki/latest/\n";
    std::cout << "The dump is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License\n";
    std::cout << "The full license can be found at https://creativecommons.org/licenses/by-sa/3.0/legalcode\n";

    std::cout << "\nWikipedia contains " << graph.size() << " pages and " << graph.num_edges << " links to other Wikipedia pages\n";
    return;
}


// Ask yes or no
bool menu_calls::y_or_n() {
    char response;
    std::cout << "\nPress Y for yes and anything else for no\n";
    std::cin >> response;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (toupper(response) == 'Y')
        return true;
    return false;
}