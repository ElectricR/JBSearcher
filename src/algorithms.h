#pragma once 
#include <vector>
#include <string>

namespace algo {

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

std::vector<bool> KnuthMorrisPratt(const std::string& P, const std::string& T) {
    std::string S = P + '#' + T;
    std::vector<size_t> p = prefix_function(S);
    std::vector<bool> ans;
    size_t i = 0;
    bool scored = false;
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

std::vector<bool> nonsequencial_substring_algorithm(const std::string& P, const std::string& T) {
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
        if (i == P.size()) {
            res.push_back(1);
        }
        else {
            res.push_back(0);
        }
    }
    return res;
}


} //namespace algo
