// Files to include
#include "include/menu.hpp"

// Load data
int menu_calls::load() {
    return graph.load();
}


// Display info on a page
int menu_calls::display_page() {
    const std::string TITLE;

    system("clear");
    std::cout << "\nWhat page would you like to display?\n";
    try {
        const std::string *TITLE_PTR{&TITLE};
        //const std::string *LINE_PTR{&TITLE};
        //std::string *change_ptr{const_cast<std::string *>(LINE_PTR)};
        std::string *change_ptr{const_cast<std::string *>(TITLE_PTR)};
        //std::string change_ptr{const_cast<std::string &>(*LINE_PTR)};
        std::getline(std::cin, *change_ptr);
        //std::getline(std::cin, title);
    } catch (const std::exception &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    system("clear");
    //const auto PAGE = graph.find(title);
    const auto PAGE = graph.find(std::move(TITLE));
    if (PAGE == nullptr) {
        std::cerr << "\nPage not found\n\n";
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
    //std::string num;
    const std::string NUM;
    const unsigned int N{};
    bool is_digit{};
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nHow many pages would you like to display/save?\n";

    // Check if input is a number
    do {
        is_digit = true;
        try {
            const std::string *NUM_PTR{&NUM};
            std::string *change_ptr{const_cast<std::string *>(NUM_PTR)};
            std::cin >> *change_ptr;
            //std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::exception &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        //std::cin >> num;
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        //for (const char &C : num) {
        for (const char &C : NUM) {
            if (!isdigit(C)) {
                system("clear");
                std::cout << "\nPlease enter a number\nTry again\n";
                is_digit = false;
                break;
            }
        }
    } while (!is_digit);
    try {
        const unsigned int *N_PTR{&N};
        unsigned int *change_ptr{const_cast<unsigned int *>(N_PTR)};
        *change_ptr = std::stoi(NUM);
    } catch (const std::exception &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    //n = std::stoi(NUM);
    //n = std::stoi(num);
    if (N > graph.size()) {   // Make sure it doesn't go out of bounds
        const unsigned int *N_PTR{&N};
        unsigned int *change_ptr{const_cast<unsigned int *>(N_PTR)};
        *change_ptr = graph.size();
        //N = graph.size();
    }

    system("clear");
    std::cout << "Would you like to display or save the top " << N << " linked to pages to a file?\nPress D for display and S for save\n";
    char response{};
    //const char RESPONSE{};
    do {
        try {
            //const char *RESPONSE_PTR{&RESPONSE};
            //char *change_ptr{const_cast<char *>(RESPONSE_PTR)};
            //std::cin >> *change_ptr;
            std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            //*change_ptr = toupper(*change_ptr);
        } catch (const std::exception &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }

        response = toupper(response);
        //if (RESPONSE != 'D' && RESPONSE != 'S'){
        if (response != 'D' && response != 'S'){
            system("clear");
            std::cout << "\nPlease enter D or S\n";
        }
    } while (response != 'D' && response != 'S');
    //} while (RESPONSE != 'D' && RESPONSE != 'S');
    //if (RESPONSE == 'S') {
    if (response == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?";
        csv = y_or_n();
    }

    system("clear");
    std::cout << "Finding top " << N << " most linked to pages...\n";
    const auto START_TIME{std::chrono::high_resolution_clock::now()};
    const auto TOP_N_LIST = graph.top_n(N);
    const auto END_TIME{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - START_TIME).count()};

    system("clear");
    if (!save) {
        std::cout << "\nTop " << N << " most linked to pages:" << "\n\n";
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
        try {
            fs::current_path("user_data");
        } catch (const fs::filesystem_error &E) {
            try {
                fs::create_directory("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            try {
                fs::current_path("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
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
            try {
                fs::current_path("top_linked_articles");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
        }

        if (csv) {
            std::string file_name{"top_" + std::to_string(N) + "_linked_articles.csv"};
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
            std::string file_name{"top_" + std::to_string(N) + "_linked_articles.txt"};
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
        }
        if (file_out.is_open())
            file_out.close();
        try {
            fs::current_path(MAIN_DIR);
        } catch (const fs::filesystem_error &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "\nTop " << N << " most linked to pages found in " << (float)END_TIME / 1000 << " seconds, or " << ((double)(END_TIME / 1000) / 60) << " minutes\n\n";
    return EXIT_SUCCESS;
}


// Display all pages linking to a page
int menu_calls::display_linked_to() {
    //std::string title;
    const std::string TITLE;
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nWhat page would you like to find all the other pages linking to?\n";
    try {
        const std::string *TITLE_PTR{&TITLE};
        std::string *change_ptr{const_cast<std::string *>(TITLE_PTR)};
        std::getline(std::cin, *change_ptr);
        //std::getline(std::cin, title);
    } catch (const std::ios_base::failure &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    /* // Check if the page actually exists
    if (graph.find(title) == nullptr) {
        system("clear");
        std::cerr << "Page not found\n\n";
        return EXIT_FAILURE;
    } */

    system("clear");
    std::cout << "Would you like to display or save the pages linking to " << TITLE << "?\nPress D for display and S for save\n";
    /* char response{};
    do {
        try {
            std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (const std::ios_base::failure &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }

        response = toupper(response);
        if (response != 'D' && response != 'S'){
            system("clear");
            std::cout << "\nPlease enter D or S\n";
        }
    } while (response != 'D' && response != 'S'); */
    const char RESPONSE{};
    do {
        try {
            const char *RESPONSE_PTR{&RESPONSE};
            char *change_ptr{const_cast<char *>(RESPONSE_PTR)};
            std::cin >> *change_ptr;
            //std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            *change_ptr = toupper(*change_ptr);
        } catch (const std::exception &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }

        //response = toupper(response);
        if (RESPONSE != 'D' && RESPONSE != 'S'){
            system("clear");
            std::cout << "\nPlease enter D or S\n";
        }
    } while (RESPONSE != 'D' && RESPONSE != 'S');
    if (RESPONSE == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?";
        csv = y_or_n();
    }

    system("clear");
    const auto LINKED_TO{graph.linked_to(TITLE)};
    if (LINKED_TO.empty()) {
        std::cerr << "\nNo pages link to " << TITLE << ", or " << TITLE << " isn't found\n";
        return EXIT_FAILURE;
    }

    if (!save) {
        std::cout << "\nPages that link to " << TITLE << ":\n";
        for (unsigned int i{}; i < LINKED_TO.size(); ++i)
            std::cout << i + 1 << ") " << LINKED_TO[i]->title << '\n';
    } else {
        std::ofstream file_out;
        file_out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        // Change directory to Articles-p
        fs::path MAIN_DIR{fs::current_path()};
        try {
            fs::current_path(MAIN_DIR.parent_path().parent_path() / "data/");
        } catch (const fs::filesystem_error &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        
        try {
            fs::current_path("user_data");
        } catch (const fs::filesystem_error &E) {
            try {
                fs::create_directory("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            try {
                fs::current_path("user_data");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
        }

        try {
            fs::current_path("pages_linking_to");
        } catch (const fs::filesystem_error &E) {
            try {
                fs::create_directory("pages_linking_to");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            try {
                fs::current_path("pages_linking_to");
            } catch (const fs::filesystem_error &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
        }

        std::string save_title{TITLE};
        std::replace(save_title.begin(), save_title.end(), ' ', '_');  // Replace spaces with underscores in the title for ✨𝒻ℴ𝓇𝓂𝒶𝓉𝓉𝒾𝓃ℊ✨

        if (csv) {
            std::string file_name{"pages_linking_to_" + save_title + ".csv"};
            try {
                file_out.open(file_name);
                file_out << "Page,Pages linking to page\n" << TITLE << ',';
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
            try {
                file_out.open(file_name);
                file_out << "Pages that link to " << TITLE << ":\n";
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
        if (file_out.is_open())
            file_out.close();
        try {
            fs::current_path(MAIN_DIR);
        } catch (const fs::filesystem_error &E) {
            std::cerr << "\nError: " << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
        std::cout << "\nPages that link to " << TITLE << " saved\n";
    }
    return EXIT_SUCCESS;
}


// Display info about the Wikipedia dump
int menu_calls::display_wiki_info(){
    system("clear");
    std::cout << "\nThis program uses the Wikipedia XML dump\n";
    std::cout << "The dump can be found at https://dumps.wikimedia.org/enwiki/latest/\n";
    std::cout << "The dump is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License\n";
    std::cout << "The full license can be found at https://creativecommons.org/licenses/by-sa/3.0/legalcode\n";

    std::cout << "\nWikipedia contains " << graph.size() << " pages and " << graph.num_edges << " links to other Wikipedia pages\n";
    return EXIT_SUCCESS;
}


// Ask yes or no
bool menu_calls::y_or_n() {
    char response;
    std::cout << "\nPress Y for yes and anything else for no\n";
    try {
        std::cin >> response;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } catch (const std::ios_base::failure &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        std::cin.clear();   // Clear the error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;   // Not an essential function so we can just return false
    }

    if (toupper(response) == 'Y')
        return true;
    return false;
}