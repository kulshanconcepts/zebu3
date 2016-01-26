CXXFLAGS=-O2 -W -Wall -std=c++11 -I./include
OBJECTS=sdp.o main.o serial.o logger.o

all: obj sdpclient

clean:
	-rm -rf sdpclient obj

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sdpclient: $(addprefix obj/, $(OBJECTS))
	$(CXX) -o $@ $+ -lpthread

obj:
	test -d obj || mkdir obj
