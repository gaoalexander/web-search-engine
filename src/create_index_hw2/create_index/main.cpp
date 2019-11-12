//
//  main.cpp
//  0_master
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#include <iostream>
#include <vector>
#include <gunzip.h>
#include <inverted_index.h>
#include <unix_merge_sort.h>
#include <compress_index.h>
using namespace std;

void download_pages(const string& proj_dir, int& n_pages) {
    // Download Common Crawl files from Amazon S3 bucket
    vector<string> cmds;
    string cmd = "cd " + proj_dir + '\n' +\
                 "python 1_download_pages.py " + to_string(n_pages);
    system(cmd.c_str());
    cout << "EXECUTION SHELL COMMAND: " << endl << cmd << endl << endl;
}

int main() {
    // I/O PATHS
    string proj_dir = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/";
    string data_dir = proj_dir + "_data/";
    string cc_path = data_dir + "common_crawl_data/";
    int download_n_pages = 2;

    // Main processes for building Inverted Index
//    download_pages(proj_dir, download_n_pages);
//    gunzip(cc_path);
//    index(data_dir);
//    unix_sort(data_dir);
//    compress_index(data_dir);
    
    return 0;
}


