#ifndef WIKI_LOADER_HPP
#define WIKI_LOADER_HPP

// Files to include
#include "indicators/indicators.hpp"
#include "json.hpp"
#include "wiki_graph.hpp"

// Libraries
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

// Namespaces
namespace fs = std::filesystem;
using json = nlohmann::json;

// Classes & Structs

// File IO
class wiki_loader {
   public:
    /* wiki_loader(wiki_graph &Graph) {
        graph = &Graph;
    } */
    wiki_loader(wiki_graph &Graph) : graph{&Graph} {}
    int load();

   private:
    std::ifstream file_in;
    wiki_graph *graph;
    void load_title(std::set<std::string> &titles);
    void load_links(indicators::BlockProgressBar &bar, unsigned int &progress);
};

#endif