#pragma once

#include "header.hpp"

namespace sf
{
    template <typename T>
    class SF_API linkedlist
    {
    public:
        linkedlist()
        {
            head = nullptr;
            _size = 0;
        };
        ~linkedlist() = default;

        void push(T data);
        T pop();
        T peek();
        bool empty();
        int size();
        void clear();
        T get(int index);
        void remove(int index);

    private:
        struct node
        {
            T data;
            node *next;
        };

        node *head;
        int _size;

        std::vector<T> _node_arr;
    };

    template <typename T>
    void linkedlist<T>::push(T data)
    {
        node *new_node = new node;
        new_node->data = data;
        new_node->next = head;
        head = new_node;
        _size++;

        _node_arr.push_back(data);
    }

    template <typename T>
    T linkedlist<T>::pop()
    {
        if (head == nullptr)
            throw std::runtime_error("Cannot pop from empty list");

        node *temp = head;
        T data = temp->data;
        head = head->next;
        delete temp;
        _size--;

        _node_arr.pop_back();
        return data;
    }

    template <typename T>
    T linkedlist<T>::peek()
    {
        if (head == nullptr)
            throw std::runtime_error("Cannot peek from empty list");

        return head->data;
    }

    template <typename T>
    bool linkedlist<T>::empty()
    {
        return head == nullptr;
    }

    template <typename T>
    int linkedlist<T>::size()
    {
        return _size;
    }

    template <typename T>
    void linkedlist<T>::clear()
    {
        while (head != nullptr)
            pop();
    }

    template <typename T>
    T linkedlist<T>::get(int index)
    {
        if (index < 0 || index >= _size)
            throw std::runtime_error("Index out of bounds");

        return _node_arr[index];
    }

    template <typename T>
    void linkedlist<T>::remove(int index)
    {
        node *res = head;
        _node_arr.erase(_node_arr.begin() + index);

        if (index == 0)
        {
            head = head->next;
            delete res;
            _size--;
            return;
        }

        for (int i = 0; i < index - 1; i++)
            res = res->next;

        node *temp = res->next;
        res->next = temp->next;
        delete temp;
        _size--;
    }
} // namespace sf
