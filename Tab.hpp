#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <iostream>

#include "Protocol.hpp"
#include <netdb.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <unistd.h>


class Tab{
    Protocol proto;
    struct protohdr* packet;
    bool running, ex;
    std::string pageUrl;
    

public:
    int id;
    Tab(int);
    void listenSocket();
    void loadPage(std::string);
    void must_kill(int);
    bool is_running();
    void runningOff();
};
