Let's consider this question: "How you would you find out if a machine's stack grows up or down in memory?"

In Linux it's not possible to accurately calculate the stack size of another process thread from the current process at a given time (neither for the user space stack nor for the kernel space stack), but it's possible to obtain the stack size reserved by the operating system for a particular thread of another user space process.

The stack size of a machine can be defined as the total size of stack memory reserved in user space for all processes running on the current machine at a given time + the total size of stack memory reserved in kernel space for all processes running on the current machine at a given time. The size of stack memory reserved in user space for a particular process can be defined as the total size of stack memory reserved in user space for all threads of that process.

From the user's point of view, this program consists of three parts. The first part of the program sums up the size of stack memory reserved in user space for all processes running on the current machine at a given time. Reading information about processes and threads is implemented based on the procps library. The second part of the program reads the total size of stack memory reserved in kernel space for all processes running on the current machine at a given time. The source of this information is the 'nr_kernel_stack' parameter from the '/proc/vmstat' file. If the given parameter couldn't be read from the specified file, an attempt is made to read the same parameter from the file '/proc/zoneinfo'. If both reading options don't work, the program exits. The first and second parts of the program are triggered once per minute. The third part of the program compares the current results with the results obtained one minute ago, and displays the difference characteristics in kilobytes to the console once per minute. The program can be stopped by the user at any time by pressing the key combination 'Ctrl+C'.

For the purity of the experiment, before starting the program you need:
a) terminate all processes launched using the 'vfork' system call, as well as prevent similar processes from running for the entire duration of the program;
b) terminate all processes representing other running machines with their own operating systems (for example, containers (Docker, Podman, ...), virtual machines (VirtualBox, VMware, ...)), and also prevent similar processes from running for the entire duration of the program.

The conditions under which this program was developed and tested:

Docker 27.1.2
Container OS: Ubuntu 22.04.4 LTS
C++ standard: C++20
Compiler: Clang 18.1.8
CMake 3.30.2
GNU Make 4.3
boost 1.86.0
procps 2:3.3.17-6ubuntu2.1
