#include <iostream>
#include <chrono>
#include "BinaryTree.h"
#include "AllTreeTest.h"
//ОБЯЗАТЕЛЬНО!!!
// g++ TestBinaryTree.cpp -std=c++20 -o TestBinaryTree
int mainMenu() {
    std::cout << "1. Test ops\n2. Performance test\n3. Exit\n";
    int choice; std::cin >> choice; return choice;
}

void Performance() {
    using namespace std::chrono;
    BinaryTree<int> tree;
    const int N = 30000;
    auto start = high_resolution_clock::now();
    for(int i=0;i<N;++i) tree.Insert(rand());
    auto mid = high_resolution_clock::now();
    for(int i=0;i<N;++i) tree.Search(rand());
    auto end = high_resolution_clock::now();
    auto ins = duration_cast<milliseconds>(mid-start).count();
    auto sea = duration_cast<milliseconds>(end-mid).count();
    std::cout << ins << "," << sea << std::endl;
}

int main() {
    int c;
    while((c = mainMenu())!=3) {
        if(c==1) {
            RunAllTests();
        } 
        else if(c==2) {
            Performance();
        }
    }
    return 0;
}