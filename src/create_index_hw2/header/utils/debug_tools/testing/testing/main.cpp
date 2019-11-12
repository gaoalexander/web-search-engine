//
//  main.cpp
//  testing
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//


#include <iostream>
#include <string>
#include <varbyte.h>
#include <stddef.h>
#include <vector>
#include <fstream>
using namespace std;

int main(int argc, char** argv) {
    
    string fpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/testing/data/test.bin";
    
    ofstream outfile(fpath);
    // each command writes a value to outfile:
    encodeVB<uint64_t>(0xCAFE, outfile);
    encodeVB<uint64_t>(0xCAFE, outfile);
    encodeVB<uint64_t>(0xCAFF, outfile);
    encodeVB<uint64_t>(1, outfile);
    encodeVB<uint64_t>(12000, outfile);
    encodeVB<uint64_t>(1200000, outfile);
    encodeVB<uint64_t>(8, outfile);
    encodeVB<uint64_t>('a', outfile);

    outfile.close();
    
    // read a binary file, decode varbyte-compressed binary data, display in stdout
    ifstream infile(fpath, ios::in | ios::binary);
    decodeVB(infile);
    infile.close();
}
