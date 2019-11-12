#ifndef __VARIABLE_LENGTH_INTEGER_H
#define __VARIABLE_LENGTH_INTEGER_H
#include <cstdint>
#include <stddef.h>
#include <vector>
#include <fstream>
#include <cstdlib>
using namespace std;

int getLength(ifstream& file) {
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    return length;
}

int encodedLen(int value) {
    int len = 1;
    while (value > 127) {
        value >>= 7;
        ++len;
    }
    return len;
}

template<typename int_t = uint64_t>
int encodeVB(int_t value, ofstream& outfile) {
    int len = 1;
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
template<typename int_t = uint64_t>
void decodeVB(ifstream& infile, int& start, int& len) {
    if (infile){
        int length = len;
        char* temp = new char[length];
        infile.seekg(start, infile.beg);
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
template<typename int_t = uint64_t>
void decodeVB_TermFromIndex(ifstream& infile, unsigned int& start, int& len, vector<int>& termData) {
    if (infile){
        int length = len;
        char* temp = new char[length];
        infile.seekg(start, infile.beg);
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
//                cout << ret << endl;
                termData.push_back(ret);
                ret = 0, j = 0;
            }
        }
    }
}
#endif //__VARIABLE_LENGTH_INTEGER_H
