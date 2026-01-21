#ifndef SEQUENCE_H
#define SEQUENCE_H
// #include "HandleError.h"
#include "numerable.h"
template<class T>
class Sequence{
protected:
    using ValueType = T;

public:
    virtual ~Sequence() = default;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual void Set(int index, T value) = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) = 0;
    virtual int GetLength () const = 0;
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;
    virtual T operator[](int index) const = 0;
    virtual Sequence<T>* Map(T (*func)(T&)) = 0;
    // Sequence<T>::Map(void (*func)(<тип ошибки>, int)) [с T=int]
    virtual Sequence<T>* Map(T (*func)(T&, int)) = 0;
    // virtual Sequence<T>* Map(T (*func)(T)) const;
    // virtual Sequence<T>* Map(T (*func)(T, int)) const;
    virtual Sequence<T>* where(bool (*predicate)(T&)) = 0;
    virtual T Reduce(T (*func)(T, T), T init) = 0;
    virtual std::ostream& print(std::ostream& os) const = 0;
    virtual T PopFront() = 0;
    virtual T PopBack() = 0;
    // virtual void SetCurrentNode(int index) = default;
    // virtual void Next() = default;
    // virtual T& GetCurrentNode() const = default;
    virtual IEnumerator<T>* GetEnumerator() const = 0;
    // template <typename U>
    //CHANGE!!!
    // Sequence<U>* CreateSequence() const{
    //     return static_cast<const Derived<U>*>(this)->CreateSequenceType();
    // }
    // virtual CreateSequenceType() const = 0;
    // template <typename T1, typename T2>
    // virtual friend Sequence<std::tuple<T1, T2>>* Zip(Sequence<T1>*, Sequence<T2>*) = 0;

    // template <typename T1, typename T2, typename T3>
    // virtual friend Sequence<std::tuple<T1, T2, T3>>* Zip3(Sequence<T1>*, Sequence<T2>*, Sequence<T3>*) = 0;

    // template <typename... seqs>
    // virtual friend Sequence<std::tuple<typename seqs::ValueType...>>* ZipN(seqs*... sequences) = 0;

};
template<typename T>
    std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
        return seq.print(os);
    }
#endif