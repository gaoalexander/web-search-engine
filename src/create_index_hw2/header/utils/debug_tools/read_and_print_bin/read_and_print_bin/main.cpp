//
//  main.cpp
//  read_and_print_bin
//
//  Created by Alexander Gao on 10/16/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#include <string>
#include <iostream>
#include <fstream>
#include <varbyte.h>
#include <sstream>
#include <map>
using namespace std;

void readAndPrint(string filepath) {
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        
        // get length of file:
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);
        char* buf = new char[length];
        std::cout << "Reading " << length << " characters... " << endl;
        
        // read data as a block:
        file.read (buf, length);
        for (int i = 0; i < length; ++i) {
            cout << buf[i];
        }
        if (file)
            cout << "all characters read successfully.";
        else
            cout << "error: only " << file.gcount() << " could be read";
        file.close();
        // ...buf contains the entire file...
        delete[] buf;
    }
}

void readAndPrintTermDict(string filepath, map<string, int>& term_to_index) {
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        string line;
        string term;
        int termID;
        int count = 0;
        
        while (getline(file, line))
        {
            istringstream iss(line);
            iss >> term >> termID;
            term_to_index[term] = termID;
            cout << count << ' ' << term << ' ' << termID << endl;
            ++count;
        }
        file.close();
    }
}

void readAndPrintDict(string filepath, map<int, string>& page_table) {
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        string line;
        string url;
        int pageID;
        int count = 0;
        
        while (getline(file, line))
        {
            istringstream iss(line);
            iss >> pageID >> url;
            page_table[pageID] = url;
            ++count;
            cout << pageID << ' ' << url << endl;

        }
        file.close();
    }
}

int main() {
//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/InvertedIndex/data/intermediate/postings_000.bin";
//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/index/lexicon.bin";
//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/InvertedIndex/data/common_crawl_data/wet/CC-MAIN-20190915072355-20190915094355-00379.warc.wet";
//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/test_intermediate/postings_000.bin";
    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/test_index/inv_index.bin";
    readAndPrint(testpath);

//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/InvertedIndex/data/index/compact_inv_index.bin";
//
//    string testpath = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/index/lexicon.bin";
//    map<string, int> term_to_index;
//    readAndPrintTermDict(testpath, term_to_index);

//    ifstream infile(testpath);
//    decodeVB(infile);
//
//    string dict_path = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/index/term_dict.bin";
//    map<string, int> term_to_index;
//    readAndPrintTermDict(dict_path, term_to_index);
//
//    string dict_path = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/index/page_table.bin";
//    map<int, string> page_table;
//    readAndPrintDict(dict_path, page_table);

//    map<string, int> term_to_index;
    
    return 0;
}
