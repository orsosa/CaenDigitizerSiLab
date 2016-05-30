lib = CaenDigitizerSiLab
SRCS = $(lib).cxx # source files
OBJS = $(SRCS:.cxx=.o)

all: lib$(lib).so.1.0.1 

example: example.o
	g++ -L. -lCaenDigitizerSiLab -lCAENDigitizer -o $@ $^

example.o : example.cxx
	g++ -I. -c -o $@ $^ 


lib$(lib).so.1.0.1 : $(OBJS)
	g++ -shared -Wl,-soname,lib$(lib).so.1 -o $@ $^ -lc

%.o : %.cxx
	g++ -g -c -Wall -fPIC $^ -o $@