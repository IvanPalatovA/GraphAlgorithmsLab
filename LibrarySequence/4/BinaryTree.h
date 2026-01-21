#pragma once

#include <functional>
#include <string>
#include <stdexcept>
#include <sstream>
#include "TreeNode.h"
// #include "../HandleError.h"


template<typename T>
class BinaryTree {
private:
    TreeNode<T>* root;
    size_t nodeCount;
    // найти вершину в дереве
    TreeNode<T>* _findNode(TreeNode<T>* node, const T& value) const {
        if (!node) return nullptr;
        if (value < node->value) return _findNode(node->left, value);
        if (node->value < value) return _findNode(node->right, value);
        return node;
    }
    // копировать поддерево в структуру target
    void _copySubtree(TreeNode<T>* node, BinaryTree<T>& target) const {
        if (!node) return;
        target.Insert(node->value);
        _copySubtree(node->left, target);
        _copySubtree(node->right, target);
    }

    // глубокое копирование дерева
    TreeNode<T>* _copy(TreeNode<T>* src) {
        if (!src) return nullptr;
        TreeNode<T>* newNode = new TreeNode<T>(src->value);
        newNode->left = _copy(src->left);
        newNode->right = _copy(src->right);
        return newNode;
    }
    TreeNode<T>* _insert(TreeNode<T>* node, const T& value) {
        if (!node) {
            ++nodeCount;
            return new TreeNode<T>(value);
        }
        if (value < node->value)
            node->left = _insert(node->left, value);
        else if (node->value < value)
            node->right = _insert(node->right, value);
        else
            // HandleError(IndexOutOfRange, "BinaryTree::Insert", "duplicate value", 0);
            throw std::invalid_argument("BinaryTree::Insert: duplicate value");
        return _balance(node);
    }

    TreeNode<T>* _findMin(TreeNode<T>* node) const {
        return node->left ? _findMin(node->left) : node;
    }

    TreeNode<T>* _remove(TreeNode<T>* node, const T& value) {
    if (!node)
        throw std::out_of_range("BinaryTree: value not found");

    if (value < node->value) {
        node->left = _remove(node->left, value);
    }
    else if (node->value < value) {
        node->right = _remove(node->right, value);
    }
    else {
        // нашли узел для удаления
        // 1) если нет левого ребёнка — возвращаем правый
        if (!node->left) {
            TreeNode<T>* rightChild = node->right;
            delete node;
            --nodeCount;
            return rightChild;
        }
        // 2) если нет правого — возвращаем левый
        if (!node->right) {
            TreeNode<T>* leftChild = node->left;
            delete node;
            --nodeCount;
            return leftChild;
        }
        TreeNode<T>* succ = _findMin(node->right);
        node->value = succ->value;
        node->right = _remove(node->right, succ->value);
    }
    return _balance(node);
}

    int _height(TreeNode<T>* node) const {
        if (!node) return 0;
        return 1 + std::max(_height(node->left), _height(node->right));
    }

    int _getBalanceFactor(TreeNode<T>* node) const {
        return node ? _height(node->left) - _height(node->right) : 0;
    }

    TreeNode<T>* _rotateRight(TreeNode<T>* y) {
        TreeNode<T>* x = y->left;
        y->left = x->right;
        x->right = y;
        return x;
    }
    TreeNode<T>* _rotateLeft(TreeNode<T>* x) {
        TreeNode<T>* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    TreeNode<T>* _balance(TreeNode<T>* node) {
        int bf = _getBalanceFactor(node);
        if (bf > 1) {
            if (_getBalanceFactor(node->left) < 0)
                node->left = _rotateLeft(node->left);
            return _rotateRight(node);
        }
        if (bf < -1) {
            if (_getBalanceFactor(node->right) > 0)
                node->right = _rotateRight(node->right);
            return _rotateLeft(node);
        }
        return node;
    }

    void _free(TreeNode<T>* node) {
        if (!node) return;
        _free(node->left);
        _free(node->right);
        delete node;
    }

    // Обход с конкатенацией в строку
    void _toString(TreeNode<T>* node, const std::string& fmt, std::ostringstream& out) const {
        if (!node) return;
        // простой pre-order
        out << fmt.front() << node->value << fmt[1];
        _toString(node->left, fmt, out);
        out << fmt[2];
        _toString(node->right, fmt, out);
        out << fmt.back();
    }
    // равны ли деревья
    bool _areTreesEqual(TreeNode<T>* a, TreeNode<T>* b) const {
        if (!a && !b) return true;
        if (!a || !b) return false;
        if (!(a->value == b->value)) return false;
        return _areTreesEqual(a->left, b->left) && _areTreesEqual(a->right, b->right);
    }
public:
    // конструктор без параметров
    BinaryTree() : root(nullptr), nodeCount(0) {}
    ~BinaryTree() { _free(root); }
    TreeNode<T>* GetRoot() const {
        return root;
    }
    BinaryTree(const BinaryTree& other) {
        nodeCount = other.nodeCount;
        root = _copy(other.root);
    }
    void Insert(const T& value) { root = _insert(root, value); }
    bool Search(const T& value) const {
        TreeNode<T>* cur = root;
        while (cur) {
            if (value < cur->value) cur = cur->left;
            else if (cur->value < value) cur = cur->right;
            else return true;
        }
        return false;
    }
    // является ли вершина рутом?
    bool isRoot(const T& valuee){
        if (root->value == valuee) {return true;}
        else {return false;}
    }
    // удаление вершины
    void Remove(const T& value) { root = _remove(root, value); }
    // КЛП
    void KLP(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; visit(n->value); dfs(n->left); dfs(n->right);
        };
        dfs(root);
    }
    // КПЛ
    void KPL(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; visit(n->value); dfs(n->right); dfs(n->left);
        };
        dfs(root);
    }
    // ЛКП
    void LKP(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; dfs(n->left); visit(n->value); dfs(n->right);
        };
        dfs(root);
    }
    // ЛПК
    void LPK(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; dfs(n->left); dfs(n->right); visit(n->value);
        };
        dfs(root);
    }
    // ПКЛ
    void PKL(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; dfs(n->right); visit(n->value); dfs(n->left);
        };
        dfs(root);
    }
    // ПЛК
    void PLK(std::function<void(const T&)> visit) const {
        std::function<void(TreeNode<T>*)> dfs = [&](TreeNode<T>* n) {
            if (!n) return; dfs(n->right); dfs(n->left); visit(n->value); 
        };
        dfs(root);
    }

    BinaryTree<T> Map(std::function<T(const T&)> f) const {
        BinaryTree<T> res;
        KLP([&](const T& v){ res.Insert(f(v)); });
        return res;
    }
    BinaryTree<T> Where(std::function<bool(const T&)> pred) const {
        BinaryTree<T> res;
        LKP([&](const T& v){ if(pred(v)) res.Insert(v); });
        return res;
    }
    T Reduce(std::function<T(const T&, const T&)> f, const T& init) const {
        T acc = init;
        LKP([&](const T& v){ acc = f(acc, v); });
        return acc;
    }
    // копировать поддерево
    BinaryTree<T> ExtractSubtree(const T& rootValue) const {
        BinaryTree<T> subtree;
        TreeNode<T>* subroot = _findNode(root, rootValue);
        _copySubtree(subroot, subtree);
        return subtree;
    }
    // содержится ли поддерево в дереве?
    bool ContainsSubtree(const BinaryTree<T>& subtree) const {
        if (subtree.Empty()) return true;
        TreeNode<T>* subroot = _findNode(root, subtree.root->value);
        if (!subroot) return false;
        
        return _areTreesEqual(subroot, subtree.GetRoot());
    }
    // перевод в строку
    std::string ToString(const std::string& format) const {
        std::ostringstream out;
        _toString(root, format, out);
        return out.str();
    }

    bool Empty() const { return nodeCount == 0; }
    size_t Size() const { return nodeCount; }
};
