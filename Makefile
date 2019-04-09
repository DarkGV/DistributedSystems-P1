DEPEND=Protocol.o Browser.o Tab.o
COMPILER=g++
FLAGS=-fpermissive -lncurses -pthread

all: Browser

Protocol.o: Protocol.hpp Protocol.cpp
	$(COMPILER) $(FLAGS) -c Protocol.cpp

Browser.o: Browser.hpp Browser.cpp
	$(COMPILER) $(FLAGS) -c Browser.cpp

Tab.o: Tab.hpp Tab.cpp
	$(COMPILER) $(FLAGS) -c Tab.cpp

Browser: $(DEPEND)
	$(COMPILER) $(FLAGS) -o browser main.cpp $(DEPEND)

clean:
	sudo rm -rf .sd
	sudo rm -rf saved
	rm *.o browser

clear:
	rm *.o browser
