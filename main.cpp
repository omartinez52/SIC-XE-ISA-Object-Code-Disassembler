 /**
 *  main.cpp
 *  - Implements Disassembler Class and functions to disassemble object code.
 *  - Reads files provided as command line arguments through class functions.
 *  - Information from files are stored in Disassembler object.
 *  - Makes function calls to read text record object code and to close file
 *    after writing output.
 *  @course: [CS530-01 : (Professor: Brian Shen)]
 *  @authors:
 *      - [Omar Martinez(REDID:818749029)]
 *      - [Nick Riazanov(REDID:819099418)]
 */
#include <iostream> // input & output
#include <fstream>  // reading and writing to files
#include <vector>
#include <string>
#include "Disassembler.h"

using namespace std;


const static string ops[] = {
    "18", "58", "90", "40", "B4", "28",
    "88", "A0", "24", "64", "9C", "C4",
    "C0", "F4", "3C", "30", "34", "38",
    "48", "00", "68", "50", "70", "08",
    "6C", "74", "04", "D0", "20", "60",
    "98", "C8", "44", "D8", "AC", "4C",
    "A4", "A8", "F0", "EC", "0C", "78",
    "54", "80", "D4", "14", "7C", "E8",
    "84", "10", "1C", "5C", "94", "B0",
    "E0", "F8", "2C", "B8", "DC"
    };

const static string mnemonics[] = {
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
    "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
    "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
    "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
    "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR", "SIO", "SSK", "STA",
    "STB","STCH", "STF", "STI", "STL","STS",
    "STSW","STT", "STX", "SUB", "SUBF", "SUBR",
    "SVC","TD", "TIO", "TIX", "TIXR", "WD"
    };


int main(int argc, char *argv[]){
    // creating Disassembler object and passing arrays to create hashmap.
    Disassembler ds(mnemonics, ops); // [key,value] = [op,mnemonic]

    // calling method to open file test.obj and store into vector array
    if(!ds.read_obj_file(argv[1])){ // read in object code, return true if read. false otherwise.
        return 1;   // terminite program if file could not be read
    }

    // calling method to open file and map symbol and literal table from test.sym
    if(!ds.read_sym_file(argv[2])){
        return 1;   // terminate program if file could not be read
    }
    ds.read_header_row();   // read header record
    ds.read_text_record();  // read text records
    ds.read_end_record();   // print end record
    ds.close_file();
    return 0;
}