CXXFLAGS=-O2 -W -Wall
OBJECTS=sdp.o main.o

all: sdpclient

clean:
	-rm -rf sdpclient $(OBJECTS)

sdpclient: $(OBJECTS)
	$(CXX) -o $@ $+
