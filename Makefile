lib = CaenDigitizerSiLab
SRCS = $(lib).cxx # source files
OBJS = $(SRCS:.cxx=.o)

ROOTCONFIG := root-config
ROOTCFLAGS := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS := $(shell $(ROOTCONFIG) --ldflags)
ROOTGLIBS := $(shell $(ROOTCONFIG) --glibs)

CXX := g++
CXXFLAGS := -O2 -Wall -fPIC $(ROOTCFLAGS)
LD := g++
LDFLAGS := -O2 $(ROOTLDFLAGS)

INCLUDES := -I.
LIBS := $(ROOTGLIBS) -lSpectrum -lEG
HEADERS := $(lib).h

all: lib$(lib).so.1.0.1 example

example: example.o
	$(CXX) $(LIBS) -L. -lCaenDigitizerSiLab -lCAENDigitizer $(LDFLAGS)  $^ -o $@

example.o : example.cxx
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c  $^ -o $@


$(lib)Dict.cxx:  $(HEADERS) LinkDef.h
	rootcint -f $(lib)Dict.cxx -c $(CXXFLAGS) -p $^

lib$(lib).so.1.0.1 : $(OBJS) $(lib)Dict.cxx
	$(CXX) $(LDFLAGS) $(LIBS) $(CXXFLAGS)  -shared -Wl,-soname,lib$(lib).so.1  $^ -lc -o $@

%.o : %.cxx
	$(CXX) $(CXXFLAGS) -g -c -Wall -fPIC $^ -o $@


