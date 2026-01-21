#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <utility>
#include <tuple>
#include "all_headers.h"

//рациональные числа
template <typename T>
class IGroup{
public:
    virtual T Add(const T& a, const T& b) const = 0;
    virtual T Inverse(const T& a) const = 0;
    virtual T Zero() const = 0;
};
template <typename T>
class IRing : public IGroup<T>{
public:
    virtual T Multiply(const T& a, const T& b) const = 0;
    virtual T One() const = 0;
};

class IntegerGroup : public IGroup<int>{
public:
    int Add(const int& a, const int& b) const override{
        return a+b;
    }
    int Inverse(const int& a) const override{
        return (-1)*a;
    }
    int Zero() const override{
        return 0;
    }
};
class IntegerRing : public IRing<int>{
public:
    int Add(const int& a, const int& b) const override{
        return a+b;
    }
    int Inverse(const int& a) const override{
        return (-1)*a;
    }
    int Zero() const override{
        return 0;
    }
    int Multiply(const int& a, const int& b) const override {
        return a*b;
    }
    int One() const override {
        return 1;
    }
};
template <typename T>
void TestSequence(Sequence<T>* seq){
    int fails = 0;
    for (int i = 0; i < 8; ++i){
        seq = seq->Append(i);
    }
    std::cout << "=================\n";
    std::cout << "Start Test:\n";
    std::cout << "Print Sequence (enumerate):\n";
    auto it = seq->GetEnumerator();
    while (it->MoveNext()){
        std::cout << it->Current() << " ";
    }
    std::cout << "\n";
    delete it;
    std::cout << "Print Sequence (operator<<):\n";
    std::cout << *seq;
    std::cout << "\n";
    if (seq->GetFirst() != 0) { std::cout << "Fail: Sequence GetFirst\n"; ++fails; }
    if (seq->GetLast() != 7) { std::cout << "Fail: Sequence GetLast\n"; ++fails; }
    if (seq->GetLength() != 8) { std::cout << "Fail: Sequence GetLength\n"; ++fails; }
    if (seq->Get(1) != 1) { std::cout << "Fail: Sequence Get(1)\n"; ++fails; }
    auto subseq = seq->GetSubsequence(5, 7);
    if (subseq->GetLength() != 3 || subseq->Get(0) != 5 || subseq->Get(1) != 6 || subseq->Get(2) != 7) {
        std::cout << "Fail: Sequence GetSubList\n"; ++fails;
    }
    std::cout << "True subseq:\n {5, 6, 7}:\n";
    std::cout << "test subseq:\n";
    auto it1 = subseq->GetEnumerator();
    while (it1->MoveNext()){
        std::cout << it1->Current() << " ";
    }
    delete it1;
    std::cout << "\n";
    delete subseq;
    std::cout <<"Test concat + vector init, true: {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n";
    auto seq2 = new MutableArraySequence<int>({8, 9, 10});
    seq = seq->Concat(seq2);
    std::cout << *seq;
    delete seq2;
    auto only2_seq = seq->where([] (int& a) -> bool {return a%2==0;});
    std::cout <<"Test where, true: {0, 2, 4, 6, 8, 10}\n";
    std::cout << *only2_seq;
    std::cout <<"Test map, true: {1, 3, 5, 7, 9, 11}\n";



    // virtual Sequence<T>* Map(T (*func)(T&)) = 0;
    // virtual Sequence<T>* Map(T (*func)(T&, int)) = 0;


    only2_seq = only2_seq->Map([] (int& a) -> int {return a+1;});
    std::cout << *only2_seq;
    std::cout <<"Test map + index, true: {0, 1, 2, 3, 4, 5}\n";
    only2_seq = only2_seq->Map([] (int& a, int index) -> int {return a-index;});
    std::cout << *only2_seq;
    delete only2_seq;
    std::cout <<"Test Zip3, true: {[0, a, 1.2], [1, b, 3.4], [2, c, 7.7]}\n";
    std::vector<std::string> vc2 = {"a", "b", "c", "d"};
    std::vector<float> vc3 = {1.2, 3.4, 7.7};
    auto strs = new ImmutableArraySequence<std::string>(vc2);
    auto floatt = new ImmutableArraySequence<float>(vc3.data(), 3);
    auto zipped = Zip3(seq, strs, floatt);
    std::cout << *zipped;
    delete zipped;
    delete strs;
    delete floatt;
    std::cout <<"Test Reduce, true: 56\n";
    std::cout << seq->Reduce([] (int a, int b) -> int {return a+b;}, 1) << "\n";
    if ((*seq)[8] !=8) { std::cout << "Fail: Sequence operator[8]\n"; ++fails;}
    std::cout <<"Test InsertAt, true: {0, 5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n";
    seq = seq->InsertAt(5, 1);
    std::cout << *seq;
    std::cout <<"Test Prepend, true: {-1, 0, 5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n";
    seq = seq->Prepend(-1);
    std::cout << *seq;
    seq->Set(1, 99);
    if (seq->Get(1) != 99) { std::cout << "Fail: Sequence Set(1, 99)\n"; ++fails; }

    std::cout << "Test End, Summ fails: " << fails;
}
int main() {
    // визуальная проверка + немного автоматики
    // проверил, все работает
    std::cout << "IMMUTABLE_ARRAY_SEQUENCE<int>():\n";
    auto seq1 = new ImmutableArraySequence<int>();
    TestSequence(seq1);
    std::cout << "\n";
    delete seq1;

    std::cout << "IMMUTABLE_LIST_SEQUENCE<int>():\n";
    auto seq2 = new ImmutableListSequence<int>();
    TestSequence(seq2);
    std::cout << "\n";
    delete seq2;

    std::cout << "SEGMENTED_DEQUE<int>(4):\n";
    auto seq3 = new SegmentedDeque<int>(4);
    TestSequence(seq3);
    std::cout << "\n";
    delete seq3;


    std::cout << "SkipListSequence<int>():\n";
    auto seq4 = new SkipListSequence<int>();
    TestSequence(seq4);
    std::cout << "\n";
    delete seq4;
    return 0;
}