# web-search-engine
Creates Varbyte-Compressed Inverted Index of the Common Crawl dataset (https://commoncrawl.org/), as well as associated Lexicon, Term Dictionary, and Page Table.  Allows real-time querying (both conjunctive and disjunctive) of millions of pages ranked by BM25 Score, with average query time &lt; 1s. Dynamically generates ranked snippets based on query term TF-IDF at runtime. 

# Overview
 This program builds off of the previous work of creating an inverted index from the common crawl dataset, and allows a user to perform queries on this data. (Note: for more detailed information regarding the indexing stage, please consult previously submitted document “InvertedIndex”). In order to fully implement this program, some modifications to the inverted indexing program were necessary, so those changes will be described here, in addition to a detailed explanation of the querying program.
 
The main function of the querying program is designed as follows:

Program Structure

I.) Load in auxiliary data structures into main memory, which are needed in order perform querying. These include: - Term-to-TermIDDictionary
 - Lexicon which contains the following:
 - TermID
 - Start position in Index - Length in Index
 - Page Table, a dictionary that contains <Key>:<Value> pairs <DocID>:<URL>
 - Page Length Table, a dictionary that contains <Key>:<Value> pairs <DocID>:<Document Length>. The
purpose of this auxiliary data structure is to provide useful document information when calculating the BM25 Score of query candidate pages.

II.) Set parameters that will define the content that is returned for a given query. These include:
- n_results - defines the number of results to return for any query
- snippet_len - defines the length of snippets that will be displayed to preview contents of results.
  
 III.)* Get User Query
The program will listen for command line query input from the user. This process parses the query into separate terms, and returns these as a list.

IV.)* Execute Query
The program takes the list of user query terms and executes the query in the following steps:
- Lookup each term’s Start and Length in Lexicon.
- Read list containing Document IDs and Frequencies for each term into memory. - Using NextGEQ operation, produce all candidates that contain all search terms.
- Rank each candidate using BM25 Score.
- Select top n_results based on candidates with top BM25 score.
- Generate snippet for each top result.
- Display results.

*Steps III and IV will run in an infinite loop, until the program is manually terminated.

## Instructions - How To Run The Indexing Program

This program is written in C++. In order to run the program, one simply needs to run the executable file ‘Common Crawl Search Engine’ located in the project root directory. However, it must be noted that it is necessary to load the auxiliary data structures into memory at runtime — without these files, the program will not run properly. (Data files are not included with the submission for practical file size considerations).

# Updates to Indexing Program - Outputting additional data:

- Documents files - Full documents are stored in the ‘_data/documents’ sub-directory. The naming convention follows this format: <Assigned docID>.bin — these full documents will be useful when generating snippets for a given (dynamic) search query on a given document.

- Page Length Table - This is a table containing <key>:<value> pairs of <docID>:<document length>. This will be very useful when needing to calculate page BM25 scores during query execution.
   
# Query Processing
The user enters a string query (simply a string that will contain any combination of characters, number of terms, etc). This raw query needs to be processed and parsed into terms, which are what will actually be used to query the index and ultimately return pages as search results.

Any non-alphanumeric characters will be discarded. Terms are delimited by the SPACE (‘ ’) character. Each individual term-string is then converted to all lowercase characters. The term is then cross-referenced against a list of common stop words that should be excluded. Assuming it is not a stop word, it is then added to a SET of terms which represents the current query. The set data structure is used here as a simple way to avoid duplicate search terms in the query.

# Loading Term Lists into Memory
Now that we have parsed the query into a list of terms, we will need to find the location and length of each term in the Inverted Index (located on disk, too large to load at once into main memory). We do this by looking each term up in the Lexicon (which is loaded into main memory in a D.I.M.D.S.).

From the Inverted Index, we read in each term’s Document ID list and corresponding Term Frequency list.

# Finding Common Documents Using NextGEQ Algorithm
Once we have loaded each terms Document ID and Frequency lists into memory, we use a NextGEQ algorithm to find the intersection of documents across all terms. In this implementation, the user is given the option between conjunctive and disjunctive querying.

Implementation of NextGEQ function:
In pseudocode terms:
 • Initialize docID to 0
  • while (docID < largest docID across all of the query terms)...
   • docID = Next Greater or Equal (nextGEQ) to current docID, in term #0 docID list
   • For (all additional terms)...
    • If (they all contain the same docID):
     • Get each Term Frequency and # Documents containing each term
     • Calculate Page BM25 score and add to MAX HEAP
    • If (the searched-for docID is higher than the max in any list, stop looking)
    • Else (set current docID to the next highest docID and start from the top)
   
 Disjunctive Querying: Any result candidates must contain at least one of the query terms. In pseudocode terms:
 • For each term:
  • Calculate the BM25 Score for all documents containing that term
  • Add Document to Max Heap
 • Return top 10 results from heap
 
# Calculating BM25 Score
The BM25 Score is calculated according to the BM25 formula, where:
N - total number of documents contained in the index.
ft - number of documents containing term t. This is easily obtained by the length of the in-memory term docID list.
fd,t - frequency of term in document d. Obtained by the length of the in-memory term frequency list.
d - length of document. This is obtained from the in-memory page length table.
davg - average document length. This is calculated dynamically upon loading the page-length table into memory.
k1 = 1.2, b = 0.75
    
# Generating/Rating Snippets
The program returns the top n results, obtained by reading from the top of the max heap of result candidates. A snippet is then generated for each of these top results. The algorithm for generating snippets is designed in the following way:

1. Now it becomes necessary to reference the original documents. For each top-n result, we load the document into memory, from the document file that was created during indexing. It would also be possible to read the document directly from the common crawl data, if instead of exporting each document as an individual file, its location in the common crawl dataset was recorded during indexing. I would implement this in the future with some additional time. Once the file is loaded into memory, for each query term, we find all occurrences of that term in the document, and record the positions in a list.

2. Now that we have positional information for each query term in the document, we use a sliding window of snippet_len characters, and count the number and frequency of each term within the sliding window.

3. We use this to generate a “snippet ranking” for each window. While the snippet ranking takes into account the frequency of each term in the window, it gives a more significant weight to the presence of multiple terms contained within the window. Each term’s contribution to the snippet rating is a function of its Term Frequency - Inverse Document Frequency. This is used in order to give a higher weight to “rarer” query terms that are found within the snippet.

## Runtime / Performance of Snippet Generation
The average runtime complexity for generating a snippet for a single document is in the order of O(n * k * m), where:
n = the length of the document
k = number of terms in query
m = average number of occurrences of term in document
On average, generating snippets for 10 documents takes less than a second. Obviously, for very large documents, it takes longer to dynamically generate snippets, as n increases.
  
# Displaying Results
Once we have considered all snippet windows within the document, the snippet with the highest rating is displayed to the user along with the corresponding page. Each top results’ BM25 score is also displayed, along with additional term and document information that was used to calculate the score. Each query also displays the total number of valid results next to the number of results actually returned.
  
# Performance
Overall, the most computationally expensive part of building the search engine was the indexing of documents. This stage takes in the order of hours to complete. This preprocessing stage would obviously need to be completed prior to accepting user queries on the data, or in the background if the index is being updated for recrawls.

Once this stage is complete, the main bottleneck is loading the Term Dictionary, Lexicon and Page Tables into main memory upon starting the Querying program. This takes a couple minutes on average, however once these have been loaded into memory, a user can make as many queries as they wish, without having to reload the data structures upon each query.

On average, queries execute quickly, in usually less than a second — this includes ranking all candidate documents, generating snippets and displaying results. This performance seems sufficient for general search engine usage.

Time it takes to load all auxiliary data structures into memory: 2m 45s
Main memory requirement to load auxiliary data structures: ~2.5 GB
Average time to perform query: <1s to deliver 10 results with dynamically-selected snippets

In total, I processed 52 common crawl files, or about 2.3M pages.
This amounts to a total of 754445280 postings.
23,146,085 unique terms in Lexicon.

# Follow-Up Work To Be Done
The inverted index is compressed using varbyte compression. I would like to in the future implement additive varbyte compression, which would give even more optimized memory performance, both on disk and in main memory.
 
