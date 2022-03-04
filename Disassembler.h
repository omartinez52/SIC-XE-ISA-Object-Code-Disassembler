 /**
 *  Disassembler.h
 *  - Instantiates all private variables to be initialized during execution
 *    of Disasembler.cpp.
 *  - Instantiates all class function prototypes for execution of 
 *    Disasembler.cpp. 
 *  @course: [CS530-01 : (Professor: Brian Shen)]
 *  @authors:
 *      - [Omar Martinez(REDID:818749029)]
 *      - [Nick Riazanov(REDID:819099418)]
 */
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <tuple>

using namespace std;

class Disassembler
{
private:
    // write to file w/o overwritting
    ofstream ofs;
    // vector containing object code
    vector<string> object_code;
    // holds all address for literals used
    vector<int> literals;
    // starting address of program
    string start_address;
    // program name
    string prog_name;
    // program length
    string prog_length;
    // program counter. Int can be converted to hex.
    int pc;
    // holds register char in index based on reg value
    vector<char> registers;
    // base register
    string base;
    // hashmap that holds mnemonic keys and op value
    map<string, string> mnemonic_op;
    // mapping symbol to address. [key,value] = [address, symbol]
    map<string, string> symtab;
    // mappng literals to their address.
    //[key,value] = [address, (Lit_Name,Lit_Const, size)]
    map<string, tuple <string,string,unsigned int>> littab;         
public:
    // overloaded constructor
    Disassembler(const string m[], const string o[]);
    //default constructor
    Disassembler();
    //~Disassembler();
    // reading in .obj file and storing into vector
    bool read_obj_file(string filename);

    // reading in .sym file and mapping symbol to literal.
    bool read_sym_file(string filename);

    // reads first two bytes of obj code to determine format.
    bool is_format_2(string opcode);  

    // reads the flag bit in the object code to determine format.
    bool is_format_4(string obj_code);

    // reads the flag bit in the object code to determine format.
    bool is_format_3(string obj_code);

    // checks if address is a constant in littab
    bool is_in_littab(unsigned int addr);

    // reads Header row and extracts name, start addr & prog length
    void read_header_row();

    // reads object code in text records.
    void read_text_record();

    // prints end record statement
    void read_end_record();

    // capitalizes alphabetical chars in hex address.
    void to_uppercase(string &hex_addr);

    // closes file which logs SIC/XE source statements
    void close_file();                 

    // converts decimal integer into hexidecimal string 
    string decimal_to_hex(unsigned int decimal);

    // get symbol from symtab at given address.
    string get_symbol(unsigned int addr);

    // converting hex string to decimal
    unsigned int hex_to_int(string hex_address);

    // adds 0s to front of hex address number of bits needed is reached.
    void add_offset_bits(int num_bits, string &hex_addr);

    // writes format 2,3, & 4 statements to file
    void write_to_output(unsigned int cur_addr,string obj_code,string mnemonic);

    // get literal at current address.
    tuple<string,string,unsigned int> get_literal(unsigned int addr);

    // parse through littab to get all information
    void parse_literal(string littab_row);

    // parse through symtab to get all information
    void parse_symbol(string symtab_row);

    // reads first byte of object code and returns mnemonic operation
    string get_mnemonic_op(string byte,int format);

    // get operande addressing mode from object code.
    void get_addressing_mode(string obj_code, vector<int> &vec);
};