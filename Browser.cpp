#include "Browser.hpp"

#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>

void Browser::reading(){
    while(1){
   
        std::string input;
        std::string first;
	    std::vector<std::string> command_parsed;
        std::getline(std::cin, input);

        try{
            command_parsed = parseCommand(input);
        }catch(char const* e){
            std::cout << "No command was sent. Please insert valid command";
            //sleep(2);
            std::cout << std::string(100, '\n');
            continue;
        }

        if( command_parsed[0] == "create" && command_parsed.size() == 3){
            std::ifstream mem("/proc/self/statm");
            int memsize = -1;

            if(!mem) std::cout << "Não foi possível ler a memória a ser utilizado do programa." << std::endl;
            else mem >> memsize;

            if(memsize != -1 && maxSize != 0){
                if( (memsize*pageSize) + sizeof(Tab) > maxSize){
                    std::cout << "Máximo de memória atingido." << std::endl;
                    continue;
                }
                mem.close();
            }

            if(is_blocked(command_parsed[1])){
                std::cout << "Website is blocked" << std::endl;
                continue;
            }

            std::ifstream check("saved/" + command_parsed[1]);

            if(check) {
                std::cout << "Cached file, do you want to redownload? (last file downloaded will be overwritted)" << std::endl;;
                std::getline(std::cin, input);
                if(input == "n" || input == "N" ) continue;

                check.close();
            }

            createTab();
            proto.phdr->id = id-1;
	        proto.phdr->status = LOAD_URL;
	        proto.phdr->ttl = std::stoi(command_parsed[2]);
            proto.fillPayload(command_parsed[1].c_str(), command_parsed[1].size());
            proto.sendMsg("127.0.0.1", "127.0.0.1");
        }
        else if(command_parsed[0] == "kill" && command_parsed.size() == 2){
            proto.phdr->id = std::stoi(command_parsed[1])-1;
            proto.phdr->status = KILL;
	        proto.phdr->ttl = 0;
            proto.sendMsg("127.0.0.1", "127.0.0.1");
        }
        else if(command_parsed[0] == "help"){
            std::cout << std::string(100, '\n');
            std::cout << "\e[32m Comandos Disponíveis\n" << std::endl;

            std::cout << "\e[96mcreate\e[0m (Criar uma tab nova com o respetivo site e tempo de execução)" << std::endl;
            std::cout << "\t\e[95mURL\e[0m -> URL do site a ser acedido" << std::endl;
            std::cout << "\t\e[95mTTL\e[0m -> Tempo que a aba tem para executar\n" << std::endl;

            std::cout << "\e[96msend\e[0m (Pede a aba para processar outro pedido)" << std::endl;
            std::cout << "\t\e[95mID\e[0m -> ID da aba que vai processar o pedido" << std::endl;
            std::cout << "\t\e[95mURL\e[0m -> URL do site a ser acedido" << std::endl;
            std::cout << "\t\e[95mTTL\e[0m -> Tempo que a aba tem para executar\n" << std::endl;

            std::cout << "\e[96mset\e[0m (Definir número máximo de RAM a utilizar pelo Navegador)" << std::endl;
            std::cout << "\t\e[95mTamanho\e[0m -> Tamanho em bytes que o navegador pode ocupar\n" << std::endl;

            std::cout << "\e[96mlist\e[0m (Listar tabs abertas atualmente)\n" << std::endl;

            std::cout << "\e[96mkill\e[0m (Fecha uma aba)" << std::endl;
            std::cout << "\t\e[95mID\e[0m -> ID da aba a fechar [1, ..]\n" << std::endl;

            std::cout << "\e[96mblock\e[0m (Bloqueia um site)" << std::endl;
            std::cout << "\t\e[95mURL\e[0m -> URL do site a bloquear\n" << std::endl;

            std::cout << "\e[96munblock\e[0m (Desbloqueia um site)" << std::endl;
            std::cout << "\t\e[95mURL\e[0m -> URL do site a desbloquear\n" << std::endl;

            std::cout << "\e[96mblocked\e[0m (Lista sites bloqueados)\n" << std::endl;

            std::cout << "\e[96mhelp\e[0m (Ajuda no navegador)\n" << std::endl;

            std::cout << "\e[96mquit\e[0m (Sai do navegador)\n" << std::endl;

        }
        else if(command_parsed[0] == "list"){
            proto.phdr->status = LIST;

            if(id == 0){
                std::cout << "No tabs open" << std::endl;
                continue;
            }

            for(int i = 0; i < id; i++){
                proto.phdr->id = i;
                proto.sendMsg("127.0.0.1", "127.0.0.1");
            }
            
        }
        else if(command_parsed[0] == "send" && command_parsed.size() == 4){ // send 1 www.google.pt 2

            if( id <= (std::stoi(command_parsed[1])-1) || (std::stoi(command_parsed[1])-1) < 0){
                std::cout << "Tab nao existe" << std::endl;
                continue;
            }

            if(is_blocked(command_parsed[2])){
                std::cout << "Website is blocked" << std::endl;
                continue;
            }

            std::ifstream check("saved/" + command_parsed[1]);

            if(check) {
                std::cout << "Cached file, do you want to redownload? (last file downloaded will be overwritted)" << std::endl;;
                std::getline(std::cin, input);
                if(input == "n" || input == "N" ) continue;

                check.close();
            }

            proto.phdr->status = LOAD_URL;
            proto.phdr->id = (std::stoi(command_parsed[1])-1);
            proto.phdr->ttl = std::stoi(command_parsed[3]);
            proto.fillPayload(command_parsed[2].c_str(), command_parsed[2].size());
            proto.sendMsg("127.0.0.1", "127.0.0.1");
        }
        else if(command_parsed[0] == "set" && command_parsed.size() == 2){
            maxSize = std::stoi(command_parsed[1]);
        }
        else if(command_parsed[0] == "block" && command_parsed.size() == 2) {
            if( std::find(blockedWebsitesVector.begin(), blockedWebsitesVector.end(), command_parsed[1]) != blockedWebsitesVector.end())
                continue;
            blockedWebsitesVector.push_back(command_parsed[1]);
        }
        else if(command_parsed[0] == "blocked"){
            if(blockedWebsitesVector.empty()){
                std::cout << "\e[101mNão tem sites bloqueados de momento. Digite \e[0m \e[5mblock\e[0m \e[101m para bloquear um site\e[0m\n" << std::endl;
                continue;
            }
            for(int i = 0; i < blockedWebsitesVector.size(); i++)
                std::cout << blockedWebsitesVector[i] << std::endl;
        }
        else if(command_parsed[0] == "unblock" && command_parsed.size() == 2){
            for(int i = 0; i < blockedWebsitesVector.size(); i++){
                if(blockedWebsitesVector[i] == command_parsed[1]) {
                    blockedWebsitesVector.erase(blockedWebsitesVector.begin() + i);
                    break;
                }
            }
        }
        else if(command_parsed[0] == "quit"){
            proto.phdr->status = KILL;
            for(int i = 0; i < id; i++){
                proto.phdr->id = i;
                tabs[i]->runningOff();
                proto.sendMsg("127.0.0.1", "127.0.0.1");
            }

            std::ofstream block(".sd/.block", std::ios::out);

            for(std::string &website : blockedWebsitesVector){
                block << website;
                block << "\n";
            }

            return;
        }
        else
        {
            std::cout << "\e[101mComando não reconhecido! Digite \e[0m \e[5mhelp\e[0m \e[101m para listar comandos disponíveis\e[0m\n" << std::endl;
        }
        
    }
}

Browser::Browser(){
    id = 0;
    std::cout << std::string(100, '\n');
    std::thread(&Browser::tabsStatus, this).detach(); // thread that works as garbage collector
    mkdir(".sd", 0777);
    mkdir("saved", 0777);
    std::ifstream conf(".sd/.conf");
    if(!conf.is_open())
      maxSize = 0;
    else
      conf >> maxSize;
    conf.close();

    pageSize = getpagesize()/1024;
    blockedWebsitesVector = blockedWebsites();
}
// criar tab, mudar de tab, :1 google.com 2 

void Browser::createTab(){
    tabs.push_back( new Tab(id) );
    id++;
}

std::vector<std::string> Browser::parseCommand(std::string command){
    if(command.empty()) throw "No command to parse error";
    std::vector<std::string> splitCommands;
    std::string token;

    size_t pos = 0;

    while( (pos = command.find(' ') ) != std::string::npos ){
        token = command.substr(0, pos);
        splitCommands.push_back(token);
        command.erase(0, pos+1);
    }

    splitCommands.push_back(command);

    return splitCommands;
}

void Browser::tabsStatus(){

    while(1){

        for(int i = 0; i < tabs.size(); i++){
            if(!tabs[i]->is_running()){
                tabs.erase(tabs.begin() + i);
                this->id--;
            }
        }
        sleep(2);
    }

}

std::vector<std::string> Browser::blockedWebsites(){
    std::vector<std::string> websites;
    std::ifstream block(".sd/.block");
    std::string website;

    if(!block) return websites;

    block.clear();

    while(std::getline(block, website))
        websites.push_back(website);
    
    block.close();

    return websites;
}

bool Browser::is_blocked(std::string checkWebsite){

    if(blockedWebsitesVector.empty()) return false;
    for(std::string & website : blockedWebsitesVector){
        if(website.find(checkWebsite) != website.npos) return true;
    }

    return false;
}
