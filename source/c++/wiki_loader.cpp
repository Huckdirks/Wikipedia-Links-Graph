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

    unsigned int progress{};
    //double percent{};
    indicators::BlockProgressBar titles_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

    std::set<std::string> titles;   // Set of titles to make sure all titles are sorted before adding to graph
    BS::thread_pool pool;
    std::vector <std::future<std::set<std::string>>> title_futures;
    const auto START_TITLE_TIME{std::chrono::system_clock::now()};

    std::cout << "\nLoading Wikipedia page titles from " << file_names.size() << " files...\n";
    for (unsigned int i{}; i < file_names.size(); ++i){
        /* title_futures.push_back(pool.submit([this, file_names, i]() -> std::set<std::string> {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::set<std::string> titles;
            const std::string FILE_NAME{file_names[i]};

            try {
                file_in.open(FILE_NAME);
                file_in.peek();

                while (!file_in.eof())
                    load_title(titles, file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n";
                return titles;
            }
            return titles;
        })); */
        title_futures.push_back(pool.submit([this, file_names, i, &progress, &titles_bar]() -> std::set<std::string> {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::set<std::string> titles;
            const std::string FILE_NAME{file_names[i]};

            double percent{100 * ((double)progress / (titles.size() - 1))};
            if (progress % 10000 == 0 || percent >= 100) {  // Only update progress bar every 1000 titles to save time
                if (titles_bar.is_completed() || percent >= 100) {
                    titles_bar.set_option(indicators::option::ShowRemainingTime{false});
                    titles_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                }
                titles_bar.set_progress(percent);
            }

            try {
                file_in.open(FILE_NAME);
                file_in.peek();

                while (!file_in.eof())
                    load_title(titles, file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n";
                return titles;
            }
            ++progress;
            return titles;
        }));
    }
    pool.wait_for_tasks();
    for (auto &title_future : title_futures)
        titles.merge(title_future.get());

    if (titles.empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles found in files\n\n";
        return EXIT_FAILURE;
    }
    std::cout << termcolor::reset << "Loaded " << titles.size() << " titles in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - START_TITLE_TIME).count() << " seconds";
    indicators::show_console_cursor(true);

    std::cout << "\n\nLoading " << titles.size() << " titles into the graph...\n";
    indicators::BlockProgressBar graph_titles_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    indicators::show_console_cursor(false); // Hide cursor

    //unsigned int progress{};
    double percent{};

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
        /* graph_vertex page;
        page.title = TITLE;
        graph->push_back(page); */
        //graph_vertex page{TITLE};
        //graph_vertex page{TITLE};
        const graph_vertex PAGE{std::move(TITLE)};
        graph->push_back(std::move(PAGE));
        ++progress;
    }

    indicators::show_console_cursor(false);
    if (graph->empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles loaded into graph\n\n";
        return EXIT_FAILURE;
    }

    std::cout << termcolor::reset << "\n\nLoading links into graph (Adjacent Nodes)...\n";
    auto START_LINK_TIME = std::chrono::system_clock::now();
    indicators::BlockProgressBar links_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    progress = 0;

    // Load in each file's links
    for (const auto &FILE : file_names) {
        std::ifstream file_in;
        file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file_in.open(FILE);
            file_in.peek();
            
            while (!file_in.eof())
                load_links(file_in, links_bar, progress);

            file_in.close();
        } catch (const std::ifstream::failure &E) {
            std::cerr << E.what() << "\n\n";
            return EXIT_FAILURE;
        }
    }

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
//Parallel
inline int wiki_loader::load_title(std::set<std::string> &titles, std::ifstream &file_in) {
    //std::string JSON_line;
    const std::string JSON_LINE;
    try {
        //std::getline(file_in, JSON_line);
        const std::string *LINE_PTR{&JSON_LINE};
        std::string *change_ptr{const_cast<std::string *>(LINE_PTR)};
        std::getline(file_in, *change_ptr);
        file_in.peek();
    } catch (const std::exception &E) {
        std::cerr << "\n\nError loading line from file\n\n";
        return EXIT_FAILURE;
    }

    //file_in.peek();
    //const json JSON = json::parse(JSON_line);  // { } Initialization breaks this ¯\_(ツ)_/¯
    const json JSON;
    try {
        const json *JSON_PTR{&JSON};
        json *change_ptr{const_cast<json *>(JSON_PTR)};
        *change_ptr = json::parse(JSON_LINE);
    } catch (const json::parse_error &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    //const std::string TITLE{std::move(JSON[0].get<std::string>())};
    titles.insert(std::move(JSON[0].get<std::string>()));
    //titles.insert(std::move(TITLE));
    return EXIT_SUCCESS;
}


// Load in a link to the graph
// I would make this parallel, but I tried and it gave an allocation error (malloc: *** error for object 0x11ef65120: pointer being freed was not allocated) for the vector on the line "page->adjacent.push_back(adjacent_page);" while doing it parallel so I know I somehow fucked it up pretty bad ¯\_(ツ)_/¯
inline int wiki_loader::load_links(std::ifstream &file_in, indicators::BlockProgressBar &bar, unsigned int &progress) {
    const double PERCENT{100 * ((double)progress / (graph->size() - 1))};

    // Progress bar stuff
    if (progress % 1000 == 0 || PERCENT >= 100) {  // Only update progress bar every 1000 titles to save time
        if (!bar.is_completed()) {
            if (PERCENT >= 100) {
                bar.set_option(indicators::option::ShowRemainingTime{false});
                bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bar.set_progress(PERCENT);
        }
    }
    
    const std::string JSON_LINE;
    try {
        const std::string *LINE_PTR{&JSON_LINE};
        std::string *change_ptr{const_cast<std::string *>(LINE_PTR)};
        std::getline(file_in, *change_ptr);
        //std::getline(file_in, JSON_line);
        file_in.peek();
    } catch (const std::ifstream::failure &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    const json JSON{};
    try {
        const json *JSON_PTR{&JSON};
        json *change_ptr{const_cast<json *>(JSON_PTR)}; // const_cast because I want it to be constant after parsing the line, but I can't initialize it in the try block
        *change_ptr = json::parse(JSON_LINE);
    } catch (const json::parse_error &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }
    //const std::string TITLE{JSON[0].get<std::string>()};

    ++progress;
    graph_vertex *page{graph->find(JSON[0].get<std::string>())};
    //graph_vertex *page{graph->find(std::move(JSON[0].get<std::string>()))};
    //graph_vertex *page{graph->find(std::move(TITLE))};
    if (page == nullptr)
        return EXIT_FAILURE;
        
    page->adjacent.reserve(JSON[1].size());
    for (const auto &LINK : JSON[1]) {
        //graph_vertex *adjacent_page{graph->find(LINK.get<std::string>())};
        graph_vertex *adjacent_page{graph->find(std::move(LINK.get<std::string>()))};
        if (adjacent_page == nullptr)
                continue;

        page->adjacent.push_back(adjacent_page);
        ++page->links;
        ++adjacent_page->linked_to;
        ++graph->num_edges;
    }
    page->adjacent.shrink_to_fit();  // Good for memory 👍
    return EXIT_SUCCESS;
}