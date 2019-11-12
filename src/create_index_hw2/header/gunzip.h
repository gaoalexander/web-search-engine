//
//  gunzip.h
//  gunzip
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#ifndef gunzip_h
#define gunzip_h

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <helpers.h>
using namespace std;


void gunzip(string& cc_path) {
    vector<string> files;
    read_dir(cc_path, files);
    
    // For each .gz file in specified directory, run sh command <gunzip + filename>
    for (string s : files) {
        string fpath = cc_path + s;
        if (isValidFile1(s)) {
            cout << "GUNZIPPING: " << '"' << fpath << '"' << endl;
            string cmd_str = "gunzip " + fpath;
            const char* cmd = cmd_str.c_str();
            system(cmd);
        }
    }
}

#endif /* gunzip_h */
