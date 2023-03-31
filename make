#!/bin/sh

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR/source/C++
g++ -std=c++20 -Wextra -Wall -Werror -D_GLIBCXX_ASSERTIONS -O3 *.cpp -o run
echo "\nCompiled successfully!"