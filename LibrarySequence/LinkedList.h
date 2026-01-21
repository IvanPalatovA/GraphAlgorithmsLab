
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
// #include "HandleError.h"
#include <stdexcept>
#include <string>
#include "numerable.h"
//реализзовать DynamicArray(const std::vector<T>& vec) : DynamicArray(vec.data(), vec.size()) {}

// HandleError убран, заменён на throw
template <class T>
class LinkedList{
private:
    struct Node{
        T data;
        Node* next;
        Node* prev;
        Node(T item, Node* left = nullptr, Node* right = nullptr): data(item), next(right), prev(left){}
        ~Node(){
            next = nullptr;
            prev = nullptr;
        }
    };
    Node* head;
    Node* tail;

    //for conveyer
    Node* currentNode;

    int length;
    Node * GetNode(int index) const{
        if (index >= length){
            //std::cerr << "IndexOutOfRange\n";
            // HandleError(IndexOutOfRange, "class LinkedList в функции   Node * GetNode(size_t index) const", "", 0);
            throw std::out_of_range("LinkedList::GetNode: Index " + std::to_string(index) + " is out of range [0, " + std::to_string(length-1) + "]");
        }
        Node* curr;
        if (length/2 < index+1){
            curr = head;
            for (int i = 0; i < index; ++i){
                curr = curr->next;
            }
        }
        else{
            curr = tail;
            for (int i = length-1; i>index; --i){
                curr = curr->prev;
            }
        }
        return curr;
    }
protected:
    int ValidateIndex(int index) const{
        if (index >= length || index < (-1)*length){
            // HandleError(IndexOutOfRange, "class LinkedList в функции int ValidateIndex(int index) const", "", 0);
            throw std::out_of_range("LinkedList::ValidateIndex: Index " + std::to_string(index) + " is out of range");
        }
        if (index < 0 ){
            index = index*(-1)-1;
        }
        return index;
    }
public:
    LinkedList(): head(nullptr), tail(nullptr), currentNode(nullptr), length(0){}
    LinkedList(const T* array, const int size) : head(nullptr), tail(nullptr), length(0){
        if (size<0) {
            // HandleError(IndexOutOfRange, "class LinkedList в конструкторе LinkedList(const T* array, const int size)", "", 0);
            throw std::invalid_argument("LinkedList constructor: Size cannot be negative, got: " + std::to_string(size));
        }
        for (int i = 0; i < size; i++){
            Append(array[i]);
        }
    }
    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), currentNode(nullptr), length(0){
        if (this == &other) return;
        Node *current = other.head;
        while (current != nullptr){
            Append(current->data);
            current = current->next;
        }
    }
    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), currentNode(nullptr), length(other.length){
        other.head = nullptr;
        other.tail = nullptr;
        other.currentNode = nullptr;
        other.length = 0;
    }

    LinkedList& operator=(const LinkedList& other){
        if (this == &other) return *this;

        while (head != nullptr){
            Node *temp = head->next;
            delete head;
            head = temp;
        }
        tail = nullptr;
        currentNode = nullptr;
        length = 0;

        Node *current = other.head;
        while (current != nullptr){
            Append(current->data);
            current = current->next;
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other) noexcept{
        if (this == &other) return *this;

        while (head != nullptr){
            Node *temp = head->next;
            delete head;
            head = temp;
        }
        tail = nullptr;
        currentNode = nullptr;
        length = 0;

        head = other.head;
        tail = other.tail;
        length = other.length;
        currentNode = nullptr;

        other.head = nullptr;
        other.tail = nullptr;
        other.currentNode = nullptr;
        other.length = 0;

        return *this;
    }
    ~LinkedList(){
        while (head!=nullptr){
            Node *temp = head->next;
            delete(head);
            head = temp;
        }
        tail = nullptr;
        length = 0;
    }
    T GetFirst() const{
        if (length == 0){
            //std::cerr << "IndexOutOfRange\n";
            throw std::out_of_range("LinkedList is empty");
        }
        return head->data;
    }
    T Get(int index) const{
        try{
            index = ValidateIndex(index);
            return GetNode(index)->data;
        }
        catch(std::out_of_range){
            // HandleError(IndexOutOfRange, "class LinkedList в функции  T Get(int index) const", "", 0);
            throw std::out_of_range("LinkedList::Get: Index validation failed");
        }
    }
    int GetLength() const{
        return length;
    }
    void Resize(int newSize){
        if (newSize<0){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void Resize(int newSize)", "", 0);
            throw;
        }
        if (length<newSize){
            throw;
        }
        else if (newSize<length){
            while (length> newSize){
                PopBack();
            }
        }
    }
    LinkedList<T>* GetSubList(int startIndex, int endIndex){
        //включительно
        if (startIndex <0 || endIndex >= length || startIndex>endIndex){
            // HandleError(IndexOutOfRange, "class LinkedList в функции   LinkedList<T>* GetSubList(int startIndex, int endIndex)", "", 0);
            throw std::out_of_range("LinkedList::GetSubList: Invalid indices - startIndex: " + std::to_string(startIndex) + ", endIndex: " + std::to_string(endIndex) + ", length: " + std::to_string(length));
        }
        std::unique_ptr<LinkedList<T>> newlist;
        try{
            newlist = std::make_unique<LinkedList<T>>();
            Node *current = GetNode(startIndex);
            int count = endIndex - startIndex+1;
            while (current != nullptr && count > 0){
                newlist->Append(current->data);
                current = current->next;
                count--;
        }
        }
        catch(std::bad_alloc){
            // HandleError(MemoryAllocationFailed, "class LinkedList в функции GetSubList", "", 0);
            throw std::bad_alloc();
        }
        catch(...){
            // HandleError(ErrorUnknown, "class LinkedList в функции GetSubList: operation failed", "", 0);
            throw std::runtime_error("LinkedList::GetSubList: Unknown error occurred during sublist creation");
        }
        return newlist.release();
    }
    T GetLast() const{
        if (length == 0){
            //std::cerr << "IndexOutOfRange\n";
            throw std::out_of_range("LinkedList is empty");
        }
        return tail->data;
    }
    void Append(T item){
        if (head == nullptr){
            Node* newNode = new Node(item, nullptr, nullptr);
            head = tail= newNode;
        }
        else{
            Node* newNode = new Node(item, tail, nullptr);
            tail->next = newNode;
            tail = newNode;
        }
        ++length;
    }
    void Prepend(T item){
        Node* newNode = new Node(item, nullptr, head);
        if (length == 0){
            tail = newNode;
        }
        else{
            head->prev = newNode;
        }
        head = newNode;
        ++length;
    };
    void InsertAt(T item, size_t index){
        if (index > length){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void InsertAt(T item, size_t index)", "", 0);
            throw std::out_of_range("LinkedList::InsertAt: Index " + std::to_string(index) + " is out of range [0, " + std::to_string(length) + "]");
        }
        if (index == 0){
            Prepend(item);
        } else if (index == length){
            Append(item);
        } else{
            Node *nodeindexleft = GetNode(index-1);
            if (nodeindexleft){
                Node *newNode = new Node(item, nodeindexleft, nodeindexleft->next);
                nodeindexleft->next->prev = newNode;
                nodeindexleft->next = newNode;
                length++;
            }
        }
    }
    LinkedList<T>* Concat(LinkedList<T> *list){
        try{
            if (list == nullptr){
                return this;
            }
            Node *current = list->head;
            while (current != nullptr){
                this->Append(current->data);
                current = current->next;
            }
            return this;
        }
        catch (const std::bad_alloc& e){
            // HandleError(MemoryAllocationFailed, "class LinkedList в функции LinkedList<T>* Concat(LinkedList<T> *list) const", e.what(), 0);
            throw std::bad_alloc();
        }
    }
    // const Node* GetNodeConst(size_t index) const{
    //     if (index >= length){
    //         //std::cerr << "IndexOutOfRange\n";
    //         HandleError(IndexOutOfRange, "class LinkedList в функции   Node * GetNode(size_t index) const", "", 0);
    //         return nullptr;
    //     }
    //     Node* curr = head;
    //     for (size_t i = 0; i < index; i++){
    //         curr = curr->next;
    //     }
    //     const Node* curr_const = curr;
    //     return curr_const;
    // }
    // const Node* Next(const Node* curr_const) const{
    //     return curr_const->next;
    // }

    // for speed
    void SetCurrentNode(int index){
        try{
            index = ValidateIndex(index);
            currentNode = GetNode(index);
        }
        catch(...){
            // HandleError(ErrorUnknown, "class LinkedList в функции void SetCurrentNode(int index): operation failed", "", 0);
            throw std::runtime_error("LinkedList::SetCurrentNode: Unknown error occurred while setting current node");
        }
    }
    void Next(){
        currentNode = currentNode->next;
        if (currentNode == nullptr){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void SetCurrentNode(int index): operation failed", "Текущий currentNode является tail, следующего не существует", 0);
            throw std::logic_error("LinkedList::Next: Current node is tail, no next node exists");
        }
    }
    void Prev(){
        currentNode = currentNode->prev;
        if (currentNode == nullptr){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void Prev(): operation failed", "Текущий currentNode является head, следующего не существует", 0);
            throw std::logic_error("LinkedList::Prev: Current node is head, no previous node exists");
        }
    }
    T GetCurrentNode() const{
        return currentNode->data;
    }
    // for segment deque
    T PopFront(){
        T result;
        if (length == 0){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void PopFront()", "", 0);
            throw std::logic_error("LinkedList::PopFront: Cannot pop from empty list");
        }
        else if (length == 1){
            result = head->data;
            delete head;
            head = tail = nullptr;
        }
        else{
            result = head->data;
            auto nodesecond = head->next;
            delete head;
            head = nodesecond;
            head->prev = nullptr;
        }
        --length;
        return result;
    }
    T PopBack(){
        T result;
        if (length == 0){
            // HandleError(IndexOutOfRange, "class LinkedList в функции void PopBack()", "", 0);
            throw std::logic_error("LinkedList::PopBack: Cannot pop from empty list");
        }
        else if (length == 1){
            result = tail->data;
            delete tail;
            head = tail = nullptr;
        }
        else{
            result = tail->data;
            auto nodebefore = tail->prev;
            delete tail;
            tail = nodebefore;
            tail->next = nullptr;
        }
        --length;
        return result;
    }
    void Set(int index, T value) {
        auto node = GetNode(index);
        node->data = std::move(value);
    }

    IEnumerator<T>* GetEnumerator(){
        return new Enumerator(*this);
    }

    IEnumerator<T>* GetEnumerator() const{
        return new Enumerator(*this);
    }
private:
    class Enumerator : public IEnumerator<T> {
    private:
        const LinkedList<T>& ls;
        const Node* current;
        int pos;
    public:
        Enumerator(const LinkedList<T>& LS) : ls(LS), current(ls.head), pos(-1){}
        void Reset() override{
            pos = -1;
            current = ls.head;
        }
        bool MoveNext() override{
            if (pos+1 >= ls.GetLength()){
                return false;
            }
            else{
                if (pos>-1){
                    current = current->next;
                }
                ++pos;
                return true;
            }
        }
        T Current() override{
            if (pos == -1){
                // HandleError(IndexOutOfRange, "class LinkedList в функции T& Current() override", "необходимо выполнить хотя бы одно MoveNext()", 0);
                throw std::logic_error("LinkedList::Enumerator::Current: MoveNext() must be called before accessing Current()");
            }
            return current->data;
        }
    };
};
#endif