# Wikipedia Links Graph

## Table of Contents

- [Introduction](#introduction)
- [Uses](#uses)
- [Dependencies](#dependencies)
- [Compiling & Running](#compiling--running)
- [Quality Assurance](#quality-assurance)
- [Future Features](#future-features)
- [Suggestions](#suggestions)
- [Contributing](#contributing)
- [License](#license)

## Introduction

(Insert intro stuff here)

## Uses

In this current version, after loading in the data into the graph, the user can either find the most linked to pages up to a user specified number, or find all the pages linking to a page. Any pages inputted are case sensitive!!!

If you want to learn about how it does this, or how to call the functions yourself, check out the [Program Structure](https://github.com/Huckdirks/Wikipedia_Links_Graph/wiki/Program-Structure#c) page in the wiki.
## Dependencies

### Install

Double click `dependencies`, or run `bash dependencies` or `./dependencies` in the root directory or to install the python dependencies. All the c++ dependencies are included in `source/C++` already. You must have [pip](https://pip.pypa.io/en/stable/installation/) installed to download the new dependencies. Also, you'll need to install python yourself if you haven't already.

**[List of Dependecies](DEPENDENCIES.md)**

## Compiling & Running

### Compiling

Double click `compile`, or run `bash compile` or `./compile` in the command line in the root directory. You must have a version of gcc or clang that supports c++20 installed.

- [GCC](https://gcc.gnu.org/)
- [Clang](https://clang.llvm.org/)

### Running

**YOU HAVE TO COMPILE & INSTALL THE DEPENDENCIES BEFORE TRYING TO RUN THE PROGRAM!!!**

Double click `run`, or run `bash run` or `./run` in the command line in the root directory.

## Quality Assurance
Every new release is run with with [leaks](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/FindingLeaks.html) (the apple version of valgrind) to insure there are no memory leaks, and I compile with `-Wall` & `-Wextra` for getting as much standardization as possible, and `-Werror` to make sure all errors are dealt with before the files can compile.

If there are any other/better ways to check for quality assurance, please let me know!
Also, I know the python section could be more 

✨𝒫𝓎𝓉𝒽ℴ𝓃𝒾𝒸✨, but I just started seriously learning python, so I'm sure there are many things I could improve on!

## Future Features

For any news on future features, or if you want to suggest some of your own, check out [FUTURE_FEATURES.md](FUTURE_FEATURES.md)

## Suggestions

If you have any suggestions about anything, please create a [new discussion in suggestions](https://github.com/Huckdirks/Wikipedia_Graph/discussions/new?category=suggestions). I'm only a second year computer science student, so I'm sure there are many things I could improve on!

## Contributing

Contributions are always welcomed! Look at [CONTRIBUTING.md](CONTRIBUTING.md) for more information.

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
