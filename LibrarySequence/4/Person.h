#pragma once
#include <string>
#include <ctime>

/**
 * Класс Person для тестирования
 */
class Person {
private:
    int id;
    std::string firstName;
    std::string lastName;
    std::time_t birthDate;
public:
    Person(int _id, const std::string& fn, const std::string& ln, std::time_t bd)
        : id(_id), firstName(fn), lastName(ln), birthDate(bd) {}
    int GetID() const { return id; }
    const std::string& GetFirstName() const { return firstName; }
    const std::string& GetLastName() const { return lastName; }
    std::time_t GetBirthDate() const { return birthDate; }
    bool operator<(const Person& other) const { return id < other.id; }
    bool operator==(const Person& other) const { return id == other.id; }
};
