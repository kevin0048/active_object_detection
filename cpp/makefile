BIN=bin/human-detect
SRC=src/main.cpp src/ActiveObjectDetector.cpp
OBJ=obj/main.o obj/ActiveObjectDetector.o
CXXFLAGS=-Wall -Wextra
LDFLAGS=`pkg-config --libs "opencv"` -lraspicam_cv -lraspicam

all: $(BIN)

$(BIN): obj $(OBJ)
	-mkdir bin
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)

obj:
	mkdir obj

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm -rf bin
	-rm -rf obj
	echo Clean done
