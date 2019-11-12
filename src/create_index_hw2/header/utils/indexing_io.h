//
//  indexing_io.h
//  read_and_print_bin
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#ifndef indexing_io_h
#define indexing_io_h

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdlib>
#include <sstream>
using namespace std;

void loadLexicon(string& filepath, unordered_map<int, pair<unsigned int, int>>& lexicon) {
    cout << "Loading Lexicon..." << endl;
    ifstream file(filepath, ios::in | ios::binary);
    unsigned int accum_start = 0;
    if (file) {
        string line;
        int termID, start, len;
        while (getline(file, line)) {
            istringstream iss(line);
            iss >> termID >> start >> len;
            lexicon[termID] = {accum_start, len};
            accum_start += len;
        }
        cout << "Lexicon loaded." << endl << endl;
        file.close();
    }
}

void loadTermDict(string& filepath, unordered_map<string, int>& term_to_index) {
    cout << "Loading Term-To-Index dictionary..." << endl;
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        string line, term;
        int termID;
        while (getline(file, line)) {
            istringstream iss(line);
            iss >> term >> termID;
            term_to_index[term] = termID;
        }
        cout << "<Term - TermID> dictionary loaded." << endl << endl;
        file.close();
    }
}

void loadPageTable(string& filepath, unordered_map<int, string>& page_table) {
    cout << "Loading Page Table..." << endl;
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        string line, url;
        int docID;
        while (getline(file, line)) {
            istringstream iss(line);
            iss >> docID >> url;
            page_table[docID] = url;
        }
        cout << "Page Table loaded." << endl << endl;
        file.close();
    }
}
int loadPageLen(string& filepath, unordered_map<int, int>& page_len) {
    cout << "Loading Page Lengths..." << endl;
    int total_doc_lens = 0;
    ifstream file(filepath, ios::in | ios::binary);
    if (file) {
        string line;
        int docID, doc_len;
        while (getline(file, line)) {
            istringstream iss(line);
            iss >> docID >> doc_len;
            page_len[docID] = doc_len;
            total_doc_lens += doc_len;
        }
        cout << "Page Lengths loaded." << endl << endl;
        file.close();
    }
    return total_doc_lens;
}
void writeOutTermDict(unordered_map<string, int>& dict, string& path) {
    ofstream outfile(path);
    for (pair<string, int> each : dict) {
        outfile << each.first;
        outfile << ' ';
        outfile << each.second;
        outfile << '\n';
    }
}

void writeOutPageTable(unordered_map<int, string>& dict, string& path) {
    ofstream outfile(path);
    for (pair<int, string> each : dict) {
        outfile << each.first;
        outfile << ' ';
        outfile << each.second;
        outfile << '\n';
    }
}
void writeOutPageLen(unordered_map<int, int>& dict, string& path) {
    ofstream outfile(path);
    for (pair<int, int> each : dict) {
        outfile << each.first;
        outfile << ' ';
        outfile << each.second;
        outfile << '\n';
    }
}

#endif /* indexing_io_h */
