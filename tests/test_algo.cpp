#include <iostream>
#include <gtest/gtest.h>
#include "../src/algorithms.h"

template<class T>
std::ostream& operator<<(std::ostream &o, const std::vector<T>& v) {
    for (auto k : v) {
        o << k << ' ';
    }
    return o;
}


TEST(TestSuite, TestPrefix) {
    std::vector<size_t> ans = {0, 0, 0, 1, 2, 3, 0};
    ASSERT_EQ(ans, algo::prefix_function("abcabcd"));
}


TEST(TestSuite, TestAlgo1) {
    std::string key = "aba";
    
    std::string w1 = "aba";
    std::string w2 = "abaaba";
    std::string w3 = "ababa";
    std::string w4 = "asa";

    std::string T = w1 + '\n' + w2 + '\n' + w3 + '\n' + w4 + '\n';
    
    std::vector<bool> ans = {1, 1, 1, 0};
    ASSERT_EQ(algo::KnuthMorrisPratt(key, T), ans);
}

TEST(TestSuite, TestAlgo2) {
    std::string key = "aa";
    
    std::string w1 = "aba";
    std::string w2 = "abaaba";
    std::string w3 = "ababa";
    std::string w4 = "asa";

    std::string T = w1 + '\n' + w2 + '\n' + w3 + '\n' + w4 + '\n';
    
    std::vector<bool> ans = {0, 1, 0, 0};
    ASSERT_EQ(algo::KnuthMorrisPratt(key, T), ans);
}

TEST(TestSuite, TestAlgo3) {
    std::string key = "abac";
    
    std::string w1 = "aba";
    std::string w2 = "abaaba";
    std::string w3 = "ababa";
    std::string w4 = "asa";

    std::string T = w1 + '\n' + w2 + '\n' + w3 + '\n' + w4 + '\n';
    
    std::vector<bool> ans = {0, 0, 0, 0};
    ASSERT_EQ(algo::KnuthMorrisPratt(key, T), ans);
}

TEST(TestSuite, TestAlgo4) {
    std::string key = "abac";
    
    std::string w1 = "aba";
    std::string w2 = "abaaba";
    std::string w3 = "ababa";
    std::string w4 = "abac";

    std::string T = w1 + '\n' + w2 + '\n' + w3 + '\n' + w4 + '\n';
    
    std::vector<bool> ans = {0, 0, 0, 1};
    ASSERT_EQ(algo::KnuthMorrisPratt(key, T), ans);
}

TEST(TestNonSeq, Test1) {
    std::string key = "aba";
    
    std::string w1 = "aba";
    std::string w2 = "abaaba";
    std::string w3 = "ababa";
    std::string w4 = "abac";

    std::string T = w1 + '\n' + w2 + '\n' + w3 + '\n' + w4 + '\n';

    std::vector<bool> ans = {1, 1, 1, 1};
    ASSERT_EQ(algo::nonsequencial_substring_algorithm(key, T), ans);
}
