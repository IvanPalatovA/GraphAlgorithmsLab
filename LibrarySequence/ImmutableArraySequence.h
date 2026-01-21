#ifndef IMMUTABLEARRAYSEQUENCE
#define IMMUTABLEARRAYSEQUENCE
#include "ArraySequence.h"
template <class T>
class ImmutableArraySequence : public ArraySequence<T>{
protected:
    ArraySequence<T>* Clone(){
        return new ImmutableArraySequence(*this);
    }
    ArraySequence<T>* Instance() override{
        return Clone();
    }
    // const ArraySequence<T>* Instance() const{
    //     return Clone();
    // }
    template <typename U>
    Sequence<U>* CreateSequenceType() const{
        return new ImmutableArraySequence<U>();
    }
public:
    using ArraySequence<T>::ArraySequence;
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequence<T>( *other.array){};
    std::string GetFinalDerivedClass() const override{
        return "ImmutableArraySequence";
    }
};
#endif