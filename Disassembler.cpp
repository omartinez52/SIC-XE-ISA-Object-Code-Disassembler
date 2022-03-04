/**
 *  Disassembler.cpp
 *  - Constructor maps mnemonic operations with their hex value.
 *  - Reads .sym files and .obj files to extract information for disassembling.
 *  - Map symbols and literals to their respective address for efficient search.
 *  - Iterates through text records to properly format assembly instructions.
 *  - Format assembly instructions with their respective start address, 
 *    symbol, mnemonic, operand and object code.
 *  - Implements hex to decimal conversions and vice versa.
 *  - Implements helper functions for efficiency and readability.
 *  - Completes execution when all records have been read and outputted. 
 *  @course: [CS530-01 : (Professor: Brian Shen)]
 *  @authors:
 *      - [Omar Martinez(REDID:818749029)]
 *      - [Nick Riazanov(REDID:819099418)]
 */
#include "Disassembler.h"
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <bitset>
#include <tuple>
#include <fstream>
#include <iostream>

using namespace std;

int x_reg=0;    // x register
//default constructor
Disassembler::Disassembler(){}
/**
 *  Overloaded constructor
 *  - Converts mnemonic op hex address to binary.
 *  - Maps binary address key to mnemonic symbol as value.
 *  - Opens file to log assembly instructions during runtime.
 *  - Initializes class variables.
 *  @param: mnemonic op name array, mnemonic op address array
 */
Disassembler::Disassembler(const string mnemonics[],const string ops[]){
    // loop through mnemonic ops and hex of ops.
    for(int i = 0; i < 59; i++){
        int x = hex_to_int(ops[i]);
        // converts decimal to binary, makes call to bitset
        bitset<8> b(x);
        // storing key values pairs (key: op, value: mnemonic )
        mnemonic_op[b.to_string()] = mnemonics[i];
    }
    // opening file to log SIC/XE statements into
    ofs.open("out.lst", ofstream::out | ofstream::app);
    prog_name = "";
    // hard code registers to index of their value
    registers.push_back('A');registers.push_back('X');
    registers.push_back('L');registers.push_back('B');
    registers.push_back('S');registers.push_back('T');
    registers.push_back('F');
}
/**
 *  read_obj_file()
 *  - Opens file containing header, text and end records.
 *  - Stores all records into a vector of strings for later use.
 *  @param: filename
 *  @return: true if file opened, false otherwise.
 */
bool Disassembler::read_obj_file(string filename){
    ifstream file;  
    file.open(filename);
    string myString;
    // check if file was opened before attempting to read
    if(file.is_open()){
        // read data from file object and put it into string.
        while(getline(file, myString))
            object_code.push_back(myString);// push data onto vector
        file.close();   // close file after reading
        return 1;   // return true
    }
    // output error message if file was not found/opened
    else{
        cout<<"***ERROR***\nFile not found. Exitting program."<<endl;
        return 0;   //return false
    }
}
/**
 *  read_header_row()
 *  - Parses through header record to extract program name, start addr & length.
 *  - Makes log to output file with the first assembly instruction to start
 *    the program.
 *  - Initializes PC to starting address given.
 *  @param: none
 *  @return: none
 */
void Disassembler::read_header_row(){
    string str = object_code[0];
    int i = 1;  // starting index of program name
    // loop header row until we parse program name from string.
    while(str[i]){
        if(isalpha(str[i]))
            prog_name += str[i];
        else
            break;  //exit loop if we encounter a number
        i++;
    }
    start_address = str.substr(i,6);    // parse start address
    prog_length = str.substr(i+6, 6);   // parse prog_length
    // remove leading 0's from start address if any
    while(start_address.length() > 1){
        if(start_address.substr(0,1) == "0"){
            start_address.erase(start_address.begin());
        }
        else{
            break;
        }
    }
    string s = "0000\t" + prog_name + "\t" + "START\t" + start_address + "\t\n";
    // writing first line of assembly code to file
    ofs << s;
    // converting start address from hex string to int to store in pc
    pc = hex_to_int(start_address);
}
/**
 *  parse_literal()
 *  - Parses through rows from literal table to extract:name,lit_const,len,addr.
 *  - Create tuple (name,lit_const,length).
 *  - Maps address as key to tuple created as the value.
 *  @param: row from literal table containing [name,lit_const,length,address]
 *  @return: none
 */
void Disassembler::parse_literal(string littab_row){
    // vector that holds strings from littab row
    vector<string> v;
    // i: symtab row index, v_i: vector index
    int i = 0, v_i=0;
    // filling vector with empty strings for concatenating info
    for(int j = 0; j < 4; j++){
        v.push_back("");
    }
    while(i < littab_row.length()){
        // literal has no name
        if(i == 0 && littab_row[0] == ' '){
            v_i++;
        }
        // concatenate string if not a space
        if(littab_row[i] != ' '){
            v[v_i] += littab_row[i];
        }
        // increment vector index at the end of each string before space
        if(v[v_i] != "" && littab_row[i]==' '){
            v_i++;
        }
        i++;
    }
    // store name, lit_constant and length to tuple
    tuple<string,string,int> t (v[0],v[1],stoi(v[2]));
    // address is the key, tuple is the value
    littab[v[3]] = t;
}
/**
 *  parse_symbol()
 *  - Parses through rows from symbol table to extract symbol name and address.
 *  - Maps address as the key to the symbol as the value.
 *  @param: row from symbol table containing symbol and address.
 *  @return: none
 */
void Disassembler::parse_symbol(string symtab_row){
    vector<string> v;
    // i: symtab row index, v_i: vector index
    int i = 0, v_i = 0;
    // filling array with empty string to later concatenate symbol row
    v.push_back(""); v.push_back("");
    while(i < symtab_row.length() && v_i < v.size()){
        if(symtab_row[i] != ' '){
            v[v_i] += symtab_row[i];
        }
        // increment vector index at the end of each string before space
        if(v[v_i] != "" && symtab_row[i]==' '){
            v_i++;
        }
        i++;
    }
    // map address as key to symbol value
    symtab[v[1]] = v[0];
}
/**
 *  read_sym_file()
 *  - Opens .sym file to extract info from symtab and littab.
 *  - Parses file line by line to extract symbol info and literal info.
 *  - Makes function call to parse literal or parse symbol to store in hash map.
 *  @param: filename
 *  @return: true if file was opened, false otherwise.
 */
bool Disassembler::read_sym_file(string filename){
    ifstream file;  
    file.open(filename);
    string myString, key, val;
    vector<string> v;   // stores each line of the .sym file
    if(file.is_open()){
        // read data from file object and put it into string.
        while(getline(file, myString)){
            v.push_back(myString);
        }
        int sym_flag=0,lflag=0;
        // loop through rows of symbol file
        for(int i = 0; i < v.size(); i++){
            // check if row is greater than 1 character, else its a newline
            if(v[i].size() > 1){
                // check if parsed string is start of symbol table
                if(v[i].substr(0,6) == "Symbol"){
                    sym_flag = 1;   // flag for symbol table
                }
                // check if parse string is start of literal table
                else if(v[i].substr(0,4)=="Name"){
                    sym_flag = 0;
                    lflag = 1;  //flag for literal table
                }
                else{
                    // check symbol flag
                    if(sym_flag && v[i][0] != '-'){
                        myString = v[i];
                        parse_symbol(myString);
                    }
                    // check literal flag
                    if(lflag && v[i][0] != '-'){
                        myString = v[i];
                        parse_literal(myString);
                    }
                }
            }
        }
        file.close(); // close file after reading
        return 1; // return true
    }
    else{
        cout<<"***ERROR***\nFile not found. Exitting program."<<endl;
        return 0;   //return false
    }
}
/**
 *  read_text_record()
 *  - Loops through object_code vector which stores all Text records
 *  - Iterate through each text record and determine format for each object code
 *  - Once format determined, call write_to_output() to log assembly instruction
 *  - If no format, then determine byte instruction.
 *  - Handles case for RESB instruction and determine bytes needed using offset
 *    from current addresses and address of next instruction.
 *  @param: none
 *  @return: none
 */
void Disassembler::read_text_record(){
    //Looping through vector which holds all object code records. Only looping through text records.
    for(int i = 1; i < object_code.size()-1; i++){
        string str = object_code[i];    // text record string
        string obj_code,temp,const_type;    // helper vars
        map<string,string>::iterator it;    // symtab iterator
        // stores address within range of PC and start of next T record.
        vector<string> addr_in_range;
        int offset; // stores decimal value of offset in bits
        unsigned int curr_address; 
        int index = 9;  // index of first object code on any text record
        string mnemonic = "";   // mnemonic instruction string
        // loop through text record string
        while(index < str.length()-1){
            curr_address = pc;  // storing current address as decimal value
            //checking all format instructions in while loop & if curr_address is a literal addr.
            if(str.substr(0,1) == "M")
                break;
            //check if format 2 instruction.
            else if(is_format_2(str.substr(index,2)) && !is_in_littab(curr_address)){
                // return mnemonic instruction from symtab
                mnemonic = get_mnemonic_op(str.substr(index,2),2);
                // uppercase and lowercase alphabetical chars.
                temp = decimal_to_hex(curr_address);
                to_uppercase(temp);
                // add extra 0's to hex
                add_offset_bits(4, temp);   // 4 nibbles needed for printing memory loc
                char reg = registers[hex_to_int(str.substr(index+2,1))];
                if(mnemonic == "CLEAR"){
                    if(reg == 'X')
                        x_reg = 0;
                }
                ofs << temp +"\t\t" << mnemonic<<"\t"<<reg<<"\t\t"<<str.substr(index,4)<<endl;
                pc += 2;    //update pc counter
                index += 4; //update index to point to next object code
            }
            // check format 4 instruction
            else if(is_format_4(str.substr(index,8)) && !is_in_littab(curr_address)){
                mnemonic = get_mnemonic_op(str.substr(index, 2),4);
                //parsing object code from text record
                obj_code = str.substr(index,8);
                pc += 4;    //update pc counter
                index += 8; //update index to point to next object code
                //format parameters into SIX/XE source code to write to output file.
                write_to_output(curr_address,obj_code,mnemonic);
            }
            // else if format 3
            else if(is_format_3(str.substr(index,6)) && !is_in_littab(curr_address)){
                mnemonic = get_mnemonic_op(str.substr(index, 2),3);
                //parsing object code from text record
                obj_code = str.substr(index,6);
                pc += 3;    //update pc counter
                index += 6; //update index to point to next object code
                //format parameters into SIX/XE source code to write to output
                write_to_output(curr_address,obj_code,mnemonic);
            }
            else{
                // tuple returned from littab hashmap
                // auto[name, lit_const, length] = get_literal(curr_address);
                tuple<string,string,int> mytuple = get_literal(curr_address);
                string name = get<0>(mytuple);
                string lit_const = get<1>(mytuple);
                int length = get<2>(mytuple);
                // check first char in lit_const to see if it is a literal.
                if(lit_const[0] == '='){
                    const_type = '*';       // symbol for literal constant
                    int end_addr = hex_to_int(prog_length);
                    int temp = length/2;
                    ofs <<"\t\tLTORG\n";
                }
                // else, it is a byte constant
                else{
                    const_type = "BYTE";
                }
                // convert decimal address to hex
                temp = decimal_to_hex(curr_address);    //temp holder of hex_string
                // capitalize alphabetical chars in hex address
                to_uppercase(temp);
                // add extra 0's to hex
                add_offset_bits(4, temp);   // 4 nibbles needed for printing memory loc
                string temp_obj_code = str.substr(index,length);
                if(temp_obj_code.length() < 4)
                    temp_obj_code = '\t' + temp_obj_code;
                ofs <<temp<<'\t'<<name<<'\t'<<const_type<<"\t"<<lit_const<<'\t'<<temp_obj_code<<endl;
                pc += (length/2);   // update pc register 
                index += length;    // increment index to point to next obj_code
            }
        }//while
        //check if there is another record in the object code vector.
        if(i+1 <= object_code.size()-1){
            int dec1,dec2;  //decimal ints
            string next_start_addr; //starting address of next SIC/XE statement.
            vector<int> RESB;   //contains the reserved bytes for each symbol.
            //check if next record is text record
            if(object_code[i+1].substr(0,1) == "T")
                next_start_addr = object_code[i+1].substr(1,6);//start addr of next T record.
            else
                next_start_addr = prog_length;  //else, next address is end address.
            offset = hex_to_int(next_start_addr);   //next addr as int
            //iterate through symtab
            for(it = symtab.begin(); it != symtab.end(); it++){
                // convert hex string to decimal integer
                dec1 = hex_to_int(it->first);
                //if address of symbol in range between PC and next address. Add symbol to vector.
                if(dec1 >= pc && dec1 <= offset)
                    addr_in_range.push_back(it->first);
            }
            // if any symbols found within range of PC & next address
            if(addr_in_range.size()){
                //loop through symbols in range of PC & next address
                for(int j = 0; j < addr_in_range.size(); j++){
                    // converting hex string to decimal integer
                    dec1 = hex_to_int(addr_in_range[j]);
                    //check if element in next index
                    if(j+1 < addr_in_range.size()){
                        //convert next symbol address to decimal
                        dec2 = hex_to_int(addr_in_range[j+1]);
                        //push the offset from address into RESB.
                        RESB.push_back(dec2 - dec1);
                        pc += dec2-dec1;                //update pc
                    }
                    // reached last address in range,get offset w/ next start addr.
                    else{
                        RESB.push_back(offset-dec1);
                        pc += offset - dec1;
                    }
                }
                //loop through RESB vector and print lines
                for(int j = 0; j < RESB.size(); j++){
                    ofs <<addr_in_range[j].substr(2,4)<<"\t"<<symtab[addr_in_range[j]]<< "\tRESB\t" << RESB[j]<<'\n';
                }
            }
        }
    }//for
}
/**
 *  is_format_2()
 *  - Converts object code to binary to search mnemonic op table.
 *  @param: object code in hex
 *  @return: true if mnemonic op found, false otherwise
 */
bool Disassembler::is_format_2(string opcode){
    map<string,string>::iterator it;
    int x = hex_to_int(opcode);    //return decimal value of opcode
    bitset<8> b(x);     // converts decimal to binary, makes call to bitset
    it = mnemonic_op.find(b.to_string());
    if (it == mnemonic_op.end())
        return false;
    return true;
}
/**
 *  is_format_4()
 *  - Converts object code to binary to check flag bit e=1;
 *  @param: object code in hex
 *  @return: true if format 4, false otherwise
 */
bool Disassembler::is_format_4(string obj_code){
    if(obj_code.length() < 8)
        return false;
    unsigned int x = hex_to_int(obj_code); 
    bitset<32> b(x);     // converts decimal to binary, makes call to bitset
    obj_code = b.to_string();
    if (obj_code[11] == '1')
        return true;
    return false;
}
/**
 *  is_format_3()
 *  - Converts object code to binary to check flag bit e=0;
 *  @param: object code in hex
 *  @return: true if format 3, false otherwise
 */
bool Disassembler::is_format_3(string obj_code){
    if(obj_code.length() < 6)
        return false;
    unsigned int x = hex_to_int(obj_code); 
    bitset<24> b(x);     // converts decimal to binary, makes call to bitset
    obj_code = b.to_string();
    if (obj_code[11] == '0')
        return true;
    return false;
}
/**
 *  get_symbol()
 *  - Iterate through symtab to find symbol at a given address.
 *  - Int address converted to hex then formated for proper searching.
 *  @param: int memory address
 *  @return: symbol if found, empty string otherwise
 */
string Disassembler::get_symbol(unsigned int addr){
    map<string,string>::iterator it;    // iterate through sytab
    string symbol = decimal_to_hex(addr);
    to_uppercase(symbol);       //capitalize alphabetical chars in hex addr
    // adds 0s to front of hex addr until num bits needed is reached.
    add_offset_bits(6,symbol);      // 6 bits needed for searching symtab
    // search symtab for 
    it = symtab.find(symbol);
    if (it != symtab.end())
        return it->second;
    return "";
}
/**
 *  get_mnemonic_op()
 *  - Converts a byte of hex to binary to extract mnemonic operation.
 *  - Given instruction format, 2 extra bits are added to binary number.
 *  @param: byte from object code, instruction format
 *  @return: mnemonic op from hashtable.
 */
string Disassembler::get_mnemonic_op(string byte, int format){
    unsigned int x = hex_to_int(byte);  // hex string to binary
    bitset<8> b(x);                     // converts decimal to binary, makes call to bitset
    string temp = b.to_string();
    // setting final 2 bits to 0. Opcode = 6 bits + "00".
    if(format >= 3)
        temp[6] = '0'; temp[7] = '0';
    return mnemonic_op.find(temp)->second;
}
/**
 *  get_addressing_mode()
 *  - Given object code, it is converted from hex to binary.
 *  - Binary flag bits are stored into a vector as 0 or 1.
 *  @param: object code, vector to store flag bits
 *  @return: none
 * 
 */
void Disassembler::get_addressing_mode(string obj_code, vector<int> &vec){
    // number of bits in object code.
    unsigned int x = hex_to_int(obj_code);
    // binary result is 4 bytes (32 bits)
    bitset<32> b(x);    // converts decimal to binary, makes call to bitset
    string temp = b.to_string();
    // check if format 3 to remove leftmost byte.
    if(obj_code.length()/2 == 3)
        temp.erase(temp.begin(),temp.begin()+8);

    //storing flagbits into vector[n,i,x,b,p]
    for(int i = 6; i < 11; i++)
        if(temp[i] == '0')
            vec.push_back(0);
        else
            vec.push_back(1);
}
/**
 *  write_to_output():
 *  - Properly formats assembly instruction using object code.
 *  - Checks all addressing mode types and makes updates according to mode.
 *  - Assembly instruction formated then written to file.
 *  @param: current address, object code, mnemonic op
 *  @return: none 
 */
void Disassembler::write_to_output(unsigned int cur_addr,string obj_code,string mnemonic){
    // determine object code format using object code length
    string format = (obj_code.length() % 8 == 0 ? "+" : "");
    // excluding the first 3 nibbles in object code will leave disp/address.
    string operand_addr = obj_code.substr(3,obj_code.size()-3);
    string operand_mode = "";
    string operand; // name of operand
    string max_hex; //holds max hex value for given number of nibbles
    vector<int> vec;
    int index = 0;  // temp index var
    //checking flagbits to determine TA addressing mode.
    get_addressing_mode(obj_code, vec);
    // getting symbol from symtab.
    string symbol = get_symbol(cur_addr);
    // converting object code from hex to int
    unsigned int x = hex_to_int(operand_addr);  //displacement or address.
    // converting first hex num to binary to check sig bit
    unsigned int n = hex_to_int(operand_addr.substr(0,1));
    bitset<1> b(n);
    // checking most sig bit, then converting to 2's compliment if negative.
    if(b.to_string().substr(0,1) == "1"){
        max_hex="";
        //concatenating 'F's to empty string
        for(int i = 0; i < operand_addr.length();i++){
            max_hex += 'F';
        }
        // converting hex to integer
        n = hex_to_int(max_hex);
        // 2's compliment
        x = ~(n - x);
    }
    if(vec[2]){
        x += x_reg;
    }
    //check base relative addressing mode
    if(vec[3]){
        // adding base to operand address
        x += hex_to_int(base);
    }
    //check pc relative addressing mode
    if(vec[4]){
        // adding pc to operand address
        x += pc;
    }
    // b & p bits = 0 & not format 4 instruction
    if(!vec[3] && !vec[4] && format != "+"){
        operand = operand_addr;
        // erasing leading 0's from target address
        while(operand.length() > 1){
            if(operand.substr(0,1) == "0"){
                operand.erase(operand.begin());
            }
            else{
                break;
            }
        }
        // add tab if length is less than 4
        // if(operand.length() > 4)
        //     operand += '\t';
    }
    //getting operand from symtab.
    else
        operand = get_symbol(x);
    // check literal table if no operand found
    if(operand == ""){
        // retrive literal from target address
        tuple<string,string,int> mytuple = get_literal(x);
        string name = get<0>(mytuple);
        string lit_const = get<1>(mytuple);
        unsigned int length = get<2>(mytuple);
        // remove spaces from name. Literals have no name so they are all space.
        // check if literal name length is 0
        if(!name.length()){
            operand = lit_const;    // operand is literal
            literals.push_back(x);
        }
        else
            operand = name+"\t";         // else, operand is name
    }
    // converting current address from int, to hex string
    string curr_address_hex = decimal_to_hex(cur_addr);
    //capitalize alphabetical chars in hex address
    to_uppercase(curr_address_hex);
    // adds 0s to front of hex address number of bits needed is reached
    add_offset_bits(4, curr_address_hex);
    //determine TA addressing mode
    //if indirect addressing mode: (n=1, i=0)
    if(vec[0] && !vec[1])
        operand_mode = '@';
    //if immediate addressing mode: (n=0, i=1)
    if(!vec[0] && vec[1])
        operand_mode = '#';
    //check index addressing mode
    if(vec[2]){
        index = 0;
        // loop through string until space found.
        while(index >= 0 && operand.length()){
            if(operand[index] != ' '){
                index++;
            }
            else{
                operand = operand.substr(0,index);
                break;
            }
        }
        operand += ",X";
    }
    int offset = 10 - operand.length();
    while(offset > 0){
        operand += ' ';
        offset--;
    }
    // logging statements to output file
    ofs <<curr_address_hex<<"\t";
    if(symbol == "")
        ofs<<"\t";
    if(symbol.length() > 4)
        symbol = symbol + '\t';
    ofs << symbol<<format+mnemonic <<"\t";
    ofs << operand_mode+operand<<"\t"<<obj_code<<endl;
    // logging base statement to output file
    if(mnemonic == "LDB"){
        ofs <<"\t\tBASE\t"<< operand<<endl;
        base = operand_addr;
    }
    if(mnemonic == "LDX"){
        x_reg = hex_to_int(operand_addr);
    }
}
/**
 * get_literal():
 *  - Gets value from littab if given memory address is in the map.
 *  @param: decimal memory address
 *  @return: tuple containing literal information <name, lit_const, length> 
 */
tuple<string,string,unsigned int> Disassembler::get_literal(unsigned int addr){
    // converting decimal address to hex
    string temp = decimal_to_hex(addr);
    // uppercase and lowercase alphabetical chars.
    to_uppercase(temp);
    // adding 0's to front of hex address until bits needed is reached
    add_offset_bits(6,temp);        // 6 bits needed for searching symtab
    return littab[temp];
}
/**
 * is_in_littab():
 *  - Iterates through hash map to see if the given address is in littab.
 *  - Call to_uppercase() & add_offset_bits() to match searchin notation.
 *  @param: decimal memory address
 *  @return: true if found, false otherwise. 
 */
bool Disassembler::is_in_littab(unsigned int addr){
    // littab iterator
    map<string, tuple <string,string,unsigned int>>::iterator it;         
    string temp = decimal_to_hex(addr);
    //uppercase alphabetical chars only
    to_uppercase(temp);
    //adding extra 0's to hex address
    add_offset_bits(6,temp);        // 6 nibbles needed for searching littab
    it = littab.find(temp);
    if(it != littab.end())
        return true;
    return false;
}
/**
 * hex_to_int():
 *  - Uses stringstream object to convert hex string to decimal integer.
 *  @param: hex string
 *  @return: integer value  
 */
unsigned int Disassembler::hex_to_int(string hex_address){
    unsigned int x; 
    stringstream ss;    // stringstream object
    ss << hex << hex_address;    // reading hex into integer
    ss >> x;            // storing int value into x.
    return x;
}
/**
 * to_uppercase():
 *  - Statement notation and memory address use capital hex chars.
 *  - Only uppercase alphabetical chars in hex address
 *  @param: reference to string hex address
 *  @return: none 
 */
void Disassembler::to_uppercase(string &hex_addr){
    // make chars uppercase if they are lowercase alphabetical chars.
    for(int i = 0; i < hex_addr.length(); i++){
        if(hex_addr[i] >=97)
            hex_addr[i] -= 32;
    }
}
/**
 * add_offset_bits():
 *  - Statement notation and memory address require n number of bits.
 *  - Add extra bits to front of hex string to match notation.
 *  @param: number of bits needed, reference to string hex address
 *  @return: none 
 */
void Disassembler::add_offset_bits(int num_bits, string &hex_addr){
    int offset = num_bits - hex_addr.length();
    for (int i = 0; i < offset; i++)
        hex_addr = "0" + hex_addr;
}
/**
 * decimal_to_hex():
 *  - Uses stringstream object to convert decimal integer to hex string.
 *  @param: integer value
 *  @return: hex string  
 */
string Disassembler::decimal_to_hex(unsigned int decimal){
    stringstream ss;
    ss << hex << decimal;
    return ss.str();
}
/**
 * close_file():
 *  - Closes file used for logging SIC/XE statements
 *  @param: none
 *  @return: none  
 */
void Disassembler::close_file(){
    ofs.close();
}
/**
 * read_end_record():
 *  - Prints end statement to output file.
 *  @param: none
 *  @return: none  
 */
void Disassembler::read_end_record(){
    ofs << "\t\tEND\t"<<prog_name<<endl;
}