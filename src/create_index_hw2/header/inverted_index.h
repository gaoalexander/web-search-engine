//
//  inverted_index.h
//  InvertedIndex
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#ifndef inverted_index_h
#define inverted_index_h

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <varbyte.h>
#include <helpers.h>
#include <indexing_io.h>
#include <time.h>
using namespace std;

//--------------------------------------------------------INDEXING FUNCTIONS---------------------------------------------------------

void processPageHeader(const int& docID, const string& page_header, unordered_map<int, string>& page_table, unordered_map<int, int>& page_len, int& doc_len) {
    int n = page_header.size();
    string url;
    for (int i = 0; i + 4 < n; ++i) {
        if (page_header[i] == 'U' && page_header[i+1] == 'R' && page_header[i+2] == 'I' && page_header[i+3] == ':') {
            i += 5;
            while (page_header[i] != 13 && page_header[i] != 10) {
                url += page_header[i];
                ++i;
            }
            break;
        }
    }
    page_table[docID] = url;
    page_len[docID] = doc_len;
    doc_len = 0;
}

void createPostings(const string& fpath, const string& outFilename,\
                    const string& intermediate_path, const string& docs_dir, int& docID,\
                    int& termID, unordered_map<string, int>& term_to_index, unordered_map<int, string>& page_table, unordered_map<int, int>& page_len) {
    int doc_len;
    string doc_cur;
    ifstream file;
    file.open(fpath, ios::in);
    
    if (file) {
        // get length of file:
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);
        char * buf = new char [length];
        std::cout << "Reading " << length << " characters... " << endl;
        
        // read data as a block:
        file.read (buf, length);
        int pages = 1;
        string term;
        int pageHeaderFlag = 1;
        set<string> terms;
        
        // Skip WET file header
        int start = 0;
        while(!isEndOfPageHeader(buf, start)) ++start;
        start += 8;
        string page_header;
        
        unordered_map<string, int> count;
        
        // Read all contents of a single file, generating postings to be written out
        ofstream postOut(intermediate_path + outFilename, ios::out | ios::binary);
        
        for (int i = start; i + 4 < length; ++i) {
            if (isNewPage(buf, i)) {
//                string doc_out_path = docs_dir + docID + ".bin";
                ofstream docOut(docs_dir + to_string(docID) + ".bin");
                docOut << doc_cur;
                doc_cur = "";
                docOut.close();
                // Update index-to-term lookup dictionary
                for (pair<string, int> each : count) {
//                    if (docID == 26483 && (each.first == "hello" || each.first == "zzzzzz")) cout << endl << "VERIFY THIS WORKS!!!" << endl;
                    if (!exclude(each.first)) {
                        if (!term_to_index.count(each.first)) {
                            term_to_index[each.first] = termID;
    //                        cout << "term: " << each.first << "  assigned termID: " << termID << endl;
                            ++termID;
                        }
                        // Write out current document's terms to intermediate postings file
                        postOut << each.first << ' ';
                        postOut << docID << ' ';
                        postOut <<each.second << '\n';
                    }
                }
                processPageHeader(docID, page_header, page_table, page_len, doc_len);
                page_header = "";
                
                // Clear count dictionary for next document
                count.clear();
                ++pages, ++docID;
                i += 4;
                pageHeaderFlag = 1;
            } else if (isEndOfHeader(buf, i)) {
                i += 3;
                pageHeaderFlag = 0;
            } else if (!isAlphaNum(buf[i])) {
                if (pageHeaderFlag)
                    page_header += buf[i];
                if (term != "" && !exclude(term) && term.size() > 1 && term.size() < 20) {
                    transform(term.begin(), term.end(), term.begin(), ::tolower);     // convert each term to lower case
                    count[term] += 1;
                    ++doc_len;
                }
                term = "";
            } else if (!pageHeaderFlag) {
                term += buf[i];
            } else {
                // Isolate page header text
                page_header += buf[i];
            }
            doc_cur += buf[i];
        }
        cout << "NUMBER OF PAGES: " << pages << endl;
        cout << "NUMBER OF TERMS (ACCUM): " << termID << endl;
        
        if (file)
            cout << "all characters read successfully." << endl << endl;
        else
            cout << "error: only " << file.gcount() << " could be read" << endl;
        file.close();
        
        // ...buf contains the entire file...
        delete[] buf;
    }
}

//--------------------------------------------------------MAIN PROGRAM---------------------------------------------------------

void index(const string& data_dir) {
    
    time_t my_time = time(NULL);
    printf("START TIME: %s", ctime(&my_time));
    
    // I/O PATHS
//    string cc_path =            data_dir + "common_crawl_data/";
//    string intermediate_path =  data_dir + "intermediate/";
//    string term_dict_path =     data_dir + "index/term_dict.bin";
//    string page_table_path =    data_dir + "index/page_table.bin";

    string cc_path =            data_dir + "common_crawl_data/";
    string intermediate_path =  data_dir + "intermediate/";
    string term_dict_path =     data_dir + "index/term_dict.bin";
    string page_table_path =    data_dir + "index/page_table.bin";
    string page_len_path =      data_dir + "index/page_len.bin";
    string docs_dir =           data_dir + "documents/";

    
    // DECLARE VARIABLES
    int postingFileNum = 0;
    int docID = 1, termID = 0;
    vector<string> files;
    unordered_map<string, int> term_to_index;
    unordered_map<int, string> page_table;
    unordered_map<int, int> page_len;
    
    /**
     FOR EACH FILE:
     - Check whether it is a valid file.
     - Generate INTERMEDIATE POSTINGS
     - Add terms to <TERM - TERM_ID> DICTIONARY
     - Add pages to PAGE TABLE
     [Intermediate postings are written to disk in ASCII format]
     **/
    read_dir(cc_path, files);
    for (string s : files) {
        string fpath = cc_path + s;
        string out_file_name = "postings_" + numFormat(postingFileNum) + ".bin";
        cout << s << endl;
        if (isValidFile2(s)) {
            cout << fpath << endl;
            cout << "Processing file #: " << numFormat(postingFileNum++) << endl;
            createPostings(fpath, out_file_name, intermediate_path, docs_dir, docID, termID, term_to_index, page_table, page_len);
        }
    }
    
    writeOutTermDict(term_to_index, term_dict_path);
    writeOutPageTable(page_table, page_table_path);
    writeOutPageLen(page_len, page_len_path);
    
    my_time = time(NULL);
    printf("\n\nEND TIME: %s", ctime(&my_time));
}



#endif /* inverted_index_h */
