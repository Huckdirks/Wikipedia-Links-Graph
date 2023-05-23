// Files to include
#include "include/wiki_loader.hpp"

// Load all articles into the graph
int wiki_loader::load() {
    const auto START_TIME{std::chrono::system_clock::now()};

    // Change directory to Articles-p
    const fs::path MAIN_DIR{fs::current_path()};
    try {
        fs::current_path("data/load/Articles-p");
    } catch (const fs::filesystem_error &E) {
        std::cerr << "\nError: " << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    // Get all file names ending in .ndjson in directory
    std::vector<std::string> file_names;
    for (const auto &FILE : fs::directory_iterator(MAIN_DIR / "data/load/Articles-p")) {
        if (FILE.path().extension() == ".ndjson" && fs::file_size(FILE) != 0)
                file_names.push_back(FILE.path().filename());
    }

    if (file_names.empty()) {
        std::cout << "\n\nNo files found in directory\n\n";
        return EXIT_FAILURE;
    }

    std::set<std::string> titles;   // Set of titles to make sure all titles are sorted before adding to graph
    BS::thread_pool pool;
    std::vector <std::future<std::set<std::string>>> title_futures;
    //const long unsigned int LOAD_FILES_SIZE{file_names.size() - 1};

    indicators::BlockProgressBar title_bar{indicators::option::BarWidth{76}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::PrefixText{"1/2 "}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    indicators::show_console_cursor(false); // Hide cursor

    std::cout << "\nLoading Wikipedia page titles from " << file_names.size() << " files...\n";

    /* // Rewrite with range based for loop
    for (const std::string &FILE: file_names) {
        title_futures.push_back(pool.submit([this, FILE, LOAD_FILES_SIZE, &title_bar]() -> std::set<std::string> {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::set<std::string> titles;

            percent = 100 * ((double)progress / (LOAD_FILES_SIZE));
            try {   // In a try block so I can make a lock_guard just for the title_bar
                std::lock_guard<std::mutex> lock(progress_bar_mutex);
                title_bar.set_progress(percent);
            } catch (const std::exception &E) {
                std::cerr << E.what() << "\n";
            }

            try {
                file_in.open(FILE);
                file_in.peek();

                while (!file_in.eof())
                    load_title(titles, file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << "\nError: " << E.what() << "\n\n";
                return titles;
            }
            ++progress;
            return titles;
        }));
    } */

    for (unsigned int i{}; i < file_names.size(); ++i){ // Using a for loop for setup instead of the thread pool's built in parallel loop because I need to pass i by value
        title_futures.push_back(pool.submit([this, file_names, i, &title_bar]() -> std::set<std::string> {
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::set<std::string> titles;
            const std::string FILE_NAME{file_names[i]};

            percent = 100 * ((double)progress / (file_names.size() - 1));
            try {   // In a try block so I can make a lock_guard just for the title_bar
                std::lock_guard<std::mutex> lock(progress_bar_mutex);
                title_bar.set_progress(percent);
            } catch (const std::exception &E) {
                std::cerr << E.what() << "\n";
            }

            try {
                file_in.open(FILE_NAME);
                file_in.peek();

                while (!file_in.eof())
                    load_title(titles, file_in);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n";
            }
            ++progress;
            return titles;
        }));
    }
    pool.wait_for_tasks();

    title_bar.set_progress(100);
    title_bar.set_option(indicators::option::ShowRemainingTime{false});
    title_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
    title_bar.mark_as_completed();

    progress = 0;
    percent = 0;

    indicators::BlockProgressBar title_merge_bar{indicators::option::BarWidth{76}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::PrefixText{"2/2 "}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

    for (auto &title_future : title_futures){
        percent = 100 * ((double)progress / (title_futures.size() - 1));
        if (title_merge_bar.is_completed() || percent >= 100) {
            title_merge_bar.set_option(indicators::option::ShowRemainingTime{false});
            title_merge_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
        }
        title_merge_bar.set_progress(percent);
        titles.merge(title_future.get());
        ++progress;
    }

    if (titles.empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles found in files\n\n";
        return EXIT_FAILURE;
    }

    std::cout << termcolor::reset << "\n\nLoading " << titles.size() << " titles into the graph...\n";
    indicators::BlockProgressBar graph_titles_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

    // Load in each title to the graph
    for (const auto &TITLE : titles) {
        percent = 100 * ((double)progress / (titles.size() - 1));
        if (progress % 10000 == 0 && percent < 100)  // Only update progress bar every 1000 titles to save time
            graph_titles_bar.set_progress(percent);
        const graph_vertex PAGE{std::move(TITLE)};
        graph->push_back(std::move(PAGE));
        ++progress;
    }
    graph_titles_bar.set_progress(100);
    graph_titles_bar.set_option(indicators::option::ShowRemainingTime{false});
    graph_titles_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
    graph_titles_bar.mark_as_completed();

    if (graph->empty()) {
        indicators::show_console_cursor(true);
        std::cout << "\n\nNo titles loaded into graph\n\n";
        return EXIT_FAILURE;
    }

    std::cout << termcolor::reset << "\n\nLoading links into graph (Adjacent Nodes)...\n";
    auto START_LINK_TIME = std::chrono::system_clock::now();
    indicators::BlockProgressBar links_bar{indicators::option::BarWidth{80}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    progress = 0;

    // Load in each link to the graph
    for (unsigned int i{}; i < file_names.size(); ++i){
        pool.push_task([this, file_names, i, &links_bar](){
            std::ifstream file_in;
            file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                file_in.open(file_names[i]);
                file_in.peek();
                
                while (!file_in.eof())
                    load_links(file_in, links_bar);

                file_in.close();
            } catch (const std::ifstream::failure &E) {
                std::cerr << E.what() << "\n\n";
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        });
    }
    pool.wait_for_tasks();

    links_bar.set_progress(100);
    links_bar.set_option(indicators::option::ShowRemainingTime{false});
    links_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
    links_bar.mark_as_completed();

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
// Parallel
inline int wiki_loader::load_title(std::set<std::string> &titles, std::ifstream &file_in) {
    const std::string JSON_LINE;
    try {
        const std::string *LINE_PTR{&JSON_LINE};
        std::string *change_ptr{const_cast<std::string *>(LINE_PTR)};
        std::getline(file_in, *change_ptr);
        file_in.peek();
    } catch (const std::exception &E) {
        std::cerr << "\n\nError loading line from file\n\n";
        return EXIT_FAILURE;
    }

    const json JSON;
    try {
        const json *JSON_PTR{&JSON};
        json *change_ptr{const_cast<json *>(JSON_PTR)};
        *change_ptr = json::parse(JSON_LINE);   // { } Initialization breaks this ¯\_(ツ)_/¯
    } catch (const json::parse_error &E) {
        std::cerr << E.what() << "\n\n";
        return EXIT_FAILURE;
    }

    titles.insert(JSON[0].get<std::string>());
    return EXIT_SUCCESS;
}


// Load in a link to the graph
// The JSON parsing is parallel, but adding adjacent nodes to the graph is forced to be sequential because it keeps giving vector allocation/deallocation errors at the "page->adjacent.push_back(adjacent_page);" line if I don't have the mutex above ¯\_(ツ)_/¯
inline int wiki_loader::load_links(std::ifstream &file_in, indicators::BlockProgressBar &bar) {
    percent = 100 * ((double)progress / (graph->size() - 1));

    // Progress bar stuff
    if (progress % 1000 == 0 && percent < 100){
        std::lock_guard<std::mutex> lock(progress_bar_mutex);
        bar.set_progress(percent);
    }
    
    const std::string JSON_LINE;
    try {
        const std::string *LINE_PTR{&JSON_LINE};
        std::string *change_ptr{const_cast<std::string *>(LINE_PTR)};
        std::getline(file_in, *change_ptr);
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

    ++progress;

    graph_vertex *page{graph->find(JSON[0].get<std::string>())};
    if (page == nullptr)
        return EXIT_FAILURE;
        
    page->adjacent.reserve(JSON[1].size());
    std::lock_guard<std::mutex> lock(graph_mutex);  // Lock Guard has to be here because of the push_back line below. It sadly didn't work inside the for loop
    for (const auto &LINK : JSON[1]) {
        graph_vertex *adjacent_page{graph->find(std::move(LINK.get<std::string>()))};
        if (adjacent_page == nullptr)
                continue;

        page->adjacent.push_back(adjacent_page);
        ++page->links;
        ++adjacent_page->linked_to;
        ++graph->num_edges;
    }
    return EXIT_SUCCESS;
}