#include <iostream>
#include <cassert>
#include "BinaryTree.h"
#include "Person.h"

void TestInt() {
    BinaryTree<int> tree;
    tree.Insert(5);
    tree.Insert(3);
    tree.Insert(7);
    assert(tree.Search(3));
    assert(tree.Size() == 3);
    tree.Remove(3);
    assert(!tree.Search(3));
    int sum = tree.Reduce([](int a,int b){return a+b;}, 0);
    assert(sum == 12);
    std::cout << "TestInt passed\n";
}

void TestPerson() {
    BinaryTree<Person> tree;
    Person p1(1,"Ivan","Petrov",0);
    Person p2(2,"Anna","Ivanova",0);
    tree.Insert(p1);
    tree.Insert(p2);
    assert(tree.Search(p2));
    tree.Remove(p1);
    assert(!tree.Search(p1));
    std::cout << "TestPerson passed\n";
}

int main() {
    TestInt();
    TestPerson();
    return 0;
}
