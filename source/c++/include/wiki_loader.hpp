#ifndef WIKI_LOADER_HPP
#define WIKI_LOADER_HPP

// Files to include
#include "BS_thread_pool.hpp"
#include "indicators/indicators.hpp"
#include "json.hpp"
#include "wiki_graph.hpp"

// Libraries
#include <atomic>
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
    wiki_loader(wiki_graph &Graph) : graph{&Graph} {}
    int load();

   private:
    wiki_graph *graph;
    int load_title(std::set<std::string> &titles, std::ifstream &file_in);
    //int load_links(std::ifstream &file_in, indicators::BlockProgressBar &bar, unsigned int &progress);
    int load_links(std::ifstream &file_in, indicators::BlockProgressBar &bar, std::atomic<unsigned int> &progress);
};

#endif