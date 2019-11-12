//
//  helpers.h
//  InvertedIndex
//
//  Created by Alexander Gao on 10/18/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//


#ifndef helpers_h
#define helpers_h

#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

void read_dir(string cc_path, vector<string>& files) {
    // Get names of files in directory
    DIR* cc_dir = opendir(cc_path.c_str());
    struct dirent * dp;
    while ((dp = readdir(cc_dir)) != NULL) {
        files.push_back(dp->d_name);
    }
    closedir(cc_dir);
}

bool isValidFile1(const string& s) {
    // exclude hidden files, files other than .gz files
    cout << "CHECKING IF FILE NAME VALID..." << endl;
    if (s[0] == '.') return false;
    if (s[ s.size() - 1 ] != 'z') return false;
    return true;
}

bool isValidFile2(string& s) {
    if (s[0] == '.') return false;
    return true;
}

bool isValidFile3(string& s) {
    // exclude hidden files, files other than .gz files
    if (s[0] == '.') return false;
    if (s[s.size() - 1] != 'n') return false;
    return true;
}

int toInt(const string& num) {
    int n = num.size();
    int res = 0;
    for (int i = 0; i < n; ++i) {
        res *= 10;
        res += num[i] - '0';
    }
    return res;
}

bool isAlphaNum(const char& c) {
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    return false;
}

bool exclude(string s) {
    if (s == "www" ||
        s == "WWW" ||
        s == "com" ||
        s == "COM" ||
        s == "a" ||
        s == "an" ||
        s == "and" ||
        s == "are" ||
        s == "as" ||
        s == "at" ||
        s == "be" ||
        s == "by" ||
        s == "for" ||
        s == "from" ||
        s == "has" ||
        s == "he" ||
        s == "in" ||
        s == "is" ||
        s == "it" ||
        s == "its" ||
        s == "of" ||
        s == "on" ||
        s == "that" ||
        s == "the" ||
        s == "to" ||
        s == "was" ||
        s == "were" ||
        s == "will" ||
        s == "with" ) return true;
    if (s.size() > 30) return true;
    return false;
}

bool isEndOfPageHeader(const char* buf, const int& i) {
    if (buf[i] == 13 && buf[i+1] == 10 &&
        buf[i+2] == 13 && buf[i+3] == 10 &&
        buf[i+4] == 13 && buf[i+5] == 10 &&
        buf[i+6] == 13 && buf[i+7] == 10) return true;
    return false;
}

bool isNewPage(const char* buf, const int& i) {
    if (buf[i] == 10 &&
        buf[i+1] == 13 && buf[i+2] == 10 &&
        buf[i+3] == 13 && buf[i+4] == 10) return true;
    return false;
}

bool isEndOfHeader(const char* buf, const int& i) {
    if (buf[i] == 13 && buf[i+1] == 10 &&
        buf[i+2] == 13 && buf[i+3] == 10) return true;
    return false;
}

void intToBinary(ofstream& outFile, int i) {
    vector<char> digits;
    while (i) {
        digits.push_back((char)(i%10) + '0');
        i /= 10;
    }
    reverse(digits.begin(), digits.end());
    for (int j = 0; j < digits.size(); ++j) {
        outFile << digits[j];
    }
}

string numFormat(int i) {
    // Formats an integer into a three-digit number
    // Assumption: the input int will not be greater than 3 digits (>999)
    string numOut;
    vector<char> digits;
    while (i) {
        digits.push_back((char)(i%10) + '0');
        i /= 10;
    }
    while (digits.size() < 3)
        digits.push_back('0');
    reverse(digits.begin(), digits.end());
    for (int j = 0; j < digits.size(); ++j)
        numOut += digits[j];
    return numOut;
}

#endif /* helpers_h */
