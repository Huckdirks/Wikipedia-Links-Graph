// Files to include
#include "wiki_graph.hpp"

#include "wiki_loader.hpp"

// Graph Vertex Functions:
// Operator overloads

// = operator overload
void graph_vertex::operator=(const graph_vertex &vertex) {
    title = vertex.title;
    adjacent = vertex.adjacent;
}


// Display the article title, the number of times it is linked to, and the titles of the articles it links to if display_links is true
int graph_vertex::display(const bool display_links) const {
    if (title.empty())
        return 0;

    std::cout << "\nTitle: " << title << "\nTimes linked: " << links_to << '\n';
    if (display_links) {
        std::cout << "Links:\n";
        for (const auto &i : adjacent)
            std::cout << '\t' << i->title << '\n';
    }
    return 1;
}



// Wiki Graph Functions:
// Operator overloads

// These operator overloads aren't really used here anymore but it's nice if you want to access a vertex by index from outside the class

// [ ] operator overload for ints
graph_vertex &wiki_graph::operator[](const unsigned int index) {
    if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    if (index >= vertex_list.size())
        throw std::out_of_range("Index out of bounds");
    return vertex_list[index];
}

// [ ] operator overload for strings
graph_vertex &wiki_graph::operator[](const std::string val) {
    if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");

    int i{binary_search_index(val)};
    if (i == -1)
        throw std::out_of_range("Page not found");
    return vertex_list[i];
}


// Public Functions
// Wrapper Functions:

// Binary search for value (wrapper function)
graph_vertex *wiki_graph::find(const std::string to_find) {
    if (vertex_list.empty())
        return nullptr;

    return binary_search(to_find);
}

// Binary search for index (wrapper function)
int wiki_graph::find_index(const std::string to_find) {
    if (vertex_list.empty())
        return -1;

    return binary_search_index(to_find);
}

// Get top n most linked to vertices & display (wrapper function)
std::vector<graph_vertex *> wiki_graph::top_n(const int n) {
    if (n <= 0 || vertex_list.empty())
        return {};

    if (n == 1) {
        std::vector<graph_vertex *> top_n_list;
        int best{};
        graph_vertex *best_vertex{nullptr};
        for (auto &i : vertex_list) {
            if (i.links_to > best) {
                best = i.links_to;
                best_vertex = &i;
            }
        }
        top_n_list.push_back(best_vertex);
        top_n_list.shrink_to_fit();
        return top_n_list;
    }

    auto top_n_list = top_n_linked(n);
    if (top_n_list.empty())  // You never know
        return {};

    top_n_list.shrink_to_fit();
    return top_n_list;
}


// Get all vertices linked to a vertex (wrapper function)
std::vector<graph_vertex *> wiki_graph::linked_to(const std::string to_find) {
    if (vertex_list.empty())
        return {};

    // Only to check if the vertex actually exists
    graph_vertex *vertex{binary_search(to_find)};
    if (vertex == nullptr)
        return {};

    auto linked_to = all_linked_to(to_find);
    if (linked_to.empty())  // You never know
        return {};

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
    if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    return vertex_list.front();
}


// Get last vertex
graph_vertex &wiki_graph::back() {
    if (vertex_list.empty())
        throw std::out_of_range("Graph is empty");
    return vertex_list.back();
}


// Check if empty
bool wiki_graph::empty() const {
    return vertex_list.empty();
}


// Get size
int wiki_graph::size() const {
    return vertex_list.size();
}


// Add vertex to the end of the list
void wiki_graph::push_back(const graph_vertex vertex) {
    return vertex_list.push_back(vertex);
}


// Private Functions

// Find a vertex in the graph by title
// Implementation adapted from https://www.geeksforgeeks.org/binary-search/
graph_vertex *wiki_graph::binary_search(const std::string to_find) {
    int lo{}, hi = vertex_list.size() - 1, mid{};  // { } initialization doesnt work with hi so imma keep it like this

    // This below check covers all cases , so need to check for mid=lo-(hi-lo)/2
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (vertex_list[mid].title < to_find)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (vertex_list[lo].title == to_find)
        return &vertex_list[lo];
    else if (vertex_list[hi].title == to_find)
        return &vertex_list[hi];

    // If not found
    return nullptr;
}


// Find the index of a vertex in the graph by title
// Implementation adapted from https://www.geeksforgeeks.org/binary-search/
int wiki_graph::binary_search_index(const std::string to_find) {
    int lo{}, hi = vertex_list.size() - 1, mid{};

    // This below check covers all cases , so need to check for mid=lo-(hi-lo)/2
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (vertex_list[mid].title < to_find)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (vertex_list[lo].title == to_find)
        return lo;
    else if (vertex_list[hi].title == to_find)
        return hi;

    // If not found return -1 to show index isn't found
    return -1;
}


// Find top (n) most linked to pages
// Parallel
std::vector<graph_vertex *> wiki_graph::top_n_linked(const int n) {
    std::vector<graph_vertex *> top_n;
    int best{-1};  // Set to -1 so the first vertex will always be the best

    // Fill top_n with blank vertex pointers so there's something to compare to
    graph_vertex blank;
    for (int i{}; i < n; ++i)
        top_n.push_back(&blank);

    // Set up for parallelization
    int cores = std::thread::hardware_concurrency();
    int segment_size = vertex_list.size() / cores;

    // // If the segment size is < n, then remove a core to make the segment size bigger
    // There's probably a better way to do this but I'm too lazy to figure it out rn
    while (n > segment_size) {
        --cores;
        segment_size = vertex_list.size() / cores;
    }

    // Make an array the size of the # of cores the computer has of futures to hold the results of each thread
    std::unique_ptr<std::future<std::vector<graph_vertex *>>[]> futures{std::make_unique<std::future<std::vector<graph_vertex *>>[]>(cores)};
    std::unique_ptr<indicators::BlockProgressBar[]> block_bars{std::make_unique<indicators::BlockProgressBar[]>(cores)};

    // Set up progress bars
    for (int i{}; i < cores; ++i)
        block_bars[i].set_option(indicators::option::BarWidth{50}), block_bars[i].set_option(indicators::option::PrefixText{"Finding top " + std::to_string(n) + " in section " + std::to_string(i + 1) + '/' + std::to_string(cores) + ' '}), block_bars[i].set_option(indicators::option::Start{"["}), block_bars[i].set_option(indicators::option::End{"]"}), block_bars[i].set_option(indicators::option::ShowElapsedTime{true}), block_bars[i].set_option(indicators::option::ShowRemainingTime{true}), block_bars[i].set_option(indicators::option::ForegroundColor{indicators::Color::red}), block_bars[i].set_option(indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}});

    // Set up dynamic progress bar for each segment
    indicators::DynamicProgress<indicators::BlockProgressBar> bars;
    indicators::show_console_cursor(false);

    // Split the graph into (cores) segments and run top_n_linked_segment on each segment in parallel
    for (int i{}; i < cores; ++i) {
        bars.push_back(block_bars[i]);
        futures[i] = std::async(std::launch::async, &wiki_graph::top_n_linked_segment, this, n, i * segment_size, (i + 1) * segment_size, std::ref(bars));
    }

    // Wait for all the functions to finish and get the results
    for (int i{}; i < cores; ++i)
        futures[i].wait();

    std::cout << '\n';

    indicators::BlockProgressBar total_bar{indicators::option::BarWidth{50}, indicators::option::PrefixText{"Searching through section 1/" + std::to_string(cores)}, indicators::option::Start{"["}, indicators::option::End{"]"}, indicators::option::ShowElapsedTime{true}, indicators::option::ShowRemainingTime{true}, indicators::option::ForegroundColor{indicators::Color::red}, indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    int progress{};
    int section{1};

    // Go through the results of top_n_linked_segment's results and find the top n
    for (int i{}; i < cores; ++i) {
        const auto segment{futures[i].get()};
        for (const auto &vertex : segment) {
            // Progress bar
            const double percent{100 * ((double)progress / (n * cores))};
            if (fmod(percent, ((float)100 / cores)) == 0) {
                total_bar.set_option(indicators::option::PrefixText{"Searching through section " + std::to_string(section) + '/' + std::to_string(cores) + ' '});
                ++section;
            }
            if (total_bar.is_completed() || percent >= 100) {
                total_bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                total_bar.set_option(indicators::option::PrefixText{"Found top " + std::to_string(n) + " pages "});
                total_bar.set_option(indicators::option::ShowRemainingTime{false});
            }
            if (progress % 100 == 0)  // Only update the progress bar every 100 iterations
                total_bar.set_progress(percent);
            ++progress;

            // Actual work
            // If the current vertex has less links than the current bottom
            if (vertex->links_to < top_n.back()->links_to)
                continue;

            // If the current vertex has more links than the current best
            if (vertex->links_to > best) {
                best = vertex->links_to;
                top_n.pop_back();
                top_n.insert(top_n.begin(), vertex);
                continue;
            }

            // If the current vertex has less links than the current best go through the top_n vector and find where it should be inserted
            for (int j = n - 1; j > 0; --j) {
                if (vertex->title == top_n[j]->title)
                    break;
                else if (vertex->links_to < top_n[j]->links_to) {  // If the current vertex has less links than the current top_n[j], make the previous top_n[j] the current vertex
                    top_n.pop_back();
                    top_n.insert(top_n.begin() + j + 1, vertex);
                    break;
                } else if (j == 1) {
                    top_n.pop_back();
                    top_n.insert(top_n.begin() + j, vertex);
                }
            }
        }
    }
    indicators::show_console_cursor(true);
    std::cout << termcolor::reset;
    return top_n;
}


// Find top (n) most linked to pages in a segment of the graph
std::vector<graph_vertex *> wiki_graph::top_n_linked_segment(const int n, const int segment_start, const int segment_end, indicators::DynamicProgress<indicators::BlockProgressBar> &bars) {
    std::vector<graph_vertex *> top_n;
    int best{-1};  // Set to -1 so the first vertex will always be the best
    const int segment_size{segment_end - segment_start};
    const int segment_num{(segment_end / segment_size) - 1};

    // Fill top_n with first n vertex pointers so there's something to compare to
    for (int i{segment_start}; i < segment_start + n; ++i)
        top_n.push_back(&vertex_list[i]);

    for (int i = segment_start; i < segment_end; ++i) {
        // Progress bar
        const double percent{100 * ((double)(i - segment_start) / (segment_size - 1))};
        if ((i - segment_start) % (segment_size / 1000) == 0 || percent >= 100) {  // Only update the progress bar every 0.1% of the segment
            if (bars[segment_num].is_completed() || percent >= 100) {
                bars[segment_num].set_option(indicators::option::ShowRemainingTime{false});
                bars[segment_num].set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bars[segment_num].set_progress(percent);
        }

        // Actual work
        // If the current vertex has less links than the current bottom
        if (vertex_list[i].links_to < top_n.back()->links_to)
            continue;

        // If the current vertex has more links than the current best
        if (vertex_list[i].links_to > best) {
            best = vertex_list[i].links_to;
            top_n.pop_back();
            top_n.insert(top_n.begin(), &vertex_list[i]);
            continue;
        }

        // If the current vertex has less links than the current best go through the top_n vector and find where it should be inserted
        for (int j = n - 1; j > 0; --j) {
            if (vertex_list[i].title == top_n[j]->title)
                break;
            else if (vertex_list[i].links_to < top_n[j]->links_to) {  // If the current vertex has less links than the current top_n[j], make the previous top_n[j] the current vertex
                top_n.pop_back();
                top_n.insert(top_n.begin() + j + 1, &vertex_list[i]);
                break;
            } else if (j == 1) {
                top_n.pop_back();
                top_n.insert(top_n.begin() + j, &vertex_list[i]);
            }
        }
    }
    bars[segment_num].set_option(indicators::option::PrefixText{"Found top " + std::to_string(n) + " in section " + std::to_string(segment_num + 1) + '/' + std::to_string((int)(vertex_list.size() / segment_size)) + ' '});
    top_n.shrink_to_fit();
    return top_n;
}


// Find all the pages linking to a given page
// Parallel
std::vector<graph_vertex *> wiki_graph::all_linked_to(const std::string linked_to) {
    std::vector<graph_vertex *> linked_to_list;
    // Set up for parallelization
    const int cores = std::thread::hardware_concurrency();
    const int segment_size = vertex_list.size() / cores;

    // Make an array the size of the # of cores the computer has of futures to hold the results of each thread
    std::unique_ptr<std::future<std::vector<graph_vertex *>>[]> futures{std::make_unique<std::future<std::vector<graph_vertex *>>[]>(cores)};
    std::unique_ptr<indicators::BlockProgressBar[]> block_bars{std::make_unique<indicators::BlockProgressBar[]>(cores)};

    // Set up progress bars
    for (int i{}; i < cores; ++i)
        block_bars[i].set_option(indicators::option::BarWidth{50}), block_bars[i].set_option(indicators::option::PrefixText{"Finding linked to pages in section " + std::to_string(i + 1) + '/' + std::to_string(cores) + ' '}), block_bars[i].set_option(indicators::option::Start{"["}), block_bars[i].set_option(indicators::option::End{"]"}), block_bars[i].set_option(indicators::option::ShowElapsedTime{true}), block_bars[i].set_option(indicators::option::ShowRemainingTime{true}), block_bars[i].set_option(indicators::option::ForegroundColor{indicators::Color::red}), block_bars[i].set_option(indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}});

    // Set up dynamic progress bar for each segment
    indicators::DynamicProgress<indicators::BlockProgressBar> bars;
    indicators::show_console_cursor(false);

    // Split the graph into (cores) segments and run top_n_linked_segment on each segment in parallel
    for (int i{}; i < cores; ++i) {
        bars.push_back(block_bars[i]);
        futures[i] = std::async(std::launch::async, &wiki_graph::all_linked_to_segment, this, linked_to, i * segment_size, (i + 1) * segment_size, std::ref(bars));
    }

    // Wait for all the functions to finish and get the results
    for (int i{}; i < cores; ++i)
        futures[i].wait();

    std::cout << '\n';

    // Combine the results of each thread into one vector
    for (int i{}; i < cores; ++i) {
        std::vector<graph_vertex *> temp = futures[i].get();
        std::move(temp.begin(), temp.end(), std::back_inserter(linked_to_list));
    }

    // Sort the vector by title to make it fancy
    // Also IDK why VSCode is mad about std::execution::par, it works fine. Just get rid of that parameter if you really don't like the error
    std::sort(std::execution::par, linked_to_list.begin(), linked_to_list.end(), [](graph_vertex *a, graph_vertex *b) { return a->title < b->title; });

    indicators::show_console_cursor(true);
    linked_to_list.shrink_to_fit();
    return linked_to_list;
}


// Find all the pages linking to a given page in a segment of the graph
std::vector<graph_vertex *> wiki_graph::all_linked_to_segment(const std::string linked_to, const int segment_start, const int segment_end, indicators::DynamicProgress<indicators::BlockProgressBar> &bars) {
    std::vector<graph_vertex *> linked_to_list;
    const int segment_size{segment_end - segment_start};
    const int segment_num{(segment_end / segment_size) - 1};

    for (int i = segment_start; i < segment_end; ++i) {
        // Progress bar
        const double percent{100 * ((double)(i - segment_start) / (segment_size - 1))};
        if ((i - segment_start) % (segment_size / 1000) == 0 || percent >= 100) {  // Only update the progress bar every 0.1% of the segment
            if (bars[segment_num].is_completed() || percent >= 100) {
                bars[segment_num].set_option(indicators::option::ShowRemainingTime{false});
                bars[segment_num].set_option(indicators::option::ForegroundColor{indicators::Color::green});
            }
            bars[segment_num].set_progress(percent);
        }

        // Actual work
        for (const auto &link : vertex_list[i].adjacent) {
            if (link->title == linked_to) {
                linked_to_list.push_back(&vertex_list[i]);
                break;
            }
        }
    }
    bars[segment_num].set_option(indicators::option::PrefixText{"Found all linked to in section " + std::to_string(segment_num + 1) + '/' + std::to_string((int)(vertex_list.size() / segment_size)) + ' '});
    linked_to_list.shrink_to_fit();
    return linked_to_list;
}