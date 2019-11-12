//
//  compress_index.h
//  CompressIndex
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#ifndef compress_index_h
#define compress_index_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <varbyte.h>
#include <unordered_map>
#include <indexing_io.h>
#include <helpers.h>
using namespace std;

//================================================================================================================
void readPosting(string& line, unordered_map<string, int>& term_to_index, string& term, int& termID, int& docID, int& freq) {
    istringstream iss(line);
    iss >> term >> docID >> freq;
    termID = term_to_index[term];
//    cout << "term: " << term << "  termID: " << termID << "  docID: " << docID << "  freq: " << freq << endl;
}
void writeOutList(const vector<int>& list, ofstream& outfile) {
    for (int i = 0; i < list.size(); ++i) {
        encodeVB<uint64_t>(list[i], outfile);
    }
}
void writeSingleOut(int data, ofstream& outfile) {
    encodeVB<uint64_t>(data, outfile);
}
void writeTermToLexicon(ofstream& lexicon_out, const int& termID, const int& start, const int& len) {
    lexicon_out << termID << ' ';
    lexicon_out << start << ' ';
    lexicon_out << len << '\n';
//    writeSingleOut(termID, lexicon_out);
//    writeSingleOut(start, lexicon_out);
//    writeSingleOut(len, lexicon_out);
}
void writeTermToIndex(ofstream& index_out, const int& termID, const vector<int>& docIDs, const vector<int>& freqs) {
//    writeSingleOut(termID, index_out);
    writeOutList(docIDs, index_out);
    writeOutList(freqs, index_out);
}
void writePrevTerm(ofstream& index_out, ofstream& lexicon_out, int& prevTermID, int& start, int& len, vector<int>& docIDs, vector<int>& freqs) {
    writeTermToLexicon(lexicon_out, prevTermID, start, len);
    writeTermToIndex(index_out, prevTermID, docIDs, freqs);
}
void bufNextTerm(int& start, int& len, const int& termID, vector<int>& docIDs, vector<int>& freqs) {
    start += len;
    len = 0;
//    len = encodedLen(termID);
    docIDs.clear(), freqs.clear();
}

//================================================================================================================
void compressIndex(ifstream& index_in, ofstream& index_out, ofstream& lexicon_out, unordered_map<string, int>& term_to_index) {
    string line, term;
    int prevTermID = 0, start = 0, len = 0;
    int termID, docID, freq;
    vector<int> docIDs, freqs;
    int firstIter = 1;
    
    while (getline(index_in, line)) {
        readPosting(line, term_to_index, term, termID, docID, freq);
        
        if (firstIter) {
//            len = encodedLen(termID);
            firstIter = 0;
        } else if (!(prevTermID == termID)) {
            writePrevTerm(index_out, lexicon_out, prevTermID, start, len, docIDs, freqs);
            bufNextTerm(start, len, termID, docIDs, freqs); } else {}

        docIDs.push_back(docID), freqs.push_back(freq);
        len += encodedLen(docID) + encodedLen(freq);
        prevTermID = termID;
    }
    writePrevTerm(index_out, lexicon_out, prevTermID, start, len, docIDs, freqs);
}
//================================================================================================================
void compress_index(const string& data_dir) {
    cout << endl << endl << "CONDENSING & COMPRESSING INVERTED INDEX." << endl;
    
    string index_dir = data_dir + "index/";
    string index_in_path =  index_dir + "inv_index.bin";
    string index_out_path = index_dir + "compact_inv_index.bin";
    string lexicon_path =   index_dir + "lexicon.bin";
    string term_dict_path = index_dir + "term_dict.bin";
    
//        string index_in_path = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW3/test_data/test2.bin";
//        string lexicon_path = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW3/test_data/test2_lexicon.bin";
//        string index_out_path = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW3/test_data/test2_compressedIndex.bin";
    
    ifstream index_in(index_in_path);
    ofstream index_out(index_out_path);
    ofstream lexicon_out(lexicon_path);
    
    unordered_map<string, int> term_to_index;
    loadTermDict(term_dict_path, term_to_index);
    
    compressIndex(index_in, index_out, lexicon_out, term_to_index);
    
    index_out.close(), lexicon_out.close(), index_in.close();
    cout << "Finished compressing Index and creating Lexicon." << endl;
}

#endif /* compress_index_h */
