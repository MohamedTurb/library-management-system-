#include "stack.h"
using namespace std;

Stack::Stack() : top(nullptr) {}

Stack::~Stack() {
    while (top) {
        Stack::Node* tmp = top;
        top = top->next;
        delete tmp;
    }
}

bool Stack::isEmpty() const {
    return top == nullptr;
}

void Stack::push(int bookID) {
    Stack::Node* newNode = new Stack::Node(bookID);
    newNode->next = top;
    top = newNode;

    cout << "📥 Book Returned: ID = " << bookID << endl;
}

int Stack::peek() const {
    if (isEmpty()) {
        cout << "⚠️ Stack is empty!" << endl;
        return -1;
    }
    return top->bookID;
}

int Stack::pop() {
    if (isEmpty()) {
        cout << "❌ Cannot process, stack is empty!" << endl;
        return -1;
    }

    Stack::Node* temp = top;
    int id = temp->bookID;
    cout << "⚙️ Processing Return: Book ID = " << id << endl;

    top = top->next;
    delete temp;
    return id;
}

void Stack::displayStack() const {
    if (isEmpty()) {
        cout << "📭 Stack is empty!" << endl;
        return;
    }

    Stack::Node* current = top;
    cout << "📚 Returned Books Stack:\n";

    while (current != nullptr) {
        cout << "Book ID: " << current->bookID << endl;
        current = current->next;
    }
}

std::vector<int> Stack::getPendingReturns() const {
    std::vector<int> books;
    Node* current = top;

    while (current != nullptr) {
        books.push_back(current->bookID);
        current = current->next;
    }

    return books;
}
