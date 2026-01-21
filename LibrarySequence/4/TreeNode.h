#pragma once

#include <stdexcept>


template<typename T>
struct TreeNode {
    T value;
    TreeNode<T>* left;
    TreeNode<T>* right;
    TreeNode(const T& val)
        : value(val), left(nullptr), right(nullptr) {}
};
