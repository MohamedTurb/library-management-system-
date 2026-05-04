#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <string>
#include <vector>

using std::string;

struct Request {
    string userName;
    int bookID;
    Request() : userName(""), bookID(0) {}
    Request(const string& u, int b) : userName(u), bookID(b) {}
};

struct QNode {
    Request data;
    QNode* next;
    QNode(const string& u, int b) : data(u, b), next(nullptr) {}
};

class Queue {
private:
    QNode* front;
    QNode* rear;
    int size;

public:
    Queue();
    ~Queue();

    bool isEmpty() const;
    void enqueue(string userName, int bookID);
    bool dequeue(Request& request);
    bool peek(Request& request);
    void displayQueue() const;
    int getSize() const;
    std::vector<Request> getPendingRequests() const;
};

#endif // QUEUE_H
