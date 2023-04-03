#ifndef WIKI_GRAPH_HPP
#define WIKI_GRAPH_HPP

// Files to include
#include "indicators/indicators.hpp"

// Libraries
#include <algorithm>
#include <deque>
#include <execution>
#include <future>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Classes & Structs

// Graph Vertex
class graph_vertex {
   public:
    std::string title;
    std::vector<graph_vertex *> adjacent;  // Adjacent = links
    int links_to{};

    // Constructors (I only had to do this because -Werror=deprecated-copy from -Wextra)
    graph_vertex() = default;                                // Default constructor
    constexpr graph_vertex(const graph_vertex &) = default;  // Copy constructor

    void operator=(const graph_vertex &VERTEX);
    int display(const bool DISPLAY_LINKS) const;
};

// Graph
// Also a wrapper for deque<graph_vertex> vertex_list inside wiki_graph
class wiki_graph {
   public:
    graph_vertex &operator[](const unsigned int INDEX);  // [] operator overload for ints
    graph_vertex &operator[](const std::string VALUE);   // [] operator overload

    int load();                                                        // Load graph from file
    graph_vertex &front();                                             // Get first vertex
    graph_vertex &back();                                              // Get last vertex
    bool empty() const;                                                // Check if empty
    int size() const;                                                  // Get size
    void push_back(const graph_vertex VERTEX);                         // Push vertex to back
    graph_vertex *find(const std::string TO_FIND);                     // Binary search for value wrapper function
    int find_index(const std::string TO_FIND);                         // Binary search for index wrapper function
    std::vector<graph_vertex *> top_n(const int N);                    // Get top (n) most linked to vertices & display
    std::vector<graph_vertex *> linked_to(const std::string TO_FIND);  // Get all pages with links to a page

   private:
    // Adjacency list implementation
    std::deque<graph_vertex> vertex_list;  // Can use a vector instead, but holding all the pages contiguously in memory is a bad idea

    graph_vertex *binary_search(const std::string TO_FIND);                                                                                                                                      // Binary search for value
    int binary_search_index(const std::string TO_FIND);                                                                                                                                          // Binary search for index
    std::vector<graph_vertex *> top_n_linked(const int N);                                                                                                                                       // Get top (n) most linked to vertices
    std::vector<graph_vertex *> top_n_linked_segment(const int N, const int SEGMENT_START, const int SEGMENT_END, indicators::DynamicProgress<indicators::BlockProgressBar> &bars);              // Get top (n) most linked to vertices in a segment of the graph
    std::vector<graph_vertex *> all_linked_to(const std::string PAGE);                                                                                                                           // Get all pages with links to a page
    std::vector<graph_vertex *> all_linked_to_segment(const std::string PAGE, const int SEGMENT_START, const int SEGMENT_END, indicators::DynamicProgress<indicators::BlockProgressBar> &bars);  // Get all pages with links to a page in a segment of the graph
};

#endif