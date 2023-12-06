ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

#ROOTCFLAGS    = $(shell /usr/bin/root-config --cflags)
#ROOTLIBS      = $(shell /usr/bin/root-config --libs)
#ROOTGLIBS     = $(shell /usr/bin/root-config --glibs)

CXX           = g++
CXXFLAGS      = -g -Wall -fPIC -Wno-deprecated

NGLIBS         = $(ROOTGLIBS)
NGLIBS        += -lMinuit

CXXFLAGS      += $(ROOTCFLAGS)
CXX           += -I./
LIBS           = $(ROOTLIBS)

GLIBS          = $(filter-out -lNew, $(NGLIBS))

CXX	      += -I./obj/
OUTLIB	      = ./obj/
.SUFFIXES: .C
.PREFIXES: ./obj/

#----------------------------------------------------#

plotFCN: plotFCN.cpp
	g++ -o plotFCN plotFCN.cpp `root-config --cflags --glibs ` -lSpectrum

fit: fit.cpp
	g++ -o fit fit.cpp `root-config --cflags --glibs ` -lSpectrum

clean:
	rm -f plotFCN
	rm -f *~
