//
// Created by ilya on 22.06.19.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <iterator>

template<typename T>
struct list {

    struct node_base {
        node_base() : next(nullptr), prev(nullptr) {};

        node_base *next;
        node_base *prev;

        node_base(node_base *p, node_base *n) : next(n), prev(p) {};

        virtual ~node_base() = default;
    };

    struct node : node_base {
        T value;

        node() = delete;

        node(T v) : node_base(), value(v) {};

        node(T v, node_base *p) : node_base(p), value(v) {};

        node(T v, node_base *p, node_base *n) : node_base(p, n), value(v) {};
    };

    node_base fict;


    list() noexcept {
        fict.next = fict.prev = &fict;
    }

    list(list<T> const &other) : list() {
        if (&other.fict != nullptr) {
            node_base *cur = other.fict.next;
            while (cur != &other.fict) {
                push_back(((node *) cur)->value);
                cur = cur->next;
            }
        }
    }

    list<T> &operator=(list<T> const &other) {
        if (&other.fict == &fict) {
            return *this;
        }
        list<T> tmp(other);
        swap(tmp, *this);
        return *this;
    }

    void push_back(T const &value) {
        node_base *last = fict.prev;
        fict.prev = new node(value, last, &fict);
        last->next = fict.prev;
    }

    void push_front(T const &value) {
        insert(begin(), value);
    }

    void pop_back() {
        node_base *del = fict.prev;
        fict.prev->prev->next = &fict;
        fict.prev = fict.prev->prev;
        delete del;
    }

    void clear() {
        node_base *it = fict.next;
        while (it != &fict) {
            node_base *mem = it;
            it = it->next;
            delete mem;
        }
        fict.prev = fict.next = &fict;

    }

    ~list() {
        clear();
    }

    bool empty() const {
        return &fict == fict.next;
    }

    T &back() {
        return ((node *) fict.prev)->value;
    }

    T const &back() const {
        return ((node *) fict.prev)->value;
    }

    T &front() {
        return ((node *) fict.next)->value;
    }

    T const &front() const {
        return ((node *) fict.next)->value;
    }

    // ================================ iterator =============================

    template<typename U>
    struct my_iterator : public std::iterator<std::bidirectional_iterator_tag, U> {
        friend class list<T>;

        my_iterator() = default;

        my_iterator(my_iterator<T> const &other) : it(other.it) {}

        my_iterator &operator++() {
            it = it->next;
            return *this;
        }

        my_iterator operator++(int) {
            my_iterator<U> old(*this);
            ++*this;
            return old;
        }

        my_iterator &operator--() {
            it = it->prev;
            return *this;
        }

        my_iterator operator--(int) {
            my_iterator<U> old(*this);
            --*this;
            return old;
        }

        U &operator*() const { return static_cast<node *>(it)->value; }

        U *operator->() const { return &static_cast<node *>(it)->value; }

        template<typename Z>
        bool operator==(my_iterator<Z> const &other) const {
            return it == other.it;
        }

        template<typename Z>
        bool operator!=(my_iterator<Z> const &other) const {
            return it != other.it;
        }

        my_iterator(node_base *p) : it(p) {}

        my_iterator(nullptr_t) = delete;

    public:
        node_base *it;
    };

    using iterator = my_iterator<T>;
    using const_iterator = my_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<my_iterator<T>>;
    using const_reverse_iterator =
    std::reverse_iterator<my_iterator<T const>>;

    iterator begin() { return iterator(fict.next); }

    const_iterator begin() const {
        return const_iterator(fict.next);
    }

    iterator end() {
        return iterator(&fict);
    }

    const_iterator end() const {
        return const_iterator(const_cast<node_base *>(&fict));
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator pos, T const &x) {
        auto mem = pos;
        mem.it->prev = new node(x, mem.it->prev, mem.it);
        node_base *new_node = mem.it->prev;
        new_node->prev->next = new_node;
        return iterator(new_node);
    }

    iterator erase(const_iterator pos) {
        pos.it->prev->next = pos.it->next;
        pos.it->next->prev = pos.it->prev;
        iterator ret = pos.it->next;
        delete pos.it;
        return ret;
    }

    void pop_front() {
        erase(begin());
    }

    void splice(const_iterator pos, list<T> &other, const_iterator begin, const_iterator end) {
        node_base *left = begin.it->prev;

        pos.it->prev->next = begin.it;
        begin.it->prev = pos.it->prev;

        end.it->prev->next = pos.it;
        pos.it->prev = end.it->prev;

        end.it->prev = left;
        left->next = end.it;
    }
};

template<typename U>
void swap(list<U> &a, list<U> &b) {
    auto a_prev = a.fict.prev;
    auto a_next = a.fict.next;

    auto b_prev = b.fict.prev;
    auto b_next = b.fict.next;

    a_prev->next = &b.fict;
    a_next->prev = &b.fict;

    b_prev->next = &a.fict;
    b_next->prev = &a.fict;

    std::swap(a.fict, b.fict);
}

#endif //LIST_LIST_H
