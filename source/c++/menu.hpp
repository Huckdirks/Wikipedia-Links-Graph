#ifndef MENU_HPP
#define MENU_HPP

// Files to include
#include "utility.hpp"
#include "wiki_graph.hpp"

// Libraries
#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
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
    void display_wiki_info();
    bool y_or_n();

   private:
    wiki_graph graph;
};

// Menu Interface
class menu : public menu_calls, public python_init {
   public:
    menu() { init_menu(); }

   private:
    int init_menu();
    int interface();
    int selector();
};

#endif