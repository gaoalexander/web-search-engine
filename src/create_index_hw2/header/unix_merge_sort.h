//
//  unix_merge_sort.h
//  unix_merge_sort
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#ifndef unix_merge_sort_h
#define unix_merge_sort_h

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <helpers.h>
using namespace std;

void unix_sort(const string& data_dir) {
    // I/O PATHS
//    string intermediates_path = data_dir + "intermediate/";
//    string temp_dir =           data_dir + "index/temp/";
//    string outpath =            data_dir + "index/inv_index.bin";
    
    string intermediates_path = data_dir + "intermediate/";
    string temp_dir =           data_dir + "index/temp/";
    string outpath =            data_dir + "index/inv_index.bin";
    
    vector<string> files;
    vector<string> cmds;
    string cmd_str = string("sort ") + string("-m ") +\
    string("-o ") + outpath + " " +\
    string("-k1,1 -k2,2n ") +\
    string("-T ") + temp_dir;
    
    // Create a list of sort commands that will be run on the shell
    read_dir(intermediates_path, files);
    for (string fname : files) {
        string fpath = string(intermediates_path) + string(fname);
        if (isValidFile3(fname)) {
            string cmd_sort_each = ("sort " + string("-o ") + fpath + " " + string("-k1,1 ") + string("-k2,2n ") + fpath);
            cmds.push_back(cmd_sort_each);
            cmd_str += " " + string(fpath);
        }
    }
    cmds.push_back(cmd_str);
    
    // Run sort commands on shell
    for (string cmd : cmds) {
        cout << "EXECUTING UNIX SORT COMMAND: " << endl << "$ " << cmd << endl << endl;
        system(cmd.c_str());
    }
}


#endif /* unix_merge_sort_h */
