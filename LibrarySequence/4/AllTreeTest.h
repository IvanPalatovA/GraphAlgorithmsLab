#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>
#include "BinaryTree.h"
#include "Person.h"

// Дополнительные функциональные тесты

void TestBalancingProperties() {
    std::cout << "=== TestBalancingProperties ===" << "\n";
    
    BinaryTree<int> tree;
    
    // Вставляем элементы в порядке возрастания
    std::vector<int> ascending = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int val : ascending) {
        tree.Insert(val);
    }
    
    for (int val : ascending) {
        assert(tree.Search(val));
    }
    
    // Вставляем элементы в порядке убывания
    BinaryTree<int> tree2;
    std::vector<int> descending = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    for (int val : descending) {
        tree2.Insert(val);
    }
    
    for (int val : descending) {
        assert(tree2.Search(val));
    }
    
    std::cout << "TestBalancingProperties passed!" << "\n";
}

void TestAllTraversals() {
    std::cout << "=== TestAllTraversals ===" << "\n";
    
    BinaryTree<int> tree;
    std::vector<int> values = {4, 2, 6, 1, 3, 5, 7};
    for (int val : values) {
        tree.Insert(val);
    }
    
    std::vector<int> klp, kpl, lkp, lpk, pkl, plk;
    
    tree.KLP([&](const int& val) { klp.push_back(val); });
    tree.KPL([&](const int& val) { kpl.push_back(val); });
    tree.LKP([&](const int& val) { lkp.push_back(val); });
    tree.LPK([&](const int& val) { lpk.push_back(val); });
    tree.PKL([&](const int& val) { pkl.push_back(val); });
    tree.PLK([&](const int& val) { plk.push_back(val); });
    
    // Проверяем, что все обходы содержат все элементы
    assert(klp.size() == values.size());
    assert(kpl.size() == values.size());
    assert(lkp.size() == values.size());
    assert(lpk.size() == values.size());
    assert(pkl.size() == values.size());
    assert(plk.size() == values.size());
    
    // LKP (inorder) должен быть отсортирован
    for (size_t i = 1; i < lkp.size(); i++) {
        assert(lkp[i-1] < lkp[i]);
    }
    // KLP должен начинаться с корня
    assert(std::find(values.begin(), values.end(), klp[0]) != values.end());
    assert(tree.isRoot(klp[0]));
    std::cout << "TestAllTraversals passed!" << "\n";
}

void TestMapChaining() {
    std::cout << "=== TestMapChaining ===" << "\n";
    
    BinaryTree<int> tree;
    std::vector<int> values = {1, 2, 3, 4, 5};
    for (int val : values) {
        tree.Insert(val);
    }
    // x -> x*2 -> x+1 -> x*x
    BinaryTree<int> step1 = tree.Map([](const int& x) { return x * 2; });
    BinaryTree<int> step2 = step1.Map([](const int& x) { return x + 1; });
    BinaryTree<int> final = step2.Map([](const int& x) { return x * x; });

    // Проверяем результат для значения 3:
    // 3 -> 6 -> 7 -> 49
    assert(final.Search(49));
    
    // Для значения 5: 5 -> 10 -> 11 -> 121
    assert(final.Search(121));
    std::cout << "TestMapChaining passed!" << "\n";
}

void TestWhereFiltering() {
    std::cout << "=== TestWhereFiltering ===" << "\n";
    
    BinaryTree<int> tree;
    for (int i = 1; i <= 20; i++) {
        tree.Insert(i);
    }
    
    // Фильтр 1: только четные числа
    BinaryTree<int> evens = tree.Where([](const int& x) { return x % 2 == 0; });
    
    for (int i = 2; i <= 20; i += 2) {
        assert(evens.Search(i));
    }
    for (int i = 1; i <= 19; i += 2) {
        assert(!evens.Search(i));
    }
    
    // Фильтр 2: только числа больше 10
    BinaryTree<int> large = tree.Where([](const int& x) { return x > 10; });
    
    for (int i = 11; i <= 20; i++) {
        assert(large.Search(i));
    }
    for (int i = 1; i <= 10; i++) {
        assert(!large.Search(i));
    }
    
    // Комбинация 2-х фильтров: четные и больше 10
    BinaryTree<int> evenAndLarge = tree.Where([](const int& x) { 
        return x % 2 == 0 && x > 10; 
    });
    
    std::vector<int> expected = {12, 14, 16, 18, 20};
    for (int val : expected) {
        assert(evenAndLarge.Search(val));
    }
    assert(evenAndLarge.Size() == expected.size());
    
    std::cout << "TestWhereFiltering passed!" << "\n";
}

void TestReduceOperations() {
    std::cout << "=== TestReduceOperations ===" << "\n";
    
    BinaryTree<int> tree;
    std::vector<int> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int val : values) {
        tree.Insert(val);
    }
    
    // Сумма всех элементов
    int sum = tree.Reduce([](const int& acc, const int& val) { 
        return acc + val; 
    }, 0);
    assert(sum == 55); // 1+2+...+10 = 55
    
    // Произведение всех элементов
    int product = tree.Reduce([](const int& acc, const int& val) { 
        return acc * val; 
    }, 1);
    assert(product == 3628800); // 10!
    
    // Максимальный элемент
    int max = tree.Reduce([](const int& acc, const int& val) { 
        return std::max(acc, val); 
    }, INT_MIN);
    assert(max == 10);
    
    // Минимальный элемент
    int min = tree.Reduce([](const int& acc, const int& val) { 
        return std::min(acc, val); 
    }, INT_MAX);
    assert(min == 1);
    
    // Подсчет четных элементов
    int evenCount = tree.Reduce([](const int& acc, const int& val) { 
        return acc + (val % 2 == 0 ? 1 : 0); 
    }, 0);
    assert(evenCount == 5); // 2, 4, 6, 8, 10
    std::cout << "TestReduceOperations passed!" << "\n";
}
void TestSubtreeOperations() {
    std::cout << "=== TestSubtreeOperations ===" << "\n";
    
    BinaryTree<int> tree;
    std::vector<int> values = {10, 5, 15, 3, 7, 12, 18, 1, 4, 6, 8};
    for (int val : values) {
        tree.Insert(val);
    }
    
    // Извлекаем поддерево с корнем 5
    BinaryTree<int> subtree = tree.ExtractSubtree(5);
    assert(subtree.Search(5));
    assert(subtree.Search(3));
    assert(subtree.Search(7));
    assert(!subtree.Search(10)); // 10 не должно быть в поддереве
    
    // Проверяем, что изначальное дерево имеет извлеченное поддерево
    assert(tree.ContainsSubtree(subtree));
    
    // Создаем дерево, которое не содержится в исходном
    BinaryTree<int> notSubtree;
    notSubtree.Insert(100);
    notSubtree.Insert(200);
    assert(!tree.ContainsSubtree(notSubtree));
    
    std::cout << "TestSubtreeOperations passed!" << "\n";
}

void TestPersonAdvanced() {
    std::cout << "=== TestPersonAdvanced ===" << "\n";

    BinaryTree<Person> tree;
    Person p1(1, "Ivan", "Palatov", 0);
    Person p2(2, "Kirill", "Fedchenko", 0);
    Person p3(3, "Anton", "Holodkov", 0);
    Person p4(4, "Prosto", "Chell", 0);
    tree.Insert(p1);
    tree.Insert(p2);
    tree.Insert(p3);
    tree.Insert(p4);
    
    // Тест Map: создаем дерево ID+100
    BinaryTree<Person> mapped = tree.Map([](const Person& p) {
        return Person(p.GetID() + 100, p.GetFirstName(), p.GetLastName(), p.GetBirthDate());
    });
    assert(mapped.Search(Person(101, "Ivan", "Palatov", 0)));
    assert(mapped.Search(Person(104, "Prosto", "Chell", 0)));
    
    // Тест Where: только четные ID
    BinaryTree<Person> evenIds = tree.Where([](const Person& p) {
        return p.GetID() % 2 == 0;
    });
    assert(evenIds.Search(p2)); // ID = 2
    assert(evenIds.Search(p4)); // ID = 4
    assert(!evenIds.Search(p1)); // ID = 1
    assert(!evenIds.Search(p3)); // ID = 3
    
    std::cout << "TestPersonAdvanced passed!" << "\n";
}

void TestToString() {
    std::cout << "=== TestToString ===" << "\n";
    
    BinaryTree<int> tree;
    tree.Insert(5);
    tree.Insert(3);
    tree.Insert(7);
    tree.Insert(1);
    tree.Insert(2);
    
    std::string result = tree.ToString("(:,)");
    // std::cout << "ToString result: " << result << "\n";
    assert(result == "(5:(2:(1:,),(3:,)),(7:,))");
    std::cout << "TestToString passed!" << "\n";
}

void TestEdgeCases() {
    std::cout << "=== TestEdgeCases ===" << "\n";
    
    // Тест пустого дерева
    BinaryTree<int> emptyTree;
    assert(emptyTree.Empty());
    assert(emptyTree.Size() == 0);
    assert(!emptyTree.Search(10));
    
    // Тест с одним элементом
    BinaryTree<int> singleTree;
    singleTree.Insert(42);
    assert(!singleTree.Empty());
    assert(singleTree.Size() == 1);
    assert(singleTree.Search(42));
    assert(singleTree.isRoot(42));
    
    // Тест копирования
    BinaryTree<int> original;
    original.Insert(10);
    original.Insert(5);
    original.Insert(15);
    
    BinaryTree<int> copy = original;
    assert(copy.Search(10));
    assert(copy.Search(5));
    assert(copy.Search(15));
    assert(copy.Size() == original.Size());
    
    std::cout << "TestEdgeCases passed!" << "\n";
}

void TestExceptionHandling() {
    std::cout << "=== TestExceptionHandling ===" << "\n";
    
    BinaryTree<int> tree;
    tree.Insert(10);
    tree.Insert(5);
    tree.Insert(15);
    
    // Тест дублирования значения
    try {
        tree.Insert(10); // Должно выбросить исключение
        assert(false); // Не должны дойти до сюда
    } catch (const std::invalid_argument&) {
        // Ожидаемое исключение
    }
    
    // Тест удаления несуществующего элемента
    try {
        tree.Remove(100); // Должно выбросить исключение
        assert(false);
    } catch (const std::out_of_range&) {
        // Ожидаемое исключение
    }
    
    std::cout << "TestExceptionHandling passed!" << "\n";
}

// главная функция для тестирования всех функций тестов
void RunAllTests() {
    std::cout << "========================================" << "\n";
    std::cout << "     RUNNING ALL BINARY TREE TESTS     " << "\n";
    std::cout << "========================================" << "\n";
    
    try {
        TestBalancingProperties();
        TestMapChaining();
        TestWhereFiltering();
        TestReduceOperations();
        TestSubtreeOperations();
        TestPersonAdvanced();
        TestAllTraversals();
        TestToString();
        TestEdgeCases();
        TestExceptionHandling();
        
        std::cout << "\n========================================" << "\n";
        std::cout << "    ALL TESTS PASSED SUCCESSFULLY!   " << "\n";
        std::cout << "========================================" << "\n";
    } catch (const std::exception& e) {
        std::cout << "\n TEST FAILED: " << e.what() << "\n";
    } catch (...) {
        std::cout << "\n UNKNOWN ERROR OCCURRED!" << "\n";
    }
}
    