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
    std::getline(std::cin, title);

    system("clear");
    const auto page = graph.find(title);
    if (page == nullptr) {
        std::cout << "\nPage not found\n\n";
        return 0;
    }

    std::cout << "\nDo you want to also display " << page->title << "'s links to pages?\n";
    const bool response{y_or_n()};
    system("clear");
    page->display(response);

    return 1;
}


// Display top n
int menu_calls::display_top_n() {
    std::string num;
    bool flag{};
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nHow many pages would you like to display/save?\n";

    // Check for valid input
    do {
        flag = true;
        std::cin >> num;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (const char &c : num) {
            if (!isdigit(c)) {
                std::cout << "\nPlease enter a number\nTry again\n";
                flag = false;
                break;
            }
        }

    } while (!flag);
    int n{std::stoi(num)};

    system("clear");
    std::cout << "Would you like to display or save the top " << n << " linked to pages to a file?\nPress D for display and S for save\n";
    char response{};
    do {
        std::cin >> response;
        response = toupper(response);
        if (response != 'D' && response != 'S')
            std::cout << "\nPlease enter D or S\n";
    } while (response != 'D' && response != 'S');
    if (response == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?";
        csv = y_or_n();
    }

    system("clear");
    std::cout << "Finding top " << n << " most linked to pages...\n";
    const auto start{std::chrono::high_resolution_clock::now()};
    const auto top_n_list = graph.top_n(n);
    const auto end{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()};

    system("clear");
    if (!save) {
        std::cout << "\nTop " << n << " most linked to pages:" << "\n\n";
        for (const auto &page : top_n_list)
            page->display(false);
    } else {
        std::ofstream file_out;
        // Change directory to Articles-p
        const fs::path main_dir{fs::current_path()};
        fs::current_path(main_dir.parent_path().parent_path() / "data/");
        if (!fs::exists("user_data"))
            fs::create_directory("user_data");
        fs::current_path("user_data");

        if (!fs::exists("top_linked_articles"))
            fs::create_directory("top_linked_articles");
        fs::current_path("top_linked_articles");

        if (csv) {
            std::string file_name{"top_" + std::to_string(n) + "_linked_articles.csv"};
            file_out.open(file_name);
            file_out << "Title,# Links To\n";
            for (const auto &page : top_n_list)
                file_out << page->title << ',' << page->links_to << '\n';
        } else {
            std::string file_name{"top_" + std::to_string(n) + "_linked_articles.txt"};
            int place{1};
            file_out.open(file_name);
            for (const auto &page : top_n_list) {
                file_out << place << ") " << page->title << ": " << page->links_to << '\n';
                ++place;
            }
        }
        if (file_out.is_open())
            file_out.close();
        fs::current_path(main_dir);
    }

    std::cout << "\nTop " << n << " most linked to pages found in " << (float)end / 1000 << " seconds, or " << ((double)(end / 1000) / 60) << " minutes\n\n";
    return 1;
}


// Display all pages linking to a page
int menu_calls::display_linked_to() {
    std::string title;
    bool save{};
    bool csv{};

    system("clear");
    std::cout << "\nWhat page would you like to find all the other pages linking to?\n";
    std::getline(std::cin, title);

    // Check if the page actually exists
    if (graph.find(title) == nullptr) {
        system("clear");
        std::cout << "Page not found\n\n";
        return 0;
    }

    system("clear");
    std::cout << "Would you like to display or save the pages linking to " << title << "?\nPress D for display and S for save\n";
    char response{};
    do {
        std::cin >> response;
        response = toupper(response);
        if (response != 'D' && response != 'S')
            std::cout << "\nPlease enter D or S\n";
    } while (response != 'D' && response != 'S');
    if (response == 'S') {
        save = true;
        system("clear");
        std::cout << "Would you like to save as a .csv file instead of a .txt?\nPress Y for yes and N for no\n";
        csv = y_or_n();
    }

    system("clear");
    const auto linked_to{graph.linked_to(title)};
    if (linked_to.empty()) {
        std::cout << "\nNo pages link to " << title << ", or " << title << " isn't found\n";
        return 0;
    }

    if (!save) {
        std::cout << "\nPages that link to " << title << ":\n";
        for (unsigned int i{}; i < linked_to.size(); ++i)
            std::cout << i + 1 << ") " << linked_to[i]->title << '\n';
    } else {
        std::ofstream file_out;
        // Change directory to Articles-p
        const fs::path main_dir{fs::current_path()};
        fs::current_path(main_dir.parent_path().parent_path() / "data/");
        if (!fs::exists("user_data"))
            fs::create_directory("user_data");
        fs::current_path("user_data");

        if (!fs::exists("pages_linking_to"))
            fs::create_directory("pages_linking_to");
        fs::current_path("pages_linking_to");

        std::string save_title{title};
        std::replace(save_title.begin(), save_title.end(), ' ', '_');  // Replace spaces with underscores in the title for ✨𝒻ℴ𝓇𝓂𝒶𝓉𝓉𝒾𝓃ℊ✨

        if (csv) {
            std::string file_name{"pages_linking_to_" + save_title + ".csv"};
            file_out.open(file_name);
            file_out << "Page,Pages linking to page\n" << title << ',';
            for (const auto &page : linked_to)
                file_out << page->title << ',';
        } else {
            std::string file_name{"pages_linking_to_" + save_title + ".txt"};
            int place{1};
            file_out.open(file_name);
            file_out << "Pages that link to " << title << ":\n";
            for (const auto &page : linked_to) {
                file_out << place << ") " << page->title << '\n';
                ++place;
            }
        }
        if (file_out.is_open())
            file_out.close();
        fs::current_path(main_dir);
        std::cout << "\nPages that link to " << title << " saved\n";
    }
    return 1;
}


// Ask yes or no
bool menu_calls::y_or_n() {
    char response;
    std::cout << "\nPress Y for yes and anything else for no\n";
    std::cin >> response;

    if (toupper(response) == 'Y')
        return true;
    return false;
}