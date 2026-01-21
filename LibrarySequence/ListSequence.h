#ifndef LISTSEQUENCE_H
#define LISTSEQUENCE_H
// #include "HandleError.h"
#include "numerable.h"
#include "LinkedList.h"

// поменял HandleError на throw
template <class T>
class ListSequence : public Sequence<T>{
protected:
    LinkedList<T>* list;
    //Instance можно улучшить - выдавать умный указатель, но придется все переделывать
    virtual ListSequence<T>* Instance() = 0;
    virtual const ListSequence<T>* Instance() const = 0;
    // virtual ListSequence<T>* Clone() const;
    virtual ListSequence<T>* CreateNew(LinkedList<T>&& list) const = 0;
public:
    virtual std::string GetFinalDerivedClass() const = 0;
    explicit ListSequence(int size) : list(new LinkedList<T>(size)){};
    ListSequence(const T* data, int size) : list(new LinkedList<T>(data, size)){};
    ListSequence(const LinkedList<T>& newlist): list(new LinkedList<T>(newlist)) {};
    ListSequence(LinkedList<T>&& newlist) : list(new LinkedList<T>(std::move(newlist))) {};
    
    explicit ListSequence() : list(new LinkedList<T>()) {};
    ~ListSequence() override {
        delete this->list;
    }
    T PopFront() override{
        return list->PopFront();
    }
    T PopBack() override{
        return list->PopBack();
    }
    T GetFirst() const override {
        return this->list->GetFirst();
    }

    T GetLast() const override {
        return this->list->GetLast();
    }
    T Get(int index) const override {
        return this->list->Get(index);
    }
    void Set(int i, T v) override { list->Set(i, std::move(v)); }
    // old version
    // Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
    //     try{
    //         // std::unique_ptr<LinkedList<T>> subList(list->GetSubList(startIndex, endIndex));
    //         // return new ListSequence<T>(std::move(*subList));  // Указываем тип ListSequence<T>
    //         std::unique_ptr<LinkedList<T>> subList(list->GetSubList(startIndex, endIndex));
    //         return this->CreateNew(std::move(*subList));
    //     }
    //     catch(std::bad_alloc){
    //         HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override", "", 0);
    //         return nullptr;
    //     }
    //     catch(...){
    //         HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override: operation failed", "", 0);
    //         return nullptr;
    //     }
        
    // }
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
    // проверяем корректность индексов
    if (startIndex < 0 || endIndex >= this->GetLength() || startIndex > endIndex) {
        throw std::out_of_range("ListSequence GetSubsequence: invalid range");
        return nullptr;
    }
    auto result = this->CreateNew(LinkedList<T>());
    auto it = this->list->GetEnumerator();
    int idx = 0;
    while (it->MoveNext()) {
        if (idx >= startIndex && idx <= endIndex) {
            // добавляем элемент в новую последовательность
            result->list->Append(it->Current());
        }
        ++idx;
        if (idx > endIndex) break;  // можно прервать досрочно
    }
    delete it;
    return result;
    }
    int GetLength () const override{
        return this->list->GetLength();
    }

    Sequence<T>* Append(T item) override{
        auto instance = std::unique_ptr<ListSequence<T>>(this->Instance());
        instance->list->Append(item);
        return instance.release();
    }
    Sequence<T>* Prepend(T item) override{
        auto instance = std::unique_ptr<ListSequence<T>>(this->Instance());
        instance->list->Prepend(item);
        return instance.release();
    }
    Sequence<T>* InsertAt(T item, int index) override {
        auto instance = std::unique_ptr<ListSequence<T>>(this->Instance());
        instance->list->InsertAt(item, index);
        return instance.release();
    }
    Sequence<T>* Concat(Sequence<T>* otherSeq) override {
        try{
            auto instance = std::unique_ptr<ListSequence<T>>(this->Instance());
            if (otherSeq) {
                auto it = otherSeq->GetEnumerator();
                while (it->MoveNext()) {
                    instance->list->Append(it->Current());
                }
                delete it;
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            // HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции Sequence<T>* Concat(Sequence<T>* otherlist) override", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            // HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции Sequence<T>* Concat(Sequence<T>* otherlist) override: operation failed", "", 0);
            std::runtime_error("ListSequence Concat: operation failed");
            return nullptr;
        }
    }
    Sequence<T>* Map(T (*func)(T&)) override{
        ListSequence<T>* raw = nullptr;
        try{
            raw = Instance();
            auto instance = std::unique_ptr<ListSequence<T>>(raw);
            raw = nullptr;
            // instance->Get(i)
            auto it = instance->list->GetEnumerator();
            int i = 0;
            while(it->MoveNext()){
                T data = it->Current();
                instance->list->Set(i, func(data));
                ++i;
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T)) override", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            delete raw;
            // HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T)) override: operation failed", "", 0);
            throw std::runtime_error("ListSequence Map: operation failed");
            return nullptr;
        }
    }
    Sequence<T>* Map(T (*func)(T&, int)) override{
        ListSequence<T>* raw = nullptr;
        try{
            raw = Instance();
            auto instance = std::unique_ptr<ListSequence<T>>(raw);
            raw = nullptr;
            // instance->Get(i)
            auto it = instance->list->GetEnumerator();
            int i = 0;
            while(it->MoveNext()){
                T data = it->Current();
                instance->list->Set(i, func(data, i+1));
                ++i;
            }
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T, int))", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            delete raw;
            // HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции Sequence<T>* Map(T (*func)(T, int)): operation failed", "", 0);
            throw std::runtime_error("ListSequence Map: operation failed");
            return nullptr;
        }
    }
    // T&& GetMove(int index) override {
    //     return this->array->GetMove(index);
    // }
    Sequence<T>* where(bool (*predicate)(T&)) override{
        ListSequence<T>* raw = nullptr;
        try{
            raw = this->Instance();
            auto instance = std::unique_ptr<ListSequence<T>>(raw);
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
            instance->list->Resize(j);
            return instance.release();
        }
        catch(std::bad_alloc){
            delete raw;
            // HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции Sequence<T> where(bool (*predicate)(T))", "", 0);
            throw std::bad_alloc();
            return nullptr;
        }
        catch(...){
            delete raw;
            //HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции Sequence<T> where(bool (*predicate)(T)): operation failed", "", 0);
            throw std::runtime_error("ListSequence where: operation failed");
            return nullptr;
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

    std::ostream& print(std::ostream& os) const override{
        os << this->GetFinalDerivedClass() << ": [";
        auto it = GetEnumerator();
        int i = -1;
        while (it->MoveNext()){
            ++i;
            os << "[ " << it->Current() << " ]";
            if (i+1 < this->GetLength()){
                os << ", ";
            }
        }
        os << "]" << std::endl;
        return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const ListSequence<T>& list){
        list.print(os);
    }

    T operator[](int index) const override{
        return this->Get(index);
    }
    // void SetCurrentNode(int index) override{
    //     list->SetCurrentNode(index);
    // }
    // void Next() override{
    //     list->Next();
    // }
    // T& GetCurrentNode() const override{
    //     return list->GetCurrentNode();
    // }
    class Enumerator : public IEnumerator<T> {
    private:
        IEnumerator<T>* link_ls;
    public:
        explicit Enumerator(const ListSequence<T>& ls) : link_ls(ls.list->GetEnumerator()){}
        ~Enumerator() override {
            delete link_ls;
        }
        void Reset() override{
            link_ls->Reset();
        }
        bool MoveNext() override{
            return link_ls->MoveNext();
        }
        T Current() override{
            return link_ls->Current();
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
//         auto result = std::make_unique<ListSequence<std::tuple<T1, T2>>>();
//         for (int i = 0; i < minSize; ++i){
//             result->Append(std::make_tuple(seq1->Get(i), seq2->Get(i)));
//         }
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2): operation failed", "", 0);
//         return nullptr;
//     }
// }
// template <typename T1, typename T2, typename T3>
// Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>* seq1, Sequence<T2>* seq2, Sequence<T3>* seq3){
//     try{
//         int minSize = std::min({seq1->GetLength(), seq2->GetLength(), seq3->GetLength()});
//         auto result = std::make_unique<ListSequence<std::tuple<T1, T2, T3>>>();
//         for (int i = 0; i < minSize; ++i){
//             result->Append(std::make_tuple(seq1->Get(i), seq2->Get(i), seq3->Get(i)));
//         }
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*): operation failed", "", 0);
//         return nullptr;
//     }
// }
// template <typename... seqs>
// Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences){
//     try{
//         int minSize = std::min({sequences->GetLength()...});
//         auto result = std::make_unique<ListSequence<std::tuple<typename seqs::ValueType...>>>();
//         for (int i = 0; i < minSize; ++i){
//             result->Append(std::make_tuple(sequences->Get(i)...));
//         }
//         return result.release();
//     }
//     catch(std::bad_alloc){
//         HandleError(MemoryAllocationFailed, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences)", "", 0);
//         return nullptr;
//     }
//     catch(...){
//         HandleError(ErrorUnknown, "class ListSequence : public Sequence<T> в функции friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences): operation failed", "", 0);
//         return nullptr;
//     }
// }
#endif