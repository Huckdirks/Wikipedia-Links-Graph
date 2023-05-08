#ifndef WIKI_LOADER_HPP
#define WIKI_LOADER_HPP

// Files to include
#include "BS_thread_pool.hpp"
#include "indicators/indicators.hpp"
#include "json.hpp"
#include "wiki_graph.hpp"

// Libraries
#include <exception>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <vector>

// Namespaces
namespace fs = std::filesystem;
using json = nlohmann::json;

// Classes & Structs

// File IO
class wiki_loader {
   public:
    wiki_loader(wiki_graph &Graph) : graph{&Graph} { /* file_in.exceptions(std::ifstream::failbit | std::ifstream::badbit); */ }
    int load();

   private:
    //std::ifstream file_in;
    wiki_graph *graph;
    std::set<std::string> load_titles(const std::string FILE_NAME, unsigned int &progress, const unsigned int NUM_FILES, indicators::BlockProgressBar &title_bar);
    int load_title(std::set<std::string> &titles, std::ifstream &file_in);
    //int load_links(std::ifstream &file_in);
    int load_page_links(std::ifstream &file_in, indicators::BlockProgressBar &bar, unsigned int &progress);
};

#endif