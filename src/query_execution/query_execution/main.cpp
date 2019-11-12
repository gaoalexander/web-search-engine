//
//  main.cpp
//  query_execution
//
//  Created by Alexander Gao on 10/30/19.
//  Copyright © 2019 Alexander Gao. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <math.h>
#include <unordered_map>
#include <indexing_io.h>
#include <varbyte.h>
#include <helpers.h>
#include <streambuf>
#include <algorithm>
#include <cstdlib>
#include <set>
using namespace std;

//===============================================================================================
//GLOBAL VARIABLE AND D.I.M.D.S. DECLARATIONS
//===============================================================================================
int total_len_docs;
unordered_map<string, int> term_to_index;
unordered_map<int, pair<unsigned int, int>> lexicon;
unordered_map<int, string> page_table;
unordered_map<int, int> page_len;
unordered_map<int, pair<vector<int>, vector<string>>> bm25_stats;
unordered_map<int, float> bm25_scores;
vector<vector<int>> termLists;
string data_dir, index_dir, docs_dir, compact_index_path, term_dict_path, lexicon_path, page_table_path, page_len_path;

//===============================================================================================
//INITIALIZATION
//===============================================================================================
void declareDataPaths() {
    data_dir = "/Users/GAO/Documents/NYUClasses_Fall2019/SearchEngines/HW2/_data/";  //DECLARE PATHS OF REQUIRED DIRECTORIES
    index_dir = data_dir + "index/";
    docs_dir =  data_dir + "documents/";
    compact_index_path = index_dir + "compact_inv_index.bin";
    term_dict_path =     index_dir + "term_dict.bin";
    lexicon_path =       index_dir + "lexicon.bin";
    page_table_path =    index_dir + "page_table.bin";
    page_len_path =      index_dir + "page_len.bin";
}

void loadAuxDataStructs(string& term_dict_path, string& lexicon_path, string& page_table_path, string& page_len_path, int& total_len_docs) {
    declareDataPaths();
    loadTermDict(term_dict_path, term_to_index);
    loadLexicon(lexicon_path, lexicon);
    loadPageTable(page_table_path, page_table);
    total_len_docs = loadPageLen(page_len_path, page_len);
}
//===============================================================================================
//UTILITY
//===============================================================================================
void to_lower(string& s){
    transform(s.begin(), s.end(), s.begin(), ::tolower);
}

int seekToEndOfHeader(string& s) {
    int num_newlines = 0;
    int i = 0;
    while (num_newlines < 10) {
        if (s[i] == '\n') ++num_newlines;
        ++i;
    }
    return i;
}

void print_bm25_stats(int& docID) {
    cout << "PAGE BM25 SCORE: " << bm25_scores[docID] << endl;
    cout << "(GLOBAL) Total # Docs: " << bm25_stats[docID].first[0] << endl;
    cout << "(GLOBAL) Average Doc Length: " << bm25_stats[docID].first[1] << endl;
    cout << "Length of Current Doc: " << bm25_stats[docID].first[2] << endl;
    for (int i = 0; i < bm25_stats[docID].second.size(); ++i){
        cout << "\t|TERM: " << bm25_stats[docID].second[i] << endl;
        cout << "\t\t|Term Frequency in Doc: " << bm25_stats[docID].first[i*2 + 3] << endl;
        cout << "\t\t|Total # Docs Containing Term: " << bm25_stats[docID].first[i*2 + 4] << endl;
    }
}

void printResult(int& docID, int& i, string& snippet) {
    cout << "RESULT #" << i << endl;
    cout << "Document ID: " << docID << endl;
    cout << "URL: " << page_table[docID] << endl << endl;
    print_bm25_stats(docID);
    cout << endl << endl << "..." << snippet << "..." << endl << endl;
    cout << "============================================================================="<< endl << endl;
}

//===============================================================================================
//QUERY PROCESSING
//===============================================================================================
vector<string> parseTerms(const string& query) {            //RETURNS: query as a list of terms
    string term;
    set<string> query_terms_set;
    vector<string> query_terms;
    for (char c : query) {
        if (c != ' ' && !isAlphaNum(c))
            continue;
        else if (c == ' ') {
            to_lower(term);
            if (term != "" && term_to_index.count(term)) query_terms_set.insert(term);
            term = ""; }
        else
            term += c;
    }
    to_lower(term);
    if (term != "" && term_to_index.count(term)) query_terms_set.insert(term);
    for (string s : query_terms_set) query_terms.push_back(s);
    return query_terms;
}

vector<string> getUserQuery() {
    int termID, start, len;
    string query;
    vector<string> query_terms = {};
    
    cout << "\n\nENTER YOUR QUERY: \n";
    getline(cin, query);
    cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n";
    cout << "\nACTUAL EXECUTED QUERY IS: \n";
    return parseTerms(query);
}
//===============================================================================================
//QUERY EXECUTION
//===============================================================================================
void openList(const string& t, ifstream& infile) {
    int termID, len;
    unsigned int start;
    vector<int> termData;
    cout << t << endl;
    termID = term_to_index[t];
    start = lexicon[termID].first, len = lexicon[termID].second;
    decodeVB_TermFromIndex(infile, start, len, termData);
    termLists.push_back(termData);
}

void closeLists(vector<vector<int>>& termLists) {
    for (vector<int> v : termLists) {
        vector<int>().swap(v);
        v.clear();
    }
    vector<vector<int>>().swap(termLists);          //clear previous term lists that were loaded into memory
    termLists.clear();
}

int nextGEQ(const vector<int>& termList, const int& docID, vector<int>& lp, const int& i) {
    int n = termList.size()/2;
    while (lp[i] < n && termList[lp[i]] < docID)
        lp[i] += 1;
    if (lp[i] < n) return termList[lp[i]];
    else return -1;
}

int getFreq(const vector<int>& termList, vector<int>& lp, const int& i) {
    return termList[termList.size()/2 + lp[i]];
}

int getMaxDocID() {
    int max_docID = 0;
    for (vector<int> list : termLists) max_docID = max(max_docID, list[list.size()/2 - 1]);
    return max_docID;
}

float bm25(int& docID, const int& n_terms, const vector<int>& freq, const vector<int>& docs_term, vector<int>& page_bm25_stats) {
    int total_docs = page_table.size();
    float avg_doc_len = (float)total_len_docs/(float)total_docs;
    int len_d = page_len[docID];
    float score = 0.0;
    float k1 = 1.2, b = 0.75;
    float K = k1 * ((1-b) + b * ((float)len_d / avg_doc_len));
    page_bm25_stats.push_back(total_docs);
    page_bm25_stats.push_back(avg_doc_len);
    page_bm25_stats.push_back(len_d);
    for (int i = 0; i < n_terms; ++i) {
        score += log(((float)total_docs - (float)docs_term[i] + 0.5)/((float)docs_term[i] + 0.5)) * (((k1 + 1) * (float)freq[i]) / (K + (float)freq[i]));
        page_bm25_stats.push_back(freq[i]);
        page_bm25_stats.push_back(docs_term[i]);
    }
    return score;
}

float rateSnippet(int& start, int& snippet_len, vector<vector<int>>& term_locs, int& docID) {
    float rating = 0.0;
    vector<int> num_each_term;
    for (vector<int> term_loc : term_locs) {
        int count = 0;
        for (int i : term_loc) {
            if (i >= start && i < start + snippet_len) {
                ++count;
            }
        }
        num_each_term.push_back(count);
    }
    int term_i = 0;
    for (int i : num_each_term) {
        if (i > 0) {
            float tf_idf = ((float)bm25_stats[docID].first[term_i*2 + 3] / (float)bm25_stats[docID].first[2]) * log((float)bm25_stats[docID].first[0] / (float)bm25_stats[docID].first[term_i*2 + 4]);
            rating += (1 + 0.1 * i) * tf_idf; }
        ++term_i;
    }
    return rating;
}

string generateSnippet(int & docID, int& snippet_len, vector<string>& terms) {
    string filepath = docs_dir + to_string(docID) + ".bin";
    int snippet_start = 0;
    float snippet_rating = 0, max_snippet_rating = 0;
    vector<vector<int>> term_locs;
    
    ifstream file(filepath);
    string document((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    
    snippet_start = seekToEndOfHeader(document);
    
    for (int i = 0; i < terms.size(); ++i) {
        for (int start = snippet_start; start + terms[i].size() - 1 < document.size(); ++start) {
            if (document.substr(start, terms[i].size()) == terms[i]) {
                if (term_locs.size() < i + 1) term_locs.push_back({start});
                else term_locs[i].push_back(start);
            }
        }
    }
    for (int start = snippet_start; start + snippet_len - 1 < document.size(); ++start) {
        snippet_rating = rateSnippet(start, snippet_len, term_locs, docID);
        if (snippet_rating > max_snippet_rating) {
            max_snippet_rating = snippet_rating;
            snippet_start = start;
        }
    }
    string snippet = document.substr(snippet_start, snippet_len);
    replace(snippet.begin(), snippet.end(), '\n', ' ');
    return snippet;
}

vector<int> getQueryResults(const int& n_results, const string& docs_dir, vector<string>& q) {
    int nextID, docID = 0;
    int n_terms = termLists.size();
    int max_docID = getMaxDocID();
    int foundFlag;
    int stopFlag = 0;
    
    vector<int> docs_term(n_terms, 0);
    vector<int> lp(n_terms, 0);
    vector<int> freq(n_terms, 0);
    vector<int> results;
    
    priority_queue<pair<float, int>> max_heap;

    // utilizing AND semantics
    while (docID <= max_docID) {
        docID = nextGEQ(termLists[0], docID, lp, 0);
        if (docID == -1) break;
        foundFlag = 1;
        for (int i = 1; i < n_terms; ++i) {
            nextID = nextGEQ(termLists[i], docID, lp, i);
            if (nextID == docID) continue;
            else if (nextID == -1) {
                foundFlag = 0;
                stopFlag = 1;
                break;
            } else {
                docID = nextID;
                foundFlag = 0;
                break;
            }
        }
        if (foundFlag) {
            for (int i = 0; i < n_terms; ++i) {
                freq[i] = getFreq(termLists[i], lp, i);
                docs_term[i] = termLists[i].size()/2;
            }
            vector<int> page_bm25_stats;
            float score = bm25(docID, n_terms, freq, docs_term, page_bm25_stats);
            bm25_scores[docID] = score;
            bm25_stats[docID] = {page_bm25_stats, q};
            max_heap.push({score, docID});
            ++docID;
        }
        if (stopFlag) break;
    }
    
    cout << endl << "total number of valid results: " << max_heap.size() << endl;
    cout << "number of results returned: " << min(10, (int)max_heap.size()) << endl;
    cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"<< endl << endl;
    
    while (results.size() < n_results && !max_heap.empty()) {
        pair<float, int> result = max_heap.top();
        max_heap.pop();
        results.push_back(result.second); }
    
    return results;     //list of docIDs of top n results
}

void execute_query(vector<string>& q, const int& n_results, int& snippet_len, ifstream& index_stream) {
    vector<int> results;
    for (int i = 0; i < q.size(); ++i)
        openList(q[i], index_stream);    //load current query term lists into memory
    
    if (!termLists.empty()) {
        results = getQueryResults(n_results, docs_dir, q);
        for (int i = 1; i <= results.size(); ++i) {
            string snippet = generateSnippet(results[i-1], snippet_len, q);
            printResult(results[i-1], i, snippet); }
    } else cout << "No valid results.  Please try a different query";
    unordered_map<int, pair<vector<int>, vector<string>>>().swap(bm25_stats);
    unordered_map<int, float>().swap(bm25_scores);
    bm25_stats.clear(), bm25_scores.clear();
    closeLists(termLists);
}

//===============================================================================================
int main() {
    
    loadAuxDataStructs(term_dict_path, lexicon_path, page_table_path, page_len_path, total_len_docs);
    ifstream index_stream(compact_index_path, ios::in | ios::binary);
    
    int n_results = 10;
    int snippet_len = 500;
    
    while (true) {
        //LISTEN FOR QUERIES FOREVER UNTIL MANUALLY TERMINATED
        vector<string> q = getUserQuery();
        if (!q.empty()) execute_query(q, n_results, snippet_len, index_stream);
        else cout << "No results. Please revise your query." << endl << endl;
    }
}

//===============================================================================================
