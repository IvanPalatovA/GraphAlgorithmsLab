#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
// #include "HandleError.h"
#include <stdexcept>
#include <string>
#include <vector>
#include "numerable.h"

// HandleError убран, заменён на throw
template <class T>
class DynamicArray{
private:
    T* data;
    int length;
    int capacity;
protected:
    int ValidateIndex(int index) const{
        if (index < 0 ){
            index = index*(-1)-1;
        }
        if (index >=length){
            // HandleError(IndexOutOfRange, "class DynamicArray в функции int ValidateIndex(int index) const", "index >=length", 0);
            throw std::out_of_range("DynamicArray::ValidateIndex: Index " + std::to_string(index) + " is out of range [0, " + std::to_string(length-1) + "]");
        }
        return index;
    }
public:
    DynamicArray(const std::vector<T>& vec) : DynamicArray(vec.data(), vec.size()) {}
    DynamicArray(const T* items, int count) : length(count), capacity(count) {
        try{
            data = new T[length];
        }
        catch (const std::bad_alloc& e){
            // HandleError(MemoryAllocationFailed, "class DynamicArray в конструкторе DynamicArray(const T* items, int count)", e.what(), 0);
            throw std::bad_alloc();
        }
        catch (...){
            // HandleError(ErrorUnknown, "class DynamicArray в конструкторе DynamicArray(const T* items, int count)", "неизвестная ошибка", 0);
            throw std::runtime_error("DynamicArray constructor: Unknown error occurred while creating array from items");
        }
        for (int i = 0; i < length; ++i){
            data[i] = items[i];
        }
    }
    explicit DynamicArray(int size){
        if (size<0){
            // HandleError(NegativeSize, "class DynamicArray в конструкторе DynamicArray(int size)", "", 0);
            throw std::invalid_argument("DynamicArray constructor: Size cannot be negative, got: " + std::to_string(size));
        }
        try{
            if (size == 0){
                data = nullptr;
            }
            else{
                data = new T[size];  
            }
            length = size;
            capacity = size;
        }
        catch (const std::bad_alloc& e){
            // HandleError(MemoryAllocationFailed, "class DynamicArray в конструкторе DynamicArray(int size)", e.what(), 0);
            throw std::bad_alloc();
        }
    }
    explicit DynamicArray(): DynamicArray(0){}
    DynamicArray(const DynamicArray<T> & dynamicArray){
        try{
            data = new T[dynamicArray.length];
            length = dynamicArray.length;
            capacity = length;
            for (int i = 0; i < length; i++){data[i] = dynamicArray.data[i];}
        }
        catch (const std::bad_alloc& e){
            // HandleError(MemoryAllocationFailed, "class DynamicArray в конструкторе DynamicArray(const DynamicArray<T> & dynamicArray)", e.what(), 0);
            throw std::bad_alloc();
        }
        catch (...){
            // HandleError(ErrorUnknown, "class DynamicArray в конструкторе DynamicArray(const DynamicArray<T> & dynamicArray)", "", 0);
            throw std::runtime_error("DynamicArray copy constructor: Unknown error occurred while copying array");
        }
    }
    DynamicArray(DynamicArray<T>&& other) noexcept: data(other.data), length(other.length), capacity(other.capacity){
        other.data = nullptr;
        other.length = 0;
        other.capacity = 0;
    }
    ~DynamicArray(){
        delete[] data;
        length = 0;
    }
    
    DynamicArray<T>& operator=(DynamicArray<T> other){
        // что лучше CopyAndSwap или текущее решение?
        // if (this != &other){
        //     T* new_data = new T[other.data];
        //     length = other.length;
        //     delete[] data;
        //     data = new_data;
        // }
        // return *this;
        swap(*this, other);
        return *this;
    }
    friend void swap(DynamicArray<T>& a, DynamicArray<T>& b) noexcept{
        std::swap(a.data, b.data);
        std::swap(a.length, b.length);
    }
    // friend void move_element(DynamicArray& src, int src_idx, DynamicArray& dst, int dst_idx) {
    //     src_idx = src.ValidateIndex(src_idx);
    //     dst_idx = dst.ValidateIndex(dst_idx);
    //     dst.data[dst_idx] = std::move(src.data[src_idx]);
    // }

    //  изменил теперь без ссылки
    T Get(int index) const{
        index = ValidateIndex(index);
        return data[index];
    }
    int GetSize() const {
        return length;
    }
    void Set(int index, T value){
        index = ValidateIndex(index);
        data[index] = std::move(value);
    }
    void Resize(int newSize){
        if (newSize<0){
            // HandleError(IndexOutOfRange, "class DynamicArray в функции void Resize(int newSize)", "", 0);
            throw std::invalid_argument("DynamicArray::Resize: Size cannot be negative, got: " + std::to_string(newSize));
        }
        // std::unique_ptr<T[]> old_data(data);
        // const int old_length = length;
        std::unique_ptr<T[]> newdata;
        try{
            if (capacity<newSize || newSize<length){
                newdata = std::make_unique<T[]>(newSize);
                const int minn = std::min(length, newSize);
                for (int i = 0; i < minn; ++i){
                    newdata[i] = std::move_if_noexcept(data[i]);
                }
                delete[] data;
                data = newdata.release();
                length = newSize;
                capacity = newSize;
            }
            else{
                length = newSize;
            }
        }
        catch (const std::bad_alloc& e){
            // HandleError(MemoryAllocationFailed, "class DynamicArray в классе void Resize(int newSize)", e.what(), 0);
            throw std::bad_alloc();
        }
        catch (...){
            // data = old_data.release();
            // length = old_length;
            // HandleError(ErrorUnknown, "class DynamicArray в классе void Resize(int newSize)", "", 0);
            throw std::runtime_error("DynamicArray::Resize: Unknown error occurred during resize operation");
        }
    }
    T&& GetMove(int index) {
        index = ValidateIndex(index);
        return std::move(data[index]); // Явное перемещение
    }
    class Enumerator : public IEnumerator<T> {
    private:
        DynamicArray<T>& arr;
        int pos;
    public:
        Enumerator(DynamicArray<T>& a) : arr(a), pos(-1){}
        void Reset() override{
            pos = -1;
        }
        bool MoveNext() override{
            if (pos+1 >=arr.GetSize()){
                return false;
            }
            else{
                ++pos;
                return true;
            }
        }
        T Current() override{
            if (pos<0){
                // HandleError(IndexOutOfRange, "class DynamicArray в функции T& Current() override", "необходимо выполнить хотя бы одно MoveNext()", 0);
                throw std::logic_error("DynamicArray::Enumerator::Current: MoveNext() must be called before accessing Current()");
            }
            return arr.Get(pos);
        }
    };
    IEnumerator<T>* GetEnumerator(){
        return new Enumerator(*this);
    }
};
#endif

