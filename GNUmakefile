#OBJS = MuDst.o analysis_LMH.o
#EXE = analysis_LMH 

OBJS =Aging.o
EXE = PmtAging
 

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs) -lSpectrum

INCFLAGS = -I$(ROOTSYS)/include
LDFLAGS = -L$(ROOTSYS)/lib

CXX = g++
FLAGS = -Wall -g $(INCFLAGS) $(LDFLAGS)

COMPILE = $(CXX) $(FLAGS) -c 

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(ROOTFLAGS) $(ROOTGLIBS)

%.o: src/%.cxx
	$(COMPILE)  $< 

clean:
	rm -rf obj/* dict/* *.so;
