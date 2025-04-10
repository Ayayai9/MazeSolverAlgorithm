#ifndef WALLSLINKEDLIST
#define WALLSLINKEDLIST
#include "BlockCoord.h"
#include "BreadthAndCoord.h"

template <typename T> class Node {
public:
    T node;
    Node<T> *next;

    Node(T node, Node<T> *next = nullptr) : node(node), next(next) {}
};

template <typename T> class LinkedListTemplate {
private:
    Node<T> *head;

public:
    LinkedListTemplate() : head(nullptr) {}

    ~LinkedListTemplate() {
        Node<T> *current = head;
        Node<T> *temp = nullptr;
        while (current != nullptr) {
            temp = current->next;
            delete current;
            current = temp;
        }
    }

    void addToStart(T node) {
        Node<T> *newNode = new Node<T>(node);
        newNode->next = this->head;
        this->head = newNode;
    }

    void addToEnd(T node) {
        Node<T> *newNode = new Node<T>(node);
        if (this->head == nullptr) {
            this->head = newNode;
        } else {
            Node<T> *current = this->head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    int getSize() {
        Node<T> *current = this->head;
        int count = 0;
        while (current != nullptr) {
            current = current->next;
            count++;
        }
        return count;
    }

    void clear() {
        Node<T> *current = this->head;
        Node<T> *temp = nullptr;
        while (current != nullptr) {
            temp = current->next;
            delete current;
            current = temp;
        }
        this->head = nullptr;
    }

    T at(int index) {
        Node<T> *current = this->head;
        int counter = 0;
        T result = current->node;
        while (current != nullptr) {
            if (counter == index) {
                result = current->node;
            }
            current = current->next;
            ++counter;
        }
        return result;
    }

    Node<T> *getFirst() { return this->head; }

    Node<T> *getNext(Node<T> *current) {
        Node<T> *answer = nullptr;
        if (current != nullptr) {
            answer = current->next;
        }

        return answer;
    }

    void deleteLast() {
        if (head == nullptr) {
        } else if (head->next == nullptr) {
            delete head;
            head = nullptr;
        } else {
            Node<T> *current = head;
            while (current->next->next != nullptr) {
                current = current->next;
            }
            delete current->next;
            current->next = nullptr;
        }
        return;
    }

    bool contains(mcpp::Coordinate coordinate, int index) {
        bool contains = false;
        Node<T> *current = this->head;
        while (current != nullptr) {
            if (coordinate == current->node.getCoord() &&
                index == current->node.getBreadth()) {
                contains = true;
            }
            current = current->next;
        }
        return contains;
    }

    int distanceAt(mcpp::Coordinate coordinate) {
        Node<T> *current = this->head;
        int breadth = -1;

        while (current != nullptr) {
            if (current->node.getCoord() == coordinate) {
                breadth = current->node.getBreadth();
            }
            current = current->next;
        }
        return breadth;
    }
};

#endif