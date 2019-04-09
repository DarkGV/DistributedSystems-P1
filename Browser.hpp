#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
//#include <mutex> //std::mutex -> The status needs a little time to check and get opened tabs!

#include "Tab.hpp"
#include "Protocol.hpp"

class Browser {
    std::vector<Tab*> tabs; // trocar para MAP

    int s, id;
    struct sockaddr_in sAddr;
    Protocol proto;
    int maxSize;
    int pageSize;
    std::vector<std::string> blockedWebsitesVector;

public:
    Browser();
    void reading();
    void createTab();
    std::vector<std::string> parseCommand(std::string);
    std::vector<std::string> blockedWebsites();
    void tabsStatus();
    bool is_blocked(std::string);
};
