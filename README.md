# stack_size_summarizer

### Project Description

This project provides a tool called stack_size_summarizer that tracks stack size usage of processes on a Linux system. It periodically reads the `/proc` filesystem and calculates the total user-space stack size and the total kernel-space stack size used by all processes. It then compares these values with previous readings and reports changes that have occurred.

### Setup and Usage

Prerequisites:

- CMake 3.30.2 or higher
- C++20 compliant compiler
- Boost libraries installed on your system
- procps library installed on your system

To get started with stack_size_summarizer, clone the repository from GitHub:

```
$ git clone https://github.com/roman-empire-roman/stack_size_summarizer.git
$ cd stack_size_summarizer/
$ mkdir build
$ cd build/
$ cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-18 -DCMAKE_C_COMPILER=/usr/bin/clang-18 ../
$ make
```

Run the application:

```
$ ./stack_size_summarizer
```

### Versions Used

Below are the versions of tools and libraries used during development and testing:

- Boost: 1.86.0
- procps: 4.0.4
- C++ Standard: C++20
- C++ Compiler: Clang 18.1.8
- CMake: 3.30.2
- GNU Make: 4.3
- GNU gdb/gdbserver: 12.1
- Visual Studio Code: 1.95.3
- Podman: 4.9.3
- Podman Desktop: 1.14.1
- Host OS: Ubuntu 24.04.1 LTS
- Container OS: Ubuntu 22.04.5 LTS
- Git: 2.34.1

Ensure that your environment meets these or newer versions.

### Contact Information

Want to contribute, report an issue, or just chat about the project? Feel free to reach out to me:

- Telegram: [@roman_empire_roman](https://t.me/roman_empire_roman)
