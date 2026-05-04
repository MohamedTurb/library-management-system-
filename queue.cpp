#include "queue.h"
using namespace std;

Queue::Queue() : front(nullptr), rear(nullptr), size(0) {}

Queue::~Queue() {
    while (!isEmpty()) {
        Request temp;
        dequeue(temp);
    }
}

bool Queue::isEmpty() const {
    return front == nullptr;
}

void Queue::enqueue(string userName, int bookID) {
    QNode* newNode = new QNode(userName, bookID);

    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    size++;

    cout << "Request Added: User \"" << userName
         << "\" requested Book ID " << bookID << endl;
}

bool Queue::dequeue(Request& request) {
    if (isEmpty()) {
        cout << "Queue is empty. No requests to process." << endl;
        return false;
    }

    QNode* temp = front;

    request = front->data;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
    size--;

    cout << "Processing Request: User \"" << request.userName
         << "\" requested Book ID " << request.bookID << endl;

    return true;
}

bool Queue::peek(Request& request) {
    if (isEmpty()) {
        cout << "Queue is empty. No request to show." << endl;
        return false;
    }

    request = front->data;
    return true;
}

void Queue::displayQueue() const {
    if (isEmpty()) {
        cout << "Queue is empty. No pending requests." << endl;
        return;
    }

    QNode* current = front;

    cout << "\nPending Requests:\n";
    cout << "-----------------\n";

    while (current != nullptr) {
        cout << "User: " << current->data.userName
             << " | Book ID: " << current->data.bookID << endl;

        current = current->next;
    }

    cout << "-----------------\n";
}

int Queue::getSize() const {
    return size;
}

std::vector<Request> Queue::getPendingRequests() const {
    std::vector<Request> requests;
    QNode* current = front;

    while (current != nullptr) {
        requests.push_back(current->data);
        current = current->next;
    }

    return requests;
}