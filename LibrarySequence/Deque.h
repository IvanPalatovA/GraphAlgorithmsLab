#ifndef DEQUE_H
#define DEQUE_H
#include "MutableListSequence.h"
#include "MutableArraySequence.h"
// #include "HandleError.h"
#include "numerable.h"
// поменял HandleError на throw

template <typename T>
class SegmentedDeque : public Sequence<T>{
private:
    const int SegSize;
    MutableListSequence<MutableArraySequence<T>*>* list;
public:
    int GetLength() const override{
        int sizee = list->GetLength();
        if (sizee == 0){
            return 0;
        }
        else if (sizee == 1){
            return list->GetFirst()->GetLength();
        }
        else{
            return SegSize*(sizee-2)+list->GetFirst()->GetLength()+list->GetLast()->GetLength();
        }
    }
    T GetLast() const override{
        return list->GetLast()->GetLast();
    }
    T GetFirst() const override{
        return list->GetFirst()->GetFirst();
    }
    explicit SegmentedDeque(int SegmentSize) : SegSize(SegmentSize){
        if (SegSize<=0){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в конструкторе SegmentedDeque(int segSize)", "SegSize должен быть >0", 0);
            throw std::invalid_argument("segSize должен быть > 0");
        }
        list = new MutableListSequence<MutableArraySequence<T>*>();
        list->Append(new MutableArraySequence<T>(0));
    }
    SegmentedDeque(Sequence<T>* seq, int SegmentSize) : SegSize(SegmentSize){
        if (SegSize<=0){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в конструкторе SegmentedDeque(Sequence<T>* seq, int SegmentSize) : SegSize(SegmentSize)", "SegSize должен быть >0", 0);
            throw std::invalid_argument("segSize должен быть > 0");
        }
        else if (seq == nullptr){
            // HandleError(IncorrectParametrs, "class SegmentedDeque в конструкторе SegmentedDeque(Sequence<T>* seq, int SegmentSize) : SegSize(SegmentSize)", "передан нулевой Sequence", 0);
            throw std::invalid_argument("Sequence должен быть не nullptr");
        }
        auto it = seq->GetEnumerator();
        while (it->MoveNext()){
            this->Append(it->Current());
        }
        delete it;
    }
    ~SegmentedDeque(){
        int len = list->GetLength();
        for (int i = 0; i < len; ++i){
            delete list->PopBack();
        }
        delete list;
    }
    Sequence<T>* Append(T value) override{
        if (list->GetLast()->GetLength() >= SegSize){
            list->Append(new MutableArraySequence<T>(0));
        }
        list->GetLast()->Append(value);
        return this;
    }
    Sequence<T>* Prepend(T value) override{
        if (list->GetFirst()->GetLength() >= SegSize){
            list->Prepend(new MutableArraySequence<T>(0));
        }
        list->GetFirst()->Prepend(value);
        return this;
    }
    T PopFront() override{
        if (list->GetLength()== 0){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции T PopFront()", "нечего удалять, дэк пуст", 0);
            std::out_of_range("deque is empty");
        }
        T temp = GetFirst();
        if (list->GetFirst()->GetLength() == 1){
            delete list->PopFront();
        }
        else{
            list->GetFirst()->PopFront();
        }
        return temp;
    }
    T PopBack() override{
        if (list->GetLength()== 0){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции T PopBack()", "нечего удалять, дэк пуст", 0);
            std::out_of_range("deque is empty");
        }
        auto temp = GetLast();
        if (list->GetLast()->GetLength() == 1){
            delete list->PopBack();
        }
        else{
            list->GetLast()->PopBack();
        }
        return temp;
    }
    T Get(int index) const override{
        if (index<0 || index >= GetLength()){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции T& Get(int index)", "неверно задан индекс", 0);
            throw std::out_of_range("SegmentedDeque: Get index out of range");
        }
        int rem = index;
        auto it = list->GetEnumerator();
        while (it->MoveNext()) {
            auto segment = it->Current();
            int segLen = segment->GetLength();
            if (rem < segLen) {
                delete it;
                return segment->Get(rem);
            }
            rem -= segLen;
        }
        // сюда никогда не должны попасть (по идее)
        delete it;
        throw std::out_of_range("SegmentedDeque: Get index out of range");
        
    }
    void Set(int index, T value) override{
        if (index<0 || index >= GetLength()){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции void Set(int index, T value)", "неверно задан индекс", 0);
            throw std::out_of_range("SegmentedDeque: Get index out of range");
        }
        int rem = index;
        auto it = list->GetEnumerator();
        while (it->MoveNext()) {
            auto segment = it->Current();
            int segLen = segment->GetLength();
            if (rem < segLen) {
                segment->Set(rem, value);
                delete it;
                return;
            }
            rem -= segLen;
        }
        // сюда никогда не должны попасть (по идее)
        delete it;
        throw std::out_of_range("SegmentedDeque: Get index out of range");
    }
    //включительно []
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override{
        if (startIndex <0 || endIndex >= GetLength() || startIndex>endIndex){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции Sequence<T>* GetSubsequence(int startIndex, int endIndex) override", "", 0);
            throw std::out_of_range("SegmentedDeque GetSubsequence: invalid range");
        }
        auto newSequence = new SegmentedDeque<T>(SegSize);
        auto it = GetEnumerator();
        int i = 0;
        while (it->MoveNext()){
            if (i>=startIndex && i<=endIndex){
                newSequence->Append(it->Current());
            }
            ++i;
        }
        delete it;
        return newSequence;
    }
    Sequence<T>* Concat(Sequence<T>* deque) override{
        auto it = deque->GetEnumerator();
        while (it->MoveNext()){
            Append(it->Current());
        }
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override{
        if (index<0 || index > GetLength()){
            // HandleError(IndexOutOfRange, "class SegmentedDeque в функции Sequence<T>* InsertAt(T item, int index)", "неверно задан индекс", 0);
            throw std::out_of_range("SegmentedDeque InsertAt: index out of range");
        }
        Append(0);

        T&& move_elem = std::move(item), temp;
        auto it_ls = list->GetEnumerator();
        IEnumerator<T>* it_arr;

        //move_Next следующий - правильный
        int startList = 0;
        int sum = 0;
        while (sum+SegSize<index+1){
            ++startList;
            it_ls->MoveNext();
            sum+=it_ls->Current()->GetLength();
        }
        bool isFirst = true;
        while (it_ls->MoveNext()){
            int start_j = 0;
            //1-ый проход
            if (isFirst){
                //move_Next следующий - правильный
                it_arr = it_ls->Current()->GetEnumerator();
                while (sum+1 < index+1){
                    ++start_j;
                    ++sum;
                    it_arr->MoveNext();
                }
            }
            //> 1 прохода
            else{
                it_arr = it_ls->Current()->GetEnumerator();
            }
            isFirst = false;
            while (it_arr->MoveNext()){
                temp = std::move(it_arr->Current());
                it_ls->Current()->Set(start_j, std::move(move_elem));
                move_elem = std::move(temp);
                ++start_j;
            }
        }
        delete it_ls;
        delete it_arr;
        return this;
    }
    class Enumerator : public IEnumerator<T>{
    private:
        const SegmentedDeque<T>& dq;
        IEnumerator<MutableArraySequence<T>*>* ls_it;
        IEnumerator<T>* seg_it;
        int pos;

    public:
        Enumerator(const SegmentedDeque<T>& DQ) : dq(DQ), ls_it(dq.list->GetEnumerator()), seg_it(dq.list->GetFirst()->GetEnumerator()), pos(-1){};
        ~Enumerator(){
            delete ls_it;
            delete seg_it;
        }
        bool MoveNext() override{
            if (pos+1 >=dq.GetLength()){
                return false;
            }
            else if (pos == -1){
                ls_it->MoveNext();
                seg_it->MoveNext();
            }
            else if (!seg_it->MoveNext()){
                ls_it->MoveNext();
                delete seg_it;
                seg_it = ls_it->Current()->GetEnumerator();
                seg_it->MoveNext();
            }
            ++pos;
            return true;
        }
        void Reset() override{
            ls_it->Reset();
            delete seg_it;
            seg_it = dq.list->GetFirst()->GetEnumerator();
            pos = -1;
        }
        T Current() override{
            return seg_it->Current();
        }
        // void Change(T value) override{
        //     // ls_it->Current()->Set();
        // }

    };
    IEnumerator<T>* GetEnumerator() const override{
        return new Enumerator(*this);
    }

    T operator[](int index) const override{
        return Get(index);
    }
    Sequence<T>* Map(T (*func)(T&)) override{
        auto it = list->GetEnumerator();
        while(it->MoveNext()){
            auto segment = it->Current();
            for (int i = 0; i < segment->GetLength(); ++i){
                T data = segment->Get(i);
                segment->Set(i, func(data));
            }
        }
        return this;
    }
    virtual Sequence<T>* Map(T (*func)(T&, int)) override {
        auto it = list->GetEnumerator();
        int count = 0;
        while(it->MoveNext()){
            auto segment = it->Current();
            for (int i = 0; i < segment->GetLength(); ++i){
                ++count;
                T data = segment->Get(i);
                segment->Set(i, func(data, count));
            }
        }
        return this;
    }
    Sequence<T>* where(bool (*predicate)(T&)) override{
        auto* SD = new SegmentedDeque<T>(SegSize);
        auto it = GetEnumerator();
        while(it->MoveNext()){
            T data = it->Current();
            if (predicate(data)){
                SD->Append(data);
            }
        }
        return SD;
    }
    T Reduce(T (*func)(T, T), T init) override {
        auto it = GetEnumerator();
        while(it->MoveNext()){
            init = func(it->Current(), init);
        }
        return init;
    }
    std::ostream& print(std::ostream& os) const override{
        os << "SegmentedDeque: [";
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
    friend std::ostream& operator<<(std::ostream& os, const SegmentedDeque& seg){
        seg.print(os);
    }
};
#endif