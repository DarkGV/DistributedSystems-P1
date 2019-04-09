#include "Tab.hpp"



Tab::Tab(int id){
    this->id = id;
    running = true;
    std::thread(&Tab::listenSocket, this).detach();
}

void Tab::listenSocket(){
    
    while(running){
        packet = proto.recvMsg();
        //std::cout << (unsigned int)packet->id << " " << this->id << std::endl;
        // && <- in C++ if the left result is false, the right one is never tested
        if(packet != NULL && (unsigned int)packet->id == this->id){
            switch ( (unsigned int)packet->status) {
                case LOAD_URL:
                    // There must be a payload defined !
                    pageUrl = std::string(packet->payload);
                    std::thread( &Tab::loadPage, this, std::string(packet->payload) ).detach();
                    sleep(1);
                    std::thread(&Tab::must_kill, this, (unsigned int)packet->ttl).detach();
                    break;
                case KILL:
                    running = false;
                    ex = false;
                    break;
                case LIST:
                    std::cout << "\e[32m" <<  pageUrl << "\e[0m (" << id+1 << ")" << std::endl;
            }
        }
    }

    return ;

}

void Tab::loadPage(std::string URL){
    struct sockaddr_in serverStruct;
    int commTunnel;
    std::string command, response;
    char buffer[1024];
    std::ifstream nmr;
    //std::ofstream tosave;
    // first we resolve domain name
    ex = true;
    if(inet_addr(URL.c_str()) == -1){
        struct hostent *he;
        struct in_addr **addr_list;
        if( (he = gethostbyname(URL.c_str())) == NULL){
            this->running = false;
            ex = false;
            std::cout << "Could not find host" << std::endl;
            return;
        }
        
        addr_list = (struct in_addr **) he->h_addr_list;
        serverStruct.sin_addr = *addr_list[0];
    }
    else
        serverStruct.sin_addr.s_addr = inet_addr(URL.c_str()); 

    serverStruct.sin_family = AF_INET;
    serverStruct.sin_port = htons(80);
    
    if((commTunnel = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        std::cout << "Could not create socket for connection" << std::endl;
    
    if(connect(commTunnel, (struct sockaddr*)&serverStruct, sizeof(serverStruct)) < 0)
        std::cout << "Could not connect to remote server!" << std::endl;

    command = "GET / HTTP/1.1\nHost: " + URL + std::string("\n\n");
    if(send(commTunnel, command.c_str(), command.size(), 0) < 0)
        std::cout << "Connection lost" << std::endl;
    int readsize;

    int len = 0;
    // wait for input!
    while(!len && ioctl(commTunnel, FIONREAD, &len) >= 0 && ex) sleep(1);

    // read all input
    while(len && ex){
        read(commTunnel, buffer, 1023);
        response += std::string(buffer);
        len = 0;
        ioctl(commTunnel, FIONREAD, &len);
    }
    ex = false;
    if(!running) return;

    std::string file = std::string("saved/") + pageUrl;

    std::ofstream tosave(file);
    if(!tosave) std::cout << "Erro ao criar ficheiro" << std::endl;
    tosave << response;
    tosave.close();

    std::cout << "\e[42mTab " << id+1 << "\e[0m saved file " << pageUrl << "(\e[44m" << response.size() << "\e[0m)" << std::endl;
}

bool Tab::is_running(){
    return running;
}

void Tab::must_kill(int ttl){
    while( (--ttl) && ex) sleep(1);

    if(ex) {
        running = false;
        std::cout << "\e[41mTimeout limit exceeded! (" << id+1 << ")\e[0m" << std::endl;
    }
}

void Tab::runningOff() {
    running = false;
}
