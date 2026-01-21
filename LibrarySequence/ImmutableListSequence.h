#ifndef IMMUTABLELISTSEQUENCE
#define IMMUTABLELISTSEQUENCE
#include "ListSequence.h"
template <class T>
class ImmutableListSequence : public ListSequence<T>{
protected:
    ListSequence<T>* Instance() override{
        return Clone();
    }
    const ListSequence<T>* Instance() const override{
        // return new ImmutableListSequence<T>(*this);
        return Clone();
    }
    ListSequence<T>* CreateNew(LinkedList<T>&& list) const override{
        // if (!list) return nullptr;
        return new ImmutableListSequence(list);
    }
    ListSequence<T>* Clone() const{
        return new ImmutableListSequence(*this);
    }
    template <typename U>
    Sequence<T>* CreateSequenceType() const{
        return new ImmutableListSequence<U>();
    }
    std::string GetFinalDerivedClass() const override{
        return "ImmutableListSequence";
    }
public:
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequence<T>(*other.list){
    };
    using ListSequence<T>::ListSequence;
};
#endif