#ifndef ARRAYSEQUENCE_H
#define ARRAYSEQUENCE_H
// #include "HandleError.h"
#include <stdexcept>
#include <string>
#include <vector>
#include "numerable.h"
#include "Sequence.h"
#include "DynamicArray.h"

// HandleError убран, заменён на throw
template <class T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* array;
    virtual ArraySequence<T>* Instance() = 0;
    int ValidateIndex(int i) const {
        if (i < 0) i = -i - 1;
        if (i >= this->GetSize()) {
            // HandleError(IndexOutOfRange, "class ArraySequence : public Sequence<T> в функции int ValidateIndex(int i) const", "", 0);
            throw std::out_of_range("ArraySequence::ValidateIndex: Index " + std::to_string(i) + " is out of range [0, " + std::to_string(this->GetSize()-1) + "]");
        }
        return i;
    }
public:
    virtual std::string GetFinalDerivedClass() const = 0;
    int  GetSize()  const { return array->GetSize(); }
    void Resize(int n)    { array->Resize(n); }
    void Set(int i, T v) override { array->Set(i, std::move(v)); }
    // using Sequence<T>::GetFirst();
    // using Sequence<T>::GetLast();
    // using Sequence<T>::Get();
    // using Sequence<T>::GetSubsequence();
    // using Sequence<T>::GetLength();
    // using Sequence<T>::Append();
    // using Sequence<T>::Prepend();
    // using Sequence<T>::InsertAt();
    // using Sequence<T>::Concat();
    // using Sequence<T>::Map();
    // using Sequence<T>::where();
    // using Sequence<T>::Reduce();
    // using Sequence<T>::operator[];
    ArraySequence(){
        array = new DynamicArray<T>();
    }
    ArraySequence(T* data, int count){
        array = new DynamicArray<T>(data, count);
    }
    explicit ArraySequence(int size){
        array = new DynamicArray<T>(size);
    }
    ArraySequence(ArraySequence<T>& other){
        array = new DynamicArray<T>(*other.array);
    }
    ArraySequence(DynamicArray<T>& other){
        array = new DynamicArray<T>(other);
    }
    ArraySequence(DynamicArray<T>&& other){
        array = new DynamicArray<T>(std::move(other));
    }
    ArraySequence(const std::vector<T>& other){
        array = new DynamicArray<T>(other);
    }
    ~ArraySequence() {
        delete array;
    }
    T GetFirst() const override{
        if (this->GetSize() == 0){
            // HandleError(IndexOutOfRange, "class ArraySequence : public Sequence<T> в функции T GetFirst() const override", "массив пуст, нельзя выдать первый элемент", 0);
            throw std::out_of_range("ArraySequence::GetFirst: Cannot get first element - sequence is empty");
        }
        return this->array->Get(0);
    }
    T GetLast() const override{
        // if (array->GetSize == 0){
        //     HandleError(IndexOutOfRange, "class ArraySequence : public Sequence<T> в функции T GetFirst() const override", "массив пуст, нельзя выдать первый элемент", 0);
        //     throw std::out_of_range("Sequence is empty");
        // }
        return this->array->Get(this->GetSize()-1);
    }
    T Get(int index) const override{
        return this->array->Get(index);
    }
    T PopFront() override{
        T temp = GetFirst();
        array = GetSubsequence(1, GetLength()-1)->array;
        return temp;
    }
    T PopBack() override{
        T temp = GetLast();
        array->Resize(GetLength()-1);
        return temp;
    }
    ArraySequence<T>* GetSubsequence(int startIndex, int endIndex) override{
        startIndex = this->ValidateIndex(startIndex);
        endIndex = this->ValidateIndex(endIndex);
        if (startIndex <0 || endIndex >= this->GetSize() || startIndex>endIndex){
            // HandleError(IndexOutOfRange, "class ArraySequence : public Sequence<T> в функции ArraySequence<T>* GetSubsequence(int startIndex, int endIndex) const override", "", 0);
            throw std::out_of_range("ArraySequence::GetSubsequence: Invalid indices - startIndex: " + std::to_string(startIndex) + ", endIndex: " + std::to_string(endIndex) + ", size: " + std::to_string(this->GetSize()));
        }
        ArraySequence<T>* subSeq = Instance();
        int size = endIndex-startIndex+1;
        try{
            subSeq->array = new DynamicArray<T>(size);
            for (int i = 0; i < size; i++){
                subSeq->Set(i, this->Get(i+startIndex));
            }
        }
        catch(std::bad_alloc){
            // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции ArraySequence<T>* GetSubsequence(int startIndex, int endIndex) const override", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции ArraySequence<T>* GetSubsequence(int startIndex, int endIndex) const override: operation failed", "", 0);
            throw std::runtime_error("ArraySequence::GetSubsequence: Unknown error occurred during subsequence creation");
            return nullptr;
        }
        return subSeq;
    }
    int GetLength() const override{
        return this->GetSize();
    }
    Sequence<T>* Append(T item) override{
        auto instance = std::unique_ptr<ArraySequence<T>>(this->Instance());
        instance->Resize(instance->GetSize()+1);
        instance->Set(instance->GetSize()-1, std::move(item));
        return instance.release();
    }
    Sequence<T>* Prepend(T item) override{
        auto instance = std::unique_ptr<ArraySequence<T>>(this->Instance());
        instance->Resize(instance->GetSize()+1);
        for (int i = instance->GetSize()-1; i >0; --i){
            instance->Set(i, std::move(instance->Get(i-1)));
        }
        instance->Set(0, std::move(item));
        return instance.release();
    }
    Sequence<T>* InsertAt(T item, int index) override {
        auto instance = std::unique_ptr<ArraySequence<T>>(this->Instance());
        index = instance->ValidateIndex(index);
        instance->Resize(instance->GetSize()+1);
        for (int i = instance->GetSize()-1; i >index; --i){
            instance->Set(i, std::move(instance->Get(i-1)));
        }
        instance->Set(index, std::move(item));
        return instance.release();
    }
    Sequence<T>* Concat(Sequence<T>* list) override{
        try{
            if (!list) return this->Instance();
            auto instance = std::unique_ptr<ArraySequence<T>>(this->Instance());
            const int oldSize = instance->GetSize();
            instance->Resize(list->GetLength()+oldSize);
            for (int i = oldSize; i < oldSize+list->GetLength(); ++i){
                instance->Set(i, std::move(list->Get(i-oldSize)));
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Concat(Sequence<T>* list) const override", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Concat(Sequence<T>* list) const override: operation failed", "", 0);
            throw std::runtime_error("ArraySequence::Concat: Unknown error occurred during concatenation");
            return nullptr;
        }
    }
    Sequence<T>* Map(T (*func)(T&)) override{
        ArraySequence<T>* raw = nullptr;
        try{
            raw = this->Instance();
            auto instance = std::unique_ptr<ArraySequence<T>>(raw);
            raw = nullptr;
            for (int i = 0; i < instance->GetLength(); ++i){
                T temp = this->Get(i);
                instance->Set(i, func(temp));
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T)) override", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            delete raw;
            // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T)) override: operation failed", "", 0);
            throw std::runtime_error("ArraySequence::Map: Unknown error occurred during mapping operation");
        }
    }
    Sequence<T>* Map(T (*func)(T&, int)) override{
        ArraySequence<T>* raw = nullptr;
        try{
            raw = this->Instance();
            auto instance = std::unique_ptr<ArraySequence<T>>(raw);
            raw = nullptr;
            // instance->Get(i)
            for (int i = 0; i < instance->GetLength(); ++i){
                T temp = instance->Get(i);
                instance->Set(i, func(temp, i+1));
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T, int))", "", 0);
            throw std::bad_alloc();
        }
        catch(...){
            delete raw;
            // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T, int)): operation failed", "", 0);
            throw std::runtime_error("ArraySequence::Map: Unknown error occurred during indexed mapping operation");
        }
    }
    // T&& GetMove(int index) override {
    //     return this->array->GetMove(index);
    // }
    Sequence<T>* where(bool (*predicate)(T&)) override{
        ArraySequence<T>* raw = nullptr;
        try{
            raw = this->Instance();
            auto instance = std::unique_ptr<ArraySequence<T>>(raw);
            raw = nullptr;
            int j = 0;
            for (int i = 0; i < instance->GetLength(); ++i){
                T temp = Get(i);
                if (predicate(temp)){
                    // instance->Set(j, instance->Get(i));
                    // instance->array->move_element(*this, i, *this, j);
                    if (i!=j){
                        instance->Set(j, temp);
                    }
                    j++;
                }
            }
            instance->Resize(j);
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции Sequence<T> where(bool (*predicate)(T))", "", 0);
            throw std::bad_alloc();
        }
        catch(...){
            delete raw;
            // HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции Sequence<T> where(bool (*predicate)(T)): operation failed", "", 0);
            throw std::runtime_error("ArraySequence::where: Unknown error occurred during filtering operation");
        }
    }
    T Reduce(T (*func)(T, T), T init) override{
        for (int i = 1; i < this->GetLength(); ++i){
            init = func(this->Get(i), init);
        }
        // T a = std::move(init);
        return init;
    }

    template <typename T1, typename T2>
    friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>*, Sequence<T2>*);

    template <typename T1, typename T2, typename T3>
    friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*);

    template <typename... seqs>
    friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences);

    T operator[](int index) const override{
        return this->Get(index);
    }
    std::ostream& print(std::ostream& os) const override{
        os << this->GetFinalDerivedClass() << ": [";
        for (int i = 0; i < this->GetLength(); ++i){
            // Не пытаемся печатать значение T напрямую (operator<< может не быть определён),
            // поэтому выводим только индекс элемента.
            os << "[ index=" << i << " ]";
            if (i+1 < this->GetLength()){
                os << ", ";
            }
        }
        os << "]" << std::endl;
        return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const ArraySequence<T>& arr){
        return arr.print(os);
    }
    class Enumerator : public IEnumerator<T> {
    private:
        IEnumerator<T>* dyn_arr;
    public:
        explicit Enumerator(const ArraySequence<T>& arr) : dyn_arr(arr.array->GetEnumerator()){}
        ~Enumerator() override {
            delete dyn_arr;
        }
        void Reset() override{
            dyn_arr->Reset();
        }
        bool MoveNext() override{
            return dyn_arr->MoveNext();
        }
        T Current() override{
            return dyn_arr->Current();
        }
    };
    IEnumerator<T>* GetEnumerator() const override{
        return new Enumerator(*this);
    }
};
// template <typename T1, typename T2>
// Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2){
//     // std::unique_ptr<ListSequence<std::tuple<T1, T2>>> result;
//     try{
//         int minSize = std::min(seq1->GetLength(), seq2->GetLength());
//         auto result = std::make_unique<ArraySequence<std::tuple<T1, T2>>>(minSize);
//         for (int i = 0; i < minSize; ++i){
//             result->Set(i, std::make_tuple(seq1->Get(i), seq2->Get(i)));
//         }
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2): operation failed", "", 0);
//         return nullptr;
//     }
// }
// template <typename T1, typename T2, typename T3>
// Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>* seq1, Sequence<T2>* seq2, Sequence<T3>* seq3){
//     try{
//         int minSize = std::min({seq1->GetLength(), seq2->GetLength(), seq3->GetLength()});
//         auto result = std::make_unique<ArraySequence<std::tuple<T1, T2, T3>>>(minSize);
//         for (int i = 0; i < minSize; ++i){
//             result->Set(i, std::make_tuple(seq1->Get(i), seq2->Get(i), seq3->Get(i)));
//         }
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*): operation failed", "", 0);
//         return nullptr;
//     }
// }
// template <typename... seqs>
// Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences){
//     try{
//         int minSize = std::min({sequences->GetLength()...});
//         auto result = std::make_unique<ArraySequence<std::tuple<typename seqs::ValueType...>>>(minSize);
//         for (int i = 0; i < minSize; ++i){
//             result->Set(i, std::make_tuple(sequences->Get(i)...));
//         }s
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ArraySequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences): operation failed", "", 0);
//         return nullptr;
//     }
// }
#endif
