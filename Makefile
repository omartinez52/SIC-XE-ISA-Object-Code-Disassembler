# CXX Make variable for compiler
CXX=g++
# Make variable for compiler options
#	-std=c++11  C/C++ variant to use, e.g. C++ 2011
#	-g          include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -g

# output: main.o Disassembler.o
# 	g++ main.o Disassembler.o -o dissem

# main.o:	main.cpp
# 	g++ -c main.cpp

# Disassembler.o: Disassembler.cpp Disassembler.h
# 	g++ -c Disassembler.cpp

# Rules format:
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
dissem : Disassembler.o main.o 
	$(CXX) $(CXXFLAGS) -o dissem $^

main.o: main.cpp

Disassembler.o : Disassembler.cpp Disassembler.h

clean :
	rm *.o

