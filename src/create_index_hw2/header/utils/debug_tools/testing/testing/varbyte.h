/**
 * C++ utilities for variable-length integer encoding
 * Compile with -std=c++11 or higher
 *
 * Version 1.1: Use size_t for size argument, improve function signatures
 *
 * License: CC0 1.0 Universal
 * Originally published on http://techoverflow.net
 * Copyright (c) 2015 Uli Koehler
 */

#ifndef __VARIABLE_LENGTH_INTEGER_H
#define __VARIABLE_LENGTH_INTEGER_H
#include <cstdint>
#include <stddef.h>
#include <vector>
#include <fstream>
#include <cstdlib>
using namespace std;

int getLength(ifstream& file) {
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

template<typename int_t = uint64_t>
int encodeVB(int_t value, ofstream& outfile) {
    int len = 0;
    while (value > 127) {
        outfile << (char)((value & 127) | 128);
        value >>= 7;
        ++len;
    }
    outfile << (char)(value & 127);
    return len;
}

template<typename int_t = uint64_t>
void decodeVB(ifstream& infile) {
    if (infile){
        int length = getLength(infile);
        char* temp = new char[length];
        infile.read(temp, length);
        vector<uint8_t> buf(length, NULL);
        for (int i = 0; i < length; ++i)
            buf[i] = temp[i];
        
        int ret = 0, j = 0;
        int n = buf.size();
        for (size_t i = 0; i < n; ++i) {
            ret |= (buf[i] & 127) << (7 * j);
            ++j;
            if(!(buf[i] & 128)) {
                cout << ret << endl;
                ret = 0, j = 0;
            }
        }
    }
}

#endif //__VARIABLE_LENGTH_INTEGER_H
