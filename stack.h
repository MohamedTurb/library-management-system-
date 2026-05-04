#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <vector>

class Stack {
private:
    struct Node {
        int bookID;
        Node* next;
        Node(int id) : bookID(id), next(nullptr) {}
    };

    Node* top;

public:
    Stack();
    ~Stack();

    bool isEmpty() const;
    void push(int bookID);
    int peek() const; // returns -1 if empty
    int pop(); // returns bookID or -1 if empty
    void displayStack() const;
    std::vector<int> getPendingReturns() const;
};

#endif // STACK_H
