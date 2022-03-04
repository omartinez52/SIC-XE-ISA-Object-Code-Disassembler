## Assignment 1: Disassembler
***

#### Summary:
Design and implement a disassembler for the XE variant of the SIC/XE architecture.
The disassembler opens the object code file along with its accompanying symbol 
file and then it disassembles the object code and saves the generated assembly 
listing file. 
Disassembler supports the translation between all 59 operation codes 
(except format 1 instructions) and their corresponding mnemonics in the SIC/XE 
architecture.

## Table of Contents
1. [General Info](#general-info)
2. [Technologies](#technologies)
3. [Preparation](#preparation)
4. [Collaboration](#collaboration)

### General Info
***
The main file, main.cpp, makes calls to Disassembler Class functions to execute 
all operations necessary in disassembling object code into assembly source code.

First, all files(.obj & .sym) are read and information from the files those files
are stored in memory for later use. 
Second, iterate through each text record and parse object code from string of
hexidecimal numbers. 
Third, determine format of object code or determine if current address is a literal.
Fourth, output assembly source code into output file after each iteration.
Lastly, close output file after all assembly statements have been written to file.
## Technologies
#include <string>   // including string variables
#include <map>      // mapping keys to values
#include <vector>   // list variable
#include <sstream>  // string to hex or vice versa
#include <bitset>   // converting to binary
#include <tuple>    // storing multiple values in one variable
#include <fstream>  // reading and writing to files
#include <iostream> // input output stream
## Preparation
***
- Effectively parse through symbole file to correctly parse all information.

- Ensure mapping of address to symbol value or literal values is correct.

- Ensure mapping of mnemonic operations to their hexidecimal values.

- Ensure text record parsing checks format conditions for correct assembly statement
output.

- Run tests using provided files as well as testing through gradescope to ensure
all test cases have been met and passed.

- Account for hidden test features not provided in testing files.

## Collaboration
***
Team collaboration to properly develop an object code disassembler that passes
all test cases when reading records.

Omar Martinez (REDID:818749029) - Contribution:

- Format Disassembler class and brainstorm needed variables and functions to properly
create an object code disassembler. 

- Implement functions in a seperate .cpp file for class functions.

- Wrote code for reading files, disassembling object code, writing assembly statements
to output file, and debug any errors provided from gradescope.

- Wrote main.cpp, formatted Disassembler.h and formatted Disassembler.cpp.

- Wrote Makefile and README.md.

- Create hashmap for literal table, symbol table and mnemonic operations.

- Implement 2's compliment for object code which represents a negative number.

- Check flag bits of object code to ensure proper operand and relative addressing.

- Implement hex conversion to int and vice versa.

- Format output file so all statemtents are inline with each other.