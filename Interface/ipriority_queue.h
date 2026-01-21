#pragma once

#include <cstddef>

// Чисто виртуальный интерфейс очереди с приоритетами.
// Соответствует требованиям к PriorityQueue:
//  - Peek(i), PeekFirst, PeekLast
//  - Enqueue(item, priority)
//  - Dequeue()
//
// Для общности разделены тип элемента (TElement) и тип приоритета (TPriority).
// Чем МЕНЬШЕ значение приоритета, тем ВЫШЕ приоритет элемента.
template <typename TElement, typename TPriority>
class IPriorityQueue {
public:
    using ValueType = TElement;
    using PriorityType = TPriority;

    virtual ~IPriorityQueue() = default;

    // Очередь пуста?
    virtual bool IsEmpty() const = 0;

    // Количество элементов в очереди.
    virtual int GetSize() const = 0;

    // Peek: получить элемент по индексу в текущем представлении очереди.
    // При некорректном индексе должно выбрасываться std::out_of_range.
    virtual TElement Peek(int index) const = 0;

    // Первый элемент (наивысший приоритет).
    // При пустой очереди должно выбрасываться std::out_of_range.
    virtual TElement PeekFirst() const = 0;

    // Последний элемент (наименьший приоритет).
    // При пустой очереди должно выбрасываться std::out_of_range.
    virtual TElement PeekLast() const = 0;

    // Добавить элемент в очередь с указанным приоритетом.
    virtual void Enqueue(const TElement& item, const TPriority& priority) = 0;

    // Достать первый элемент (с наивысшим приоритетом) и удалить его из очереди.
    // При пустой очереди должно выбрасываться std::out_of_range.
    virtual TElement Dequeue() = 0;
};


