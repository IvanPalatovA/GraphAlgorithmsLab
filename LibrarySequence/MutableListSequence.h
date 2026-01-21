#ifndef MUTABLELISTSEQUENCE
#define MUTABLELISTSEQUENCE
#include "ListSequence.h"
template <class T>
class MutableListSequence : public ListSequence<T>{
protected:
    ListSequence<T>* Instance() override{
        return this;
    }
    const ListSequence<T>* Instance() const override{
        return this;
    }
    ListSequence<T>* CreateNew(LinkedList<T>&& list) const override{
        // if (!list) return nullptr;
        return new MutableListSequence(list);
    }
    template <typename U>
    Sequence<T>* CreateSequenceType() const{
        return new MutableListSequence<U>();
    }
    std::string GetFinalDerivedClass() const override{
        return "MutableListSequence";
    }
public:
    using ListSequence<T>::ListSequence;
    MutableListSequence(const MutableListSequence<T>& other) : ListSequence<T>( *other.list ){};
};
#endif