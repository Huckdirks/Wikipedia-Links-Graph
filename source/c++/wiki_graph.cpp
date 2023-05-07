// Files to include
#include "include/wiki_graph.hpp"
#include "include/wiki_loader.hpp"

// Graph Vertex Functions:
// Operator overloads

// = operator overload
void graph_vertex::operator=(const graph_vertex &VERTEX) {
    title = VERTEX.title;
    adjacent = VERTEX.adjacent;
}


// Display the article title, the number of times it is linked to, and the titles of the articles it links to if display_links is true
int graph_vertex::display(const bool DISPLAY_LINKS) const {
    if (title.empty())
        return EXIT_FAILURE;

    std::cout << "\nTitle: " << title << "\nTimes linked: " << linked_to << '\n';
    if (DISPLAY_LINKS) {
        std::cout << "Amount of links: : " << links << "\nLinks:\n";
        for (const auto &LINK : adjacent)
            std::cout << '\t' << LINK->title << '\n';
    }
    return EXIT_SUCCESS;
}



// Wiki Graph Functions:
// Operator overloads

// These operator overloads aren't really used here anymore but it's nice if you want to access a vertex by index from outside the class

// [ ] operator overload for ints
graph_vertex &wiki_graph::operator[](const unsigned int INDEX) {
    /* if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    if (INDEX >= vertex_list.size())
        throw std::out_of_range("Index out of bounds");
    return vertex_list[INDEX]; */
    try {
        return vertex_list.at(INDEX);
    } catch (const std::out_of_range &EXCEPTION) {
        std::cerr << EXCEPTION.what() << '\n';
        exit(1);
    }
}

// [ ] operator overload for strings
graph_vertex &wiki_graph::operator[](const std::string &PAGE) {
    /* if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");

    int i{binary_search_index(PAGE)};
    if (i == -1)
        throw std::out_of_range("Page not found");
    return vertex_list[i]; */
    try {
        return vertex_list.at(binary_search_index(PAGE));
    } catch (const std::out_of_range &EXCEPTION) {
        std::cerr << EXCEPTION.what() << '\n';
        exit(1);
    }
}


// Public Functions
// Wrapper Functions:

// Binary search for value (wrapper function)
graph_vertex *wiki_graph::find(const std::string &TO_FIND) {
    if (vertex_list.empty())
        return nullptr;

    return binary_search(TO_FIND);
}

// Binary search for index (wrapper function)
int wiki_graph::find_index(const std::string &TO_FIND) {
    if (vertex_list.empty())
        return -1;

    return binary_search_index(TO_FIND);
}

// Get top n most linked to vertices & display (wrapper function)
std::vector<graph_vertex *> wiki_graph::top_n(const unsigned int N) {
    if (N <= 0 || vertex_list.empty())
        return {};

    if (N == 1) {
        std::vector<graph_vertex *> top_n_list;
        graph_vertex *best_vertex{nullptr};
        for (auto &vertex: vertex_list) {
            if (vertex.linked_to > best_vertex->linked_to)
                best_vertex = &vertex;
        }
        top_n_list.push_back(best_vertex);
        top_n_list.shrink_to_fit();
        return top_n_list;
    }

    auto top_n_list = top_n_linked(N);
    if (top_n_list.empty())  // You never know
        return {};

    top_n_list.shrink_to_fit();
    return top_n_list;
}


// Get all vertices linked to a vertex (wrapper function)
std::vector<graph_vertex *> wiki_graph::linked_to(const std::string &TO_FIND) {
    if (vertex_list.empty())
        return {};

    // Only to check if the vertex actually exists
    graph_vertex *vertex{binary_search(TO_FIND)};
    if (vertex == nullptr)
        return {};

    auto linked_to = all_linked_to(TO_FIND);
    linked_to.shrink_to_fit();
    return linked_to;
}


// Main Functions

// Load articles from file
int wiki_graph::load() {
    wiki_loader loader(*this);
    return loader.load();
}


// Get first vertex
graph_vertex &wiki_graph::front() {
    /* if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    return vertex_list.front(); */
    try {
        return vertex_list.front();
    }
    catch (const std::out_of_range &E) {
        std::cerr << E.what() << '\n';
        exit(EXIT_FAILURE);
    }
}


// Get last vertex
graph_vertex &wiki_graph::back() {
    /* if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    return vertex_list.back(); */
    try {
        return vertex_list.back();
    }
    catch (const std::out_of_range &E) {
        std::cerr << E.what() << '\n';
        exit(EXIT_FAILURE);
    }
}


// Check if empty
bool wiki_graph::empty() const {
    return vertex_list.empty();
}


// Get size
unsigned int wiki_graph::size() const {
    return vertex_list.size();
}


// Add vertex to the end of the list
void wiki_graph::push_back(const graph_vertex vertex) {
    return vertex_list.push_back(vertex);
}


// Private Functions

// Find a vertex in the graph by title
// Implementation adapted from https://www.geeksforgeeks.org/binary-search/
graph_vertex *wiki_graph::binary_search(const std::string &TO_FIND) {
    unsigned int lo{}, mid{};
    long unsigned int hi{vertex_list.size() - 1};   // Needs to be long unsigned int for { } initialization

    // This below check covers all cases , so need to check for mid=lo-(hi-lo)/2
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (vertex_list[mid].title < TO_FIND)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (vertex_list[lo].title == TO_FIND)
        return &vertex_list[lo];
    else if (vertex_list[hi].title == TO_FIND)
        return &vertex_list[hi];

    // If not found
    return nullptr;
}


// Find the index of a vertex in the graph by title
// Implementation adapted from https://www.geeksforgeeks.org/binary-search/
int wiki_graph::binary_search_index(const std::string &TO_FIND) {
    unsigned int lo{}, mid{};
    long unsigned int hi{vertex_list.size() - 1};   // Needs to be long unsigned int for { } initialization

    // This below check covers all cases , so need to check for mid=lo-(hi-lo)/2
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (vertex_list[mid].title < TO_FIND)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (vertex_list[lo].title == TO_FIND)
        return lo;
    else if (vertex_list[hi].title == TO_FIND)
        return hi;

    // If not found return -1 to show index isn't found
    return -1;
}


// Find top (n) most linked to pages
// Parallel
std::vector<graph_vertex *> wiki_graph::top_n_linked(const unsigned int N) {
    std::vector<graph_vertex *> top_n;
    unsigned int best{};

    // Fill top_n with blank vertex pointers so there's something to compare to
    graph_vertex blank;
    for (unsigned int i{}; i < N; ++i)
        top_n.push_back(&blank);

    // Set up for parallelization
    unsigned int cores{std::thread::hardware_concurrency()};
    long unsigned int segment_size{vertex_list.size() / cores};

    // // If the segment size is < n, then remove a core to make the segment size bigger
    // There's probably a better way to do this but I'm too lazy to figure it out rn
    while (N > segment_size) {
        --cores;
        segment_size = vertex_list.size() / cores;
    }

    // Make an array the size of the # of cores the computer has of futures to hold the results of each thread
    std::unique_ptr<std::future<std::vector<graph_vertex *>>[]> futures{std::make_unique<std::future<std::vector<graph_vertex *>>[]>(cores)};
    std::unique_ptr<indicators::BlockProgressBar[]> block_bars{std::make_unique<indicators::BlockProgressBar[]>(cores)};

    // Set up progress bars
    for (unsigned int i{}; i < cores; ++i)
        block_bars[i].set_option(indicators::option::BarWidth{50}), block_bars[i].set_option(indicators::option::PrefixText{"Finding top " + std::to_string(N) + " in section " + std::to_string(i + 1) + '/' + std::to_string(cores) + ' '}), block_bars[i].set_option(indicators::option::Start{"["}), block_bars[i].set_option(indicators::option::End{"]"}), block_bars[i].set_option(indicators::option::ShowElapsedTime{true}), block_bars[i].set_option(indicators::option::ShowRemainingTime{true}), block_bars[i].set_option(indicators::option::ForegroundColor{indicators::Color::red}), block_bars[i].set_option(indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}});

    // Set up dynamic progress bar for each segment
    indicators::DynamicProgress<indicators::BlockProgressBar> bars;
    indicators::show_console_cursor(false);

    // Split the graph into (cores) segments and run top_n_linked_segment on each segment in parallel
    for (unsigned int i{}; i < cores; ++i) {
        bars.push_back(block_bars[i]);
        try {
            futures[i] = std::async(std::launch::async, &wiki_graph::top_n_linked_segment, this, N, i * segment_size, (i + 1) * segment_size, std::ref(bars));
        } catch (const std::future_error &E) {
            std::cerr << E.what() << '\n';
            exit(EXIT_FAILURE);
        }
        //futures[i] = std::async(std::launch::async, &wiki_graph::top_n_linked_segment, this, N, i * segment_size, (i + 1) * segment_size, std::ref(bars));
    }

    // Wait for all the functions to finish and get the results
    for (unsigned int i{}; i < cores; ++i)
        futures[i].wait();

    std::cout << '\n';

    indicators::BlockProgressBar total_bar{indicators::option::BarWidth{50}, indicators::option::PrefixText{"Searching through section 1/" + std::to_string(cores)}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    unsigned int progress{};
    unsigned int section{1};

    // Go through the results of top_n_linked_segment's results and find the top n
    for (unsigned int i{}; i < cores; ++i) {
        const std::vector<graph_vertex *> SEGMENT{};    // For exception handling for the future I need to set it's value in the try block, but since it's const after being set I use const_cast to set it
        const auto *SEGMENT_PTR{&SEGMENT};
        auto change_ptr{const_cast<std::vector<graph_vertex *> *>(SEGMENT_PTR)};
        try {
            *change_ptr = futures[i].get();   // Might need to make the result of futures[i].get() a unique_ptr
            //*change_ptr = std::make_unique<std::vector<graph_vertex *>>(futures[i].get());
            //auto temp{std::make_unique<std::vector<graph_vertex *>>(futures[i].get())};
            //change_ptr = temp.get();
        } catch (const std::future_error &E) {
            std::cerr << E.what() << '\n';
            exit(EXIT_FAILURE);
        }
        //change_ptr = &futures[i].get();

        //futures[i].get();
        double percent{100 * ((double)progress / (N * cores))};
        for (const auto &VERTEX : SEGMENT) {
            // Progress bar
            percent = 100 * ((double)progress / (N * cores));
            if (fmod(percent, ((float)100 / cores)) == 0) {
                total_bar.set_option(indicators::option::PrefixText{"Searching through section " + std::to_string(section) + '/' + std::to_string(cores) + ' '});
                ++section;
            }
            if (total_bar.is_completed() || percent >= 100) {
                total_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                total_bar.set_option(indicators::option::PrefixText{"Found top " + std::to_string(N) + " pages "});
                total_bar.set_option(indicators::option::ShowRemainingTime{false});
            }
            if (progress % 100 == 0)  // Only update the progress bar every 100 iterations
                total_bar.set_progress(percent);
            ++progress;

            // Actual work
            // If the current vertex has less links than the current bottom
            if (VERTEX->linked_to < top_n.back()->linked_to)
                continue;

            // If the current vertex has more linked_to than the current best
            if (VERTEX->linked_to >= best) {
                best = VERTEX->linked_to;
                top_n.pop_back();
                top_n.insert(top_n.begin(), VERTEX);
                continue;
            }

            // If the current vertex has less links than the current best go through the top_n vector and find where it should be inserted
            for (int j = N - 1; j > 0; --j) {
                if (VERTEX->title == top_n[j]->title)   // If duplicate
                    break;
                else if (VERTEX->linked_to < top_n[j]->linked_to) {  // If the current vertex has less links than the current top_n[j], make the previous top_n[j] the current vertex
                    top_n.pop_back();
                    top_n.insert(top_n.begin() + j + 1, VERTEX);
                    break;
                } else if (j == 1) {
                    top_n.pop_back();
                    top_n.insert(top_n.begin() + j, VERTEX);
                }
            }
        }
    }
    indicators::show_console_cursor(true);
    std::cout << termcolor::reset;
    return top_n;
}


// Find top (n) most linked to pages in a segment of the graph
std::vector<graph_vertex *> wiki_graph::top_n_linked_segment(const unsigned int N, const unsigned int SEGMENT_START, const unsigned int SEGMENT_END, indicators::DynamicProgress<indicators::BlockProgressBar> &bars) {
    std::vector<graph_vertex *> top_n;
    const unsigned int SEGMENT_SIZE{SEGMENT_END - SEGMENT_START};
    const unsigned int SEGMENT_NUM{(SEGMENT_END / SEGMENT_SIZE) - 1};
    unsigned int best{};
    double percent{};

    // Fill top_n with first n vertex pointers so there's something to compare to
    for (unsigned int i{SEGMENT_START}; i < SEGMENT_START + N; ++i)
        top_n.push_back(&vertex_list[i]);

    for (unsigned int i = SEGMENT_START; i < SEGMENT_END; ++i) {
        // Progress bar
        percent = 100 * ((double)(i - SEGMENT_START) / (SEGMENT_SIZE - 1));
        if ((i - SEGMENT_START) % (SEGMENT_SIZE / 1000) == 0 || percent >= 100) {  // Only update the progress bar every 0.1% of the segment
            if (bars[SEGMENT_NUM].is_completed() || percent >= 100) {
                bars[SEGMENT_NUM].set_option(indicators::option::ShowRemainingTime{false});
                bars[SEGMENT_NUM].set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bars[SEGMENT_NUM].set_progress(percent);
        }

        // Actual work
        // If the current vertex has less links than the current bottom
        if (vertex_list[i].linked_to < top_n.back()->linked_to)
            continue;

        // If the current vertex has more links than the current best
        if (vertex_list[i].linked_to > best) {
            best = vertex_list[i].linked_to;
            top_n.pop_back();
            top_n.insert(top_n.begin(), &vertex_list[i]);
            continue;
        }

        // If the current vertex has less links than the current best go through the top_n vector and find where it should be inserted
        for (int j = N - 1; j > 0; --j) {
            if (vertex_list[i].title == top_n[j]->title)
                break;
            else if (vertex_list[i].linked_to < top_n[j]->linked_to) {  // If the current vertex has less links than the current top_n[j], make the previous top_n[j] the current vertex
                top_n.pop_back();
                top_n.insert(top_n.begin() + j + 1, &vertex_list[i]);
                break;
            } else if (j == 1) {
                top_n.pop_back();
                top_n.insert(top_n.begin() + j, &vertex_list[i]);
            }
        }
    }
    bars[SEGMENT_NUM].set_option(indicators::option::PrefixText{"Found top " + std::to_string(N) + " in section " + std::to_string(SEGMENT_NUM + 1) + '/' + std::to_string((int)(vertex_list.size() / SEGMENT_SIZE)) + ' '});
    top_n.shrink_to_fit();
    return top_n;
}


// Find all the pages linking to a given page
// Parallel
std::vector<graph_vertex *> wiki_graph::all_linked_to(const std::string &LINKED_TO) {
    std::vector<graph_vertex *> linked_to_list;
    // Set up for parallelization
    const unsigned int CORES{std::thread::hardware_concurrency()};
    const long unsigned int SEGMENT_SIZE{vertex_list.size() / CORES};

    // Make an array the size of the # of cores the computer has of futures to hold the results of each thread
    std::unique_ptr<std::future<std::vector<graph_vertex *>>[]> futures{std::make_unique<std::future<std::vector<graph_vertex *>>[]>(CORES)};
    std::unique_ptr<indicators::BlockProgressBar[]> block_bars{std::make_unique<indicators::BlockProgressBar[]>(CORES)};

    // Set up progress bars
    for (unsigned int i{}; i < CORES; ++i)
        block_bars[i].set_option(indicators::option::BarWidth{50}), block_bars[i].set_option(indicators::option::PrefixText{"Finding linked to pages in section " + std::to_string(i + 1) + '/' + std::to_string(CORES) + ' '}), block_bars[i].set_option(indicators::option::Start{"["}), block_bars[i].set_option(indicators::option::End{"]"}), block_bars[i].set_option(indicators::option::ShowElapsedTime{true}), block_bars[i].set_option(indicators::option::ShowRemainingTime{true}), block_bars[i].set_option(indicators::option::ForegroundColor{indicators::Color::red}), block_bars[i].set_option(indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}});

    // Set up dynamic progress bar for each segment
    indicators::DynamicProgress<indicators::BlockProgressBar> bars;
    indicators::show_console_cursor(false);

    // Split the graph into (CORES) segments and run top_n_linked_segment on each segment in parallel
    for (unsigned int i{}; i < CORES; ++i) {
        bars.push_back(block_bars[i]);
        try {
            futures[i] = std::async(std::launch::async, &wiki_graph::all_linked_to_segment, this, LINKED_TO, i * SEGMENT_SIZE, (i + 1) * SEGMENT_SIZE, std::ref(bars));
        } catch (const std::future_error &E) {
            std::cerr << E.what() << '\n';
        }
        //futures[i] = std::async(std::launch::async, &wiki_graph::all_linked_to_segment, this, LINKED_TO, i * SEGMENT_SIZE, (i + 1) * SEGMENT_SIZE, std::ref(bars));
    }

    // Wait for all the functions to finish and get the results
    for (unsigned int i{}; i < CORES; ++i)
        futures[i].wait();

    std::cout << '\n';

    // Combine the results of each thread into one vector
    for (unsigned int i{}; i < CORES; ++i) {
        std::vector<graph_vertex *> temp{};
        try {
            temp = futures[i].get();
        } catch (const std::future_error &E) {
            std::cerr << E.what() << '\n';
        }
        std::move(temp.begin(), temp.end(), std::back_inserter(linked_to_list));    // moving is faster than copying I guess
    }

    // Sort the vector by title to make it fancy
    // Also IDK why VSCode is mad about std::execution::par, it works fine. Just get rid of that parameter if you really don't like the error
    // HA I figured out why. Just configure your intellisense to use g++(17+) instead of clang (also clang won't allow you to compile with std::execution::par)
    std::sort(std::execution::par, linked_to_list.begin(), linked_to_list.end(), [](graph_vertex *a, graph_vertex *b) { return a->title < b->title; });

    indicators::show_console_cursor(true);
    linked_to_list.shrink_to_fit();
    return linked_to_list;
}


// Find all the pages linking to a given page in a segment of the graph
std::vector<graph_vertex *> wiki_graph::all_linked_to_segment(const std::string &LINKED_TO, const unsigned int SEGMENT_START, const unsigned int SEGMENT_END, indicators::DynamicProgress<indicators::BlockProgressBar> &bars) {
    std::vector<graph_vertex *> linked_to_list;
    const unsigned int SEGMENT_SIZE{SEGMENT_END - SEGMENT_START};
    const unsigned int SEGMENT_NUM{(SEGMENT_END / SEGMENT_SIZE) - 1};
    double percent{};

    for (unsigned int i = SEGMENT_START; i < SEGMENT_END; ++i) {
        // Progress bar
        percent = 100 * ((double)(i - SEGMENT_START) / (SEGMENT_SIZE - 1));
        if ((i - SEGMENT_START) % (SEGMENT_SIZE / 1000) == 0 || percent >= 100) {  // Only update the progress bar every 0.1% of the segment
            if (bars[SEGMENT_NUM].is_completed() || percent >= 100) {
                bars[SEGMENT_NUM].set_option(indicators::option::ShowRemainingTime{false});
                bars[SEGMENT_NUM].set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bars[SEGMENT_NUM].set_progress(percent);
        }

        // Actual work
        for (const auto &LINK : vertex_list[i].adjacent) {
            if (LINK->title == LINKED_TO) {
                linked_to_list.push_back(&vertex_list[i]);
                break;
            }
        }
    }
    bars[SEGMENT_NUM].set_option(indicators::option::PrefixText{"Found all linked to in section " + std::to_string(SEGMENT_NUM + 1) + '/' + std::to_string((int)(vertex_list.size() / SEGMENT_SIZE)) + ' '});
    linked_to_list.shrink_to_fit();
    return linked_to_list;
}