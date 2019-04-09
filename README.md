# Browser - Distributed Systems First Project

## About
This project was written in C++ and was used Linux socket libraries including the core lirary [linux/ip.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/ip.h).

## How to compile
In order to get easier to compile a MAkefile was added, so to compile, just run make

After that a file named browser will be generated. Just run it

```
user:~/DistributesSystems-P1$ make
user:~/DistributesSystems-P1$ ls
browser Browser.hpp main.cpp Protocol.cpp Protocol.o Tab.cpp Tab.o Browser.cpp Browser.o Makefile Protocol.hpp README.md Tab.hpp
user:~/DistributesSystems-P1$ sudo ./browser
```

## Idea Behind this implementation
For this application, we managed to create our own protocol for security purposes, simulating a real life event (if a personal protocol is implemented, not following standards, is more difficult to get and forge a packet). Security was not the main focus of this project, since it was to learn better thread functionality in the language used.

The main Browser waits for commands and send orders to Tabs, using our own Packet definition. Tabs makes whatever is written inside the package, including manage time they have to execute an instruction.

The main browser can limit the memory usage, forbiding new tabs to open but not closing existing tabs.

The user is the most important part of this application, so he has it in his hands, that's why setting a memory lower than the existing doesn't make a memory reduce, some tabs might be processing sensitive data, and the user might not want to kill those.

## How to use
This application has a help command to get easier for user to interact with it