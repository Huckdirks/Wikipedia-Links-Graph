#ifndef MENU_HPP
#define MENU_HPP

// Files to include
#include "wiki_graph.hpp"

// Standard Libraries
#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <string>

// Namespaces
namespace fs = std::filesystem;

// Classes


// Menu Functions
class menu_calls {
   public:
    int load();
    int display_page();
    int display_top_n();
    int display_linked_to();
    int display_wiki_info();
    bool y_or_n();

   private:
    wiki_graph graph;
};

// Menu Interface
class menu : public menu_calls {
   public:
    menu() { init_menu(); }

   private:
    int init_menu();
    int interface();
    int selector();
};

#endif