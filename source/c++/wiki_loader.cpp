// Files to include
#include "include/wiki_loader.hpp"

// MAKE LOAD ACCEPT A PATH TO THE DIRECTORY OF FILES TO LOAD SO IT ISN'T HARD CODED

// Load all articles into the graph
int wiki_loader::load() {
    const auto START_TIME{std::chrono::system_clock::now()};

    // Change directory to Articles-p
    const fs::path MAIN_DIR{fs::current_path()};
    try {
        fs::current_path(MAIN_DIR.parent_path().parent_path() / "data/load/");
        fs::current_path("Articles-p");
    } catch (const fs::filesystem_error &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    // Get all file names ending in .ndjson in directory
    std::vector<std::string> file_names;
    for (const auto &FILE : fs::directory_iterator(MAIN_DIR.parent_path().parent_path() / "data/load/Articles-p")) {
        if (FILE.path().extension() == ".ndjson" && fs::file_size(FILE) != 0)
                file_names.push_back(FILE.path().filename());
    }

    if (file_names.empty()) {
        std::cout << "\n\nNo files found in directory\n\n";
        return EXIT_FAILURE;
    }

    std::set<std::string> titles;   // Set of titles to make sure all titles are sorted before adding to graph
    BS::thread_pool pool;
    const auto START_TITLE_TIME{std::chrono::system_clock::now()};
    std::cout << "\nLoading " << file_names.size() << " files...\n";
    // Progress bar
    //indicators::BlockProgressBar title_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    //unsigned int progress{};    // I would make this an atomic but it breaks pool.submit()
    std::vector <std::future<std::set<std::string>>> title_futures;
    //double percent{};
    //indicators::show_console_cursor(false); // Hide cursor
    // Load in each file's titles to a set to make it ordered
    //for (const auto &FILE : file_names)
    for (unsigned int i{}; i < file_names.size(); ++i){
        title_futures.push_back(pool.submit([this, file_names, i]() -> std::set<std::string> {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::set<std::string> titles;
            const std::string FILE_NAME{file_names[i]};

            try {
                file_in.open(FILE_NAME);
                file_in.peek();

                // Check if file is empty or if not able to open
                while (!file_in.eof())
                    load_title(titles, file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n";
                return titles;
            }
            //++progress;
            return titles;
        }));
    }
    pool.wait_for_tasks();
    for (auto &title_future : title_futures)
        titles.merge(title_future.get());

    std::cout << "\nLoaded " << titles.size() << " titles in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - START_TITLE_TIME).count() << " seconds";
        //title_bar.tick();
    /* // Load in each file's titles to a set to make it ordered
    for (const auto &FILE : file_names) {
        percent = 100 * ((double)progress / (file_names.size() - 1));
        if (title_bar.is_completed() || percent >= 100) {
            title_bar.set_option(indicators::option::ShowRemainingTime{false});
            title_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
        }
        title_bar.set_progress(percent);

        try {
            file_in.open(FILE);
            file_in.peek();

            // Check if file is empty or if not able to open
            while (!file_in.eof())
                load_title(titles);

            file_in.close();
        } catch (const std::ifstream::failure &E) {
            std::cerr << E.what() << "\n";
            return EXIT_FAILURE;
        }
        ++progress;
    } */

    if (titles.empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles found in files\n\n";
        return EXIT_FAILURE;
    }

    // Show cursor
    std::cout << termcolor::reset << "\n\nLoading " << titles.size() << " titles into the graph...\n";
    indicators::BlockProgressBar graph_titles_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    indicators::show_console_cursor(false); // Hide cursor
    unsigned int progress{};
    double percent{};
    //percent = 0;

    // Load in each title to the graph
    for (const auto &TITLE : titles) {
        percent = 100 * ((double)progress / (titles.size() - 1));
        if (progress % 10000 == 0 || percent >= 100) {  // Only update progress bar every 1000 titles to save time
            if (graph_titles_bar.is_completed() || percent >= 100) {
                graph_titles_bar.set_option(indicators::option::ShowRemainingTime{false});
                graph_titles_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            graph_titles_bar.set_progress(percent);
        }
        graph_vertex page;
        page.title = TITLE;
        graph->push_back(page);
        ++progress;
    }

    if (graph->empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles loaded into graph\n\n";
        return EXIT_FAILURE;
    }

    std::cout << termcolor::reset << "\n\nLoading links into graph (Adjacent Nodes)...\n";
    auto START_LINK_TIME = std::chrono::system_clock::now();
    //indicators::BlockProgressBar links_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    //progress = 0;

    // Load in each file's links
    for (unsigned int i{}; i < file_names.size(); ++i) {
        pool.push_task([this, file_names, i]() {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            const std::string FILE_NAME{file_names[i]};

            try {
                file_in.open(FILE_NAME);
                file_in.peek();

                while (!file_in.eof())
                    load_links(file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n\n";
            }
            return;
        });
    }
    pool.wait_for_tasks();

    /* // Load in each file's links
    for (const auto &FILE : file_names) {
        try {
            file_in.open(FILE);
            file_in.peek();
            
            while (!file_in.eof())
                load_page_links(links_bar, progress);

            file_in.close();
        } catch (const std::ifstream::failure &E) {
            std::cerr << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
    } */

    indicators::show_console_cursor(true);
    std::cout << termcolor::reset << "\nLinks loaded in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - START_LINK_TIME).count() << " seconds, or " << (float)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - START_LINK_TIME).count() / 60 << " minutes!!!\n\n";
    auto file_load_time{std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - START_TIME).count()};
    std::cout << termcolor::reset << "\n\n" << graph->size() << " articles loaded from file in " << file_load_time << " seconds, or " << (float)file_load_time / 60 << " minutes!!!\n\n";
    try {
        fs::current_path(MAIN_DIR);
    } catch (const fs::filesystem_error &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    fs::current_path(MAIN_DIR);
    return EXIT_SUCCESS;
}


// Load in a title to the set
inline int wiki_loader::load_title(std::set<std::string> &titles, std::ifstream &file_in) {
    std::string JSON_line;
    try {
        std::getline(file_in, JSON_line);
    } catch (const std::exception &e) {
        std::cerr << "\n\nError loading line from file\n\n";
        return EXIT_FAILURE;
    }
    //std::getline(file_in, JSON_line);

    file_in.peek();
    const json JSON = json::parse(JSON_line);  // { } Initialization breaks this ¯\_(ツ)_/¯

    const std::string TITLE{JSON[0].get<std::string>()};
    titles.insert(TITLE);
    return EXIT_SUCCESS;
}


// Load in a link to the graph
//inline int wiki_loader::load_page_links(indicators::BlockProgressBar &bar, unsigned int &progress) {
inline int wiki_loader::load_links(std::ifstream &file_in) {
    //const double PERCENT{100 * ((double)progress / (graph->size() - 1))};
    std::string JSON_line;

    /* if (progress % 1000 == 0 || PERCENT >= 100) {  // Only update progress bar every 1000 titles to save time
        if (!bar.is_completed()) {
            if (PERCENT >= 100) {
                bar.set_option(indicators::option::ShowRemainingTime{false});
                bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bar.set_progress(PERCENT);
        }
    } */
    
    try {
        std::getline(file_in, JSON_line);
    } catch (const std::ifstream::failure &E) {
        std::cerr << E.what() << "\n\n";
        //std::cerr << "\n\nError loading line from file\n\n";
        return EXIT_FAILURE;
    }
    file_in.peek();
    const json JSON{};
    const json *JSON_ptr{&JSON};
    json *change_ptr{const_cast<json *>(JSON_ptr)};
    try {
        *change_ptr = json::parse(JSON_line);
        //const json JSON = json::parse(JSON_line);  // { } Initialization breaks this ¯\_(ツ)_/¯
    } catch (const json::parse_error &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    //const json JSON = json::parse(JSON_line);  // { } Initialization breaks this ¯\_(ツ)_/¯
    const std::string TITLE{JSON[0].get<std::string>()};

    //++progress;
    graph_vertex *page{graph->find(TITLE)};
    if (page == nullptr)
        return EXIT_FAILURE;
        
    for (const auto &LINK : JSON[1]) {
        graph_vertex *adjacent_page{graph->find(LINK.get<std::string>())};
        if (adjacent_page == nullptr)
                continue;

        page->adjacent.push_back(adjacent_page);    // Shit breaks right here when this function is ran parallel (it's giving an allocation error for the vector so I might just have to cut my losses and not do this parallel)
        ++page->links;
        ++adjacent_page->linked_to;
        ++graph->num_edges;
    }
    page->adjacent.shrink_to_fit();  // Shrink the adjacent vector to fit the number of adjacent nodes (good for memory 👍)
    return EXIT_SUCCESS;
}