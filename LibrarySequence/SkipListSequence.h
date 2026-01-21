#pragma once
#include <random>
#include <vector>
#include <limits>
#include "numerable.h"
#include "Sequence.h"
// #include "HandleError.h"

// поменял HandleError на throw

static constexpr int MAX_LEVEL = 16;

template<typename T>
class SkipListNode {
public:
    T value;
    std::vector<SkipListNode*> forward; // массив всех указателей на следующие элементы
    std::vector<int> span; // число пропусков на кажджом уровне

    SkipListNode(int level, const T& val)
        : value(val), forward(level + 1, nullptr), span(level + 1, 0) {}
};

template<typename T>
class SkipList {
private:
    SkipListNode<T>* header;
    // текущий максимальный level (не больше MAX_LEVEL)
    int level;
    int length;
    // генератор случайных чисел + зерно
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    int randomLevel() {
        int lvl = 0;
        while (dis(gen) < 0.5 && lvl < MAX_LEVEL) {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList()
        : level(0), length(0), gen(std::random_device{}()), dis(0.0, 1.0) {
        // для каждого уровня - первый фиктивный узел (вертикально)
        header = new SkipListNode<T>(MAX_LEVEL, T());
        for (int i = 0; i <= MAX_LEVEL; ++i)
            // о пропуска
            header->span[i] = 0;
    }

    ~SkipList() {
        auto node = header;
        while (node) {
            auto next = node->forward[0];
            delete node;
            node = next;
        }
    }

    int Size() const { return length; }

    void Insert(int index, const T& value) {
        if (index < 0 || index > length) throw std::out_of_range("SkipList Insert: index out of range");
        //чтобы индексировать уровни от 0 до MAX_LEVEL
        // хранения узлов, ссылки которых нужно будет обновить после вставки
        std::vector<SkipListNode<T>*> update(MAX_LEVEL + 1);
        // сколько узлов пропустили, пока дошли до нужной
        std::vector<int> rank(MAX_LEVEL + 1);
        auto x = header;
        // логарифмическое нахождение всех 
        for (int i = level; i >= 0; --i) {
            //  первый - ноль, а остальные - все пропущенные ноды перед newNode
            rank[i] = (i == level ? 0 : rank[i+1]);
            while (x->forward[i] && rank[i] + x->span[i] < index + 1) {
                rank[i] += x->span[i];
                x = x->forward[i];
            }
            // добавляем ноду перед newNode на каждом уровне
            update[i] = x;
        }
        int lvl = randomLevel();
        if (lvl > level) {
            for (int i = level + 1; i <= lvl; ++i) {
                // для новых уровней пока только header 
                update[i] = header;
                // пропуск всех элементов на новом уровне
                update[i]->span[i] = length;
            }
            level = lvl;
        }
        auto newNode = new SkipListNode<T>(lvl, value);
        // обновление данных для newNode и update (предыдущая нода)
        for (int i = 0; i <= lvl; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
            // обновление пропущенных
            newNode->span[i] = update[i]->span[i] - (index - rank[i]);
            update[i]->span[i] = (index - rank[i]) + 1;
        }
        for (int i = lvl+1; i <= level; ++i) {
            update[i]->span[i]++;
        }
        length++;
    }

    T Erase(int index) {
        if (index < 0 || index >= length) throw std::out_of_range("SkipList Erase: index out of range");
        std::vector<SkipListNode<T>*> update(MAX_LEVEL + 1);
        auto x = header;
        int traversed = 0;
        for (int i = level; i >= 0; --i) {
            // если есть следующая нода
            while (x->forward[i] && traversed + x->span[i] - 1 < index) {
                traversed += x->span[i];
                x = x->forward[i];
            }
            // следующая после update - x
            update[i] = x;
        }
        auto target = x->forward[0];
        T val = target->value;
        for (int i = 0; i <= level; ++i) {
            // обновление параметров
            if (update[i]->forward[i] == target) {
                update[i]->span[i] += target->span[i] - 1;
                update[i]->forward[i] = target->forward[i];
            } else {
                update[i]->span[i]--;
            }
        }
        delete target;
        while (level > 0 && header->forward[level] == nullptr) {
            level--;
        }
        length--;
        return val;
    }
    // узнать значение по индексу
    T& At(int index) const {
        if (index < 0 || index >= length) throw std::out_of_range("SkipList At: index out of range");
        auto x = header;
        int traversed = 0;
        for (int i = level; i >= 0; --i) {
            while (x->forward[i] && traversed + x->span[i] - 1 < index) {
                traversed += x->span[i];
                x = x->forward[i];
            }
        }
        return x->forward[0]->value;
    }
};


template<typename T>
class SkipListSequence : public Sequence<T> {
private:
    SkipList<T> list;

public:
    SkipListSequence() = default;
    virtual ~SkipListSequence() = default;

    T GetFirst() const override { return list.At(0); }
    T GetLast() const override { return list.At(list.Size() - 1); }
    T Get(int index) const override { return list.At(index); }
    void Set(int index, T value) override {
        // Replace: erase then insert
        list.Erase(index);
        list.Insert(index, value);
    }
    // immutable (создается новый)
    Sequence<T>* GetSubsequence(int start, int end) override {
        if (start<0 || start > end || end >= GetLength()){
            throw std::out_of_range("SkipListSequence GetSubsequence: invalid range");
        }
        auto* seq = new SkipListSequence<T>();
        for (int i = start; i <= end; ++i)
            seq->Append(list.At(i));
        return seq;
    }

    int GetLength() const override { return list.Size(); }

    Sequence<T>* Append(T item) override {
        list.Insert(list.Size(), item);
        return this;
    }

    Sequence<T>* Prepend(T item) override {
        list.Insert(0, item);
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        list.Insert(index, item);
        return this;
    }

    Sequence<T>* Concat(Sequence<T>* other) override {
        for (int i = 0; i < other->GetLength(); ++i)
            Append(other->Get(i));
        return this;
    }

    T operator[](int index) const override {
        return Get(index);
    }

    // возвращает новый SkipListSequence
    Sequence<T>* Map(T (*func)(T&)) override {
        auto* seq = new SkipListSequence<T>();
        for (int i = 0; i < GetLength(); ++i) {
            T tmp = Get(i);
            seq->Append(func(tmp));
        }
        return seq;
    }
    Sequence<T>* Map(T (*func)(T&, int)) override {
        auto* seq = new SkipListSequence<T>();
        for (int i = 0; i < GetLength(); ++i) {
            T tmp = Get(i);
            seq->Append(func(tmp, i+1));
        }
        return seq;
    }

    // создается новый
    Sequence<T>* where(bool (*pred)(T&)) override {
        auto* seq = new SkipListSequence<T>();
        for (int i = 0; i < GetLength(); ++i) {
            T tmp = Get(i);
            if (pred(tmp)) seq->Append(tmp);
        }
        return seq;
    } 

    T Reduce(T (*func)(T, T), T init) override {
        T acc = init;
        for (int i = 0; i < GetLength(); ++i)
            acc = func(acc, Get(i));
        return acc;
    }

    std::ostream& print(std::ostream& os) const override {
        os << "[";
        for (int i = 0; i < GetLength(); ++i) {
            os << Get(i);
            if (i + 1 < GetLength()) os << ", ";
        }
        os << "]" << "\n";
        return os;
    }

    T PopFront() override { return list.Erase(0); }
    T PopBack() override { return list.Erase(list.Size() - 1); }

    class SkipListEnumerator : public IEnumerator<T> {
    private:
        const SkipList<T>& list;
        int index, len;
    public:
        SkipListEnumerator(const SkipList<T>& lst)
        : list(lst), index(-1), len(lst.Size()) {}
        bool MoveNext() override {
            if (index + 1 < len) {
                ++index;
                return true;
            }
            return false;
        }
        T Current() override {
            return list.At(index);
        }
        void Reset() override {
            index = -1;
        }
    };

    IEnumerator<T>* GetEnumerator() const override {
        return new SkipListEnumerator(list);
    }
};