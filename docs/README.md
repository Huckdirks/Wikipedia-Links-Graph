# Wikipedia Links Graph

## Table of Contents

- [Introduction](#introduction)
- [Uses](#uses)
- [Compiling & Running](#compiling--running)
    - [Dependencies](#dependencies)
    - [Compiling](#compiling)
    - [Running](#running)
- [Quality Assurance](#quality-assurance)
- [Future Features](#future-features)
- [Suggestions](#suggestions)
- [Contributing](#contributing)
- [License](#license)

## Introduction

A while ago, I had the question of what was the most linked to Wikipedia page by other Wikipdedia pages. I'd already found the [page with the most links to other pages](https://en.wikipedia.org/wiki/Wikipedia:Articles_with_the_most_references): [*List of Android smartphones*](https://en.wikipedia.org/wiki/List_of_Android_smartphones), but I couldn't find anywhere that answered my question. I guess I was kinda stupid, because as of 4/3/23 I found out that Wikipedia already had the answers I guess: [*Most Linked to Pages*](https://en.wikipedia.org/wiki/Special:MostLinkedPages) & [*Pages Linking to a Page*](https://en.wikipedia.org/wiki/Special:WhatLinksHere). Since I wasn't able to find the answers on Wikipedia (Even though it was [always there](https://en.wikipedia.org/wiki/Wikipedia:Wikipedia_records#Links) 🤦), I decided to make my own program to find the answers. At first, I had to download all of Wikipedia, and then extract all the titles and links from the pages. To do this, I copied most of my Python section from [*Will Koehrsen's Downloading and Parsing Wikipedia Articles*](https://github.com/WillKoehrsen/wikipedia-data-science/blob/master/notebooks/Downloading%20and%20Parsing%20Wikipedia%20Articles.ipynb). There was still a fair amount that I had to figure out and change to actually get it working (eg. Inside In [4]: it tells you to use `soup_dump.find_all('li', {'class': 'file'}, limit = 10)[:4]`, when it actually needs `soup_dump.find_all('a')`) so it still took me quite some time to figure out the intention of the code & all the libraries to update & fix it. Once I figured out how to download & parse all of Wikipedia into `json` files using Python, I wrote the analysis section of the program in C++. The program loads all of the data parsed by the Python section into a graph (using the [Adjacency List](https://www.programiz.com/dsa/graph-adjacency-list) implementation), which can then be analyzied by the methods below!

## Uses

In this current version, after loading in the data into the graph, the user can find all the information about a given page, information about the graph/Wikipedia as a whole, find the most linked to pages up to a user specified number, or find all the pages linking to a page. Any pages inputted are case sensitive!!! All data the user decides to save is saved in the `data/user_data` folder. The program also saves the data it needs to load in from the `data/load` folder, so it doesn't have to dowload & parse all of Wikipedia every time it's run.

If you want to learn about how it does this, or how to call the functions yourself, check out the [Program Structure](https://github.com/Huckdirks/Wikipedia_Links_Graph/wiki/Program-Structure#c) page in the wiki.

## Compiling & Running

### Dependencies

#### Install

Double click `dependencies`, or run `bash dependencies` or `./dependencies` in the root directory or to install the python dependencies. All the c++ dependencies are included in `source/c++` already. You must have [pip](https://pip.pypa.io/en/stable/installation/) installed to download the new dependencies. Also, you'll need to install python yourself if you haven't already.

**[List of Dependecies](DEPENDENCIES.md)**

### Compiling

Double click `compile`, or run `bash compile` or `./compile` in the command line in the root directory. You must have a version of gcc or clang that supports c++20 installed.

- [GCC](https://gcc.gnu.org/)
- [Clang](https://clang.llvm.org/)

### Running

**YOU HAVE TO COMPILE & INSTALL THE DEPENDENCIES BEFORE TRYING TO RUN THE PROGRAM!!!**

Double click `run`, or run `bash run` or `./run` in the command line in the root directory.

## Quality Assurance
Every new release is run with with [leaks](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/FindingLeaks.html) (the apple version of valgrind) to ensure there are no memory leaks, and the program is compiled with `-Wall` & `-Wextra` for getting as much standardization as possible, and `-Werror` to make sure all errors are dealt with before the files can be compiled. All variable, function, classe, module, & file names are written in [snake_case](https://en.wikipedia.org/wiki/Snake_case) to make sure everything is consistent, and all `const` variables are written in ALL-CAPS. The code is also quite commented, so it should be easy enough to understand what's going on.

Also, I know the python section could probably be more ✨𝒫𝓎𝓉𝒽ℴ𝓃𝒾𝒸✨, but I just started seriously learning python, so I'm sure there are many things I could improve on!

If there are any other/better ways to check for quality assurance, please let me know in the [suggestions](https://github.com/Huckdirks/Wikipedia_Links_Graph/discussions/new?category=suggestions)!

## Future Features

For any news on future features, or if you want to suggest some of your own, check out [FUTURE_FEATURES.md](FUTURE_FEATURES.md).

## Suggestions

If you have any suggestions about anything, please create a [new discussion in suggestions](https://github.com/Huckdirks/Wikipedia_Graph/discussions/new?category=suggestions). I'm only a second year computer science student, so I'm sure there are many things I could improve on!

## Contributing

Contributions are always welcomed! Look at [CONTRIBUTING.md](CONTRIBUTING.md) for more information.

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
