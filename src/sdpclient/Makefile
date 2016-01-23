CXXFLAGS=-O2 -W -Wall -std=c++11
OBJECTS=sdp.o main.o serial.o logger.o

all: sdpclient

clean:
	-rm -rf sdpclient $(OBJECTS)

sdpclient: $(OBJECTS)
	$(CXX) -o $@ $+ -lpthread
