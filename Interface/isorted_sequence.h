#pragma once

#include <cstddef>

// Чисто виртуальный интерфейс отсортированной последовательности.
// Предполагается, что элементы сравнимы операторами == и <.
template <typename TElement>
class ISortedSequence {
public:
    using ValueType = TElement;

    virtual ~ISortedSequence() = default;

    // Количество элементов.
    virtual int GetLength() const = 0;

    // Пустая ли последовательность.
    virtual bool IsEmpty() const = 0;

    // Получение элемента по индексу.
    // Должен выбросить std::out_of_range при неверном индексе.
    virtual TElement Get(int index) const = 0;

    // Первый элемент (минимальный).
    virtual TElement GetFirst() const = 0;

    // Последний элемент (максимальный).
    virtual TElement GetLast() const = 0;

    // Индекс элемента (или -1, если не найден).
    virtual int IndexOf(const TElement& element) const = 0;

    // Добавление элемента с сохранением упорядоченности.
    virtual void Add(const TElement& element) = 0;

    // Вспомогательный метод в стиле исходной реализации.
    int GetIsEmpty() const { return IsEmpty() ? 1 : 0; }
};


