#ifndef ZIPOPERATIONSEQUENCE_H
#define ZIPOPERATIONSEQUENCE_H
#include <stdexcept>
#include <string>
#include "Sequence.h"
#include "ImmutableArraySequence.h"
#include "MutableArraySequence.h"
#include "ArraySequence.h"
// HandleError убран, заменён на throw
template <typename T1, typename T2>
Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2){
    // std::unique_ptr<ListSequence<std::tuple<T1, T2>>> result;
    auto raw = new MutableArraySequence<std::tuple<T1, T2>>();
    try{
        int minSize = std::min(seq1->GetLength(), seq2->GetLength());
        std::unique_ptr<MutableArraySequence<std::tuple<T1, T2>>> result(raw);
        raw = nullptr;
        for (int i = 0; i < minSize; ++i){
            result->Append(std::make_tuple(seq1->Get(i), seq2->Get(i)));
        }
        return result.release();
    }
    catch(std::bad_alloc){
        delete raw;
        // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2)", "", 0);
        throw std::bad_alloc();
    }
    catch(...){
        delete raw;
        // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2): operation failed", "", 0);
        throw std::runtime_error("Zip: Unknown error occurred during zip operation");
    }
}
template <typename T1, typename T2, typename T3>
Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>* seq1, Sequence<T2>* seq2, Sequence<T3>* seq3){
    auto raw = new MutableArraySequence<std::tuple<T1, T2, T3>>();
    try{
        int minSize = std::min({seq1->GetLength(), seq2->GetLength(), seq3->GetLength()});
        std::unique_ptr<MutableArraySequence<std::tuple<T1, T2, T3>>> result(raw);
        raw = nullptr;
        for (int i = 0; i < minSize; ++i){
            result->Append(std::make_tuple(seq1->Get(i), seq2->Get(i), seq3->Get(i)));
        }
        return result.release();
    }
    catch(std::bad_alloc){
        delete raw;
        // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*)", "", 0);
        throw std::bad_alloc();
    }
    catch(...){
        delete raw;
        // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*): operation failed", "", 0);
        throw std::runtime_error("Zip3: Unknown error occurred during zip3 operation");
    }
}
template <typename... seqs>
Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences){
    auto raw = new MutableArraySequence<std::tuple<typename  seqs::ValueType...>>();
    try{
        int minSize = std::min({sequences->GetLength()...});
        std::unique_ptr<MutableArraySequence<std::tuple<typename seqs::ValueType...>>> result(raw);
        raw = nullptr;
        for (int i = 0; i < minSize; ++i){
            result->Append(std::make_tuple(sequences->Get(i)...));
        }
        return result.release();
    }
    catch(std::bad_alloc){
        delete raw;
        // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences)", "", 0);
        throw std::bad_alloc();
    }
    catch(...){
        delete raw;
        // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences): operation failed", "", 0);
        throw std::runtime_error("ZipN: Unknown error occurred during zipN operation");
    }
}
// template <typename... arg>
// auto UnzipN(Sequence<std::tuple<arg...>>* zippedSeq){
//     std::tuple<typename arg::ValueType...>* unzipedSeq();
// }
#endif