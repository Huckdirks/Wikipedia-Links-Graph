#ifndef UTILITY_HPP
#define UTILITY_HPP

// Files to include
#include "indicators/indicators.hpp"

// Libraries
#include <csignal>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <Python.h>
#include <string>
#include <vector>

// Namespaces
namespace fs = std::filesystem;

// Python Initializer
class python_init {
   public:
    python_init() = default;
    ~python_init() = default;
    int init_python();

   private:
    FILE *python_file{};
    const std::wstring PYTHON_PATH_WIDE{fs::current_path().parent_path().wstring() + L"/python/"};
    const std::string PYTHON_PATH{fs::current_path().parent_path().string() + "/python/"};
    std::vector <PyObject *> python_modules{};
    
    int set_paths();
};

// Signal Handler
void termination_handler(int signal);

#endif