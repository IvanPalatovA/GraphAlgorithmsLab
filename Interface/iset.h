#pragma once

#include <cstddef>

// Чисто виртуальный интерфейс множества.
// Элементы типа TElement предполагаются сравнимыми по оператору == и <.
template <typename TElement>
class ISet {
public:
    using ValueType = TElement;

    virtual ~ISet() = default;

    // Количество элементов в множестве.
    virtual std::size_t Count() const = 0;

    // Пусто ли множество.
    virtual bool IsEmpty() const = 0;

    // Содержится ли элемент во множестве.
    virtual bool Contains(const TElement& value) const = 0;

    // Добавить элемент (если такого ещё нет).
    virtual void Add(const TElement& value) = 0;

    // Удалить элемент. Может выбросить исключение, если элемента нет.
    virtual void Remove(const TElement& value) = 0;
};


