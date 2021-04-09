#pragma once 
#include <vector>
#include <string>

namespace algo {

// Default prefix function implementation
std::vector<size_t> prefix_function(const std::string& S) {
    std::vector<size_t> p(S.size(), 0);

    for (size_t i = 1; i != S.size() - 1; ++i) {
        size_t k = p[i - 1];
        while (k > 0 && S[i] != S[k]) {
            k = p[k - 1];
        }
        if (S[i] == S[k]) {
            ++k;
        }
        p[i] = k;
    }
    return p;
}

// Modified Knuth-Morris-Pratt algorithm
// Calculates prefix function for \n-separated words and
// identifies if pattern is consecutive substring of each word
std::vector<bool> KnuthMorrisPratt(const std::string& P, const std::string& T) {
    std::vector<bool> ans;
    size_t i = 0;
    bool scored = false;
    
    std::string S = P + '#' + T;
    
    //Prefix function
    std::vector<size_t> p = prefix_function(S);
    
    while (i != T.size()) {
        if (S[P.size() + i + 1] == '\n') {
            ans.push_back(scored);
            scored = false;
        }
        if (!scored && p[P.size() + i + 1] == P.size()) {
            scored = true;
        }
        ++i;
    }
    return ans;
}

// Brute force O(nm) algorithm for finding nonconsecutive substrings
std::vector<bool> nonconsecutive_substring_algorithm(const std::string& P, const std::string& T) {
    size_t j = 0;
    std::vector<bool> res;
    while (j != T.size()) {
        size_t i = 0;
        while (i != P.size() && T[j] != '\n') {
            if (P[i] == T[j]) {
                ++i;
            }
            ++j;
        }
        while (T[j] != '\n') {
            ++j;
        }
        ++j;
        res.push_back(i == P.size());
    }
    return res;
}


} //namespace algo
