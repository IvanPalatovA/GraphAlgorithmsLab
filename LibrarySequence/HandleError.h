#ifndef HANDLEERROR
#define HANDLEERROR
#include <string>
#include <iostream>
typedef enum {
    IndexOutOfRange,
    MemoryAllocationFailed,
    IncorrectParametrs,
    ErrorUnknown,
    NegativeSize
}ErrorCode;
void HandleError(ErrorCode code, const std::string& location, const std::string& context, short isCritical) {
    switch (code) {
        case MemoryAllocationFailed:
            std::cerr << "Не удалось выделить память (" << location << ", " << context << ")\n";
            throw std::bad_alloc();  // Стандартное исключение при ошибке аллокации
            break;

        case IndexOutOfRange:
            std::cerr << "Индексы выходят за пределы (" << location << ", " << context << ")\n";
            throw std::out_of_range("IndexOutOfRange in " + location + ": " + context);
            break;

        case IncorrectParametrs:
            std::cerr << "Некорректные параметры в (" << location << "). " << context << "\n";
            throw std::invalid_argument("IncorrectParameters in " + location + ": " + context);
            break;

        case NegativeSize:
            std::cerr << "Задан отрицательный размер в (" << location << "). " << context << "\n";
            throw std::invalid_argument("NegativeSize in " + location + ": " + context);
            break;
        case ErrorUnknown:
        default:
            std::cerr << "Неизвестная ошибка в (" << location << "). " << context << "\n";
            throw std::runtime_error("Unknown error in " + location + ": " + context);
            break;
    }
}
#endif