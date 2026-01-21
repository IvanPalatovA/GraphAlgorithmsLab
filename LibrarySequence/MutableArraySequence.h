#ifndef MUTABLEARRAYSEQUENCE
#define MUTABLEARRAYSEQUENCE
#include "ArraySequence.h"
template <class T>
class MutableArraySequence : public ArraySequence<T>{
protected:
    // ListSequence<T>* CreateNew() override{
    //     return *;
    // }
    ArraySequence<T>* Instance() override{
        return this;
    }
    // const ArraySequence<T>* Instance() const override{
    //     return this;
    // }
    template <typename U>
    Sequence<U>* CreateSequenceType() const{
        return new MutableArraySequence<U>();
    }
public:
    using ArraySequence<T>::ArraySequence;
    MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequence<T>( *other.array ){};
    std::string GetFinalDerivedClass() const override{
        return "MutableArraySequence";
    }
};
#endif