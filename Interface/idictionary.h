#pragma once

#include <stdexcept>

// Обобщённый интерфейс ассоциативного массива (словаря).
// Ключи предполагаются сравнимыми (операторы == и <) и, для хеш-таблицы, хешируемыми.
template <typename K, typename V>
class IDictionary {
public:
    virtual ~IDictionary() = default;

    // Количество реально хранимых элементов
    virtual std::size_t Count() const = 0;

    // Вместимость (актуально в первую очередь для хеш-таблицы)
    virtual std::size_t Capacity() const = 0;

    // Проверка наличия ключа
    virtual bool ContainsKey(const K& key) const = 0;

    // Получение значения по ключу (по значению).
    // Если ключ отсутствует, выбрасывается std::out_of_range.
    virtual V Get(const K& key) const = 0;

    // Добавление нового элемента.
    // Если ключ уже есть, метод должен выбросить исключение.
    virtual void Add(const K& key, const V& value) = 0;

    // Добавление или обновление элемента.
    // Если ключ уже есть, значение перезаписывается.
    virtual void AddOrUpdate(const K& key, const V& value) = 0;

    // Удаление элемента по ключу.
    // Должен выбросить исключение, если ключ отсутствует.
    virtual void Remove(const K& key) = 0;

    // Вспомогательные методы-обёртки с именами из спецификации.
    int GetCount() const { return static_cast<int>(Count()); }
    int GetCapacity() const { return static_cast<int>(Capacity()); }
};



