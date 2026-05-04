#include "linkedlist.h"
#include <iostream>

using namespace std;

BookNode::BookNode(int id, const string& t, const string& a)
    : bookID(id), title(t), author(a), isAvailable(true), next(nullptr) {}

BookList::BookList() : head(nullptr) {}

BookList::~BookList() {
    BookNode* curr = head;
    while (curr) {
        BookNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

void BookList::addBook(int id, const string& title, const string& author) {
    if (searchBook(id) != nullptr) {
        cout << "[BookList] ERROR: Book with ID " << id << " already exists." << endl;
        return;
    }

    BookNode* newNode = new BookNode(id, title, author);

    if (!head) {
        head = newNode;
    } else {
        BookNode* curr = head;
        while (curr->next)
            curr = curr->next;
        curr->next = newNode;
    }

    cout << "[BookList] Book Added  ->  ID: " << id
         << " | \"" << title << "\" by " << author << endl;
}

void BookList::removeBook(int id) {
    if (!head) {
        cout << "[BookList] ERROR: List is empty." << endl;
        return;
    }

    if (head->bookID == id) {
        BookNode* temp = head;
        head = head->next;
        cout << "[BookList] Book Removed  ->  ID: " << id
             << " | \"" << temp->title << "\"" << endl;
        delete temp;
        return;
    }

    BookNode* prev = head;
    BookNode* curr = head->next;
    while (curr && curr->bookID != id) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) {
        cout << "[BookList] ERROR: Book with ID " << id << " not found." << endl;
        return;
    }

    prev->next = curr->next;
    cout << "[BookList] Book Removed  ->  ID: " << id
         << " | \"" << curr->title << "\"" << endl;
    delete curr;
}

BookNode* BookList::searchBook(int id) {
    BookNode* curr = head;
    while (curr) {
        if (curr->bookID == id)
            return curr;
        curr = curr->next;
    }
    return nullptr;
}

bool BookList::updateAvailability(int id, bool status) {
    BookNode* book = searchBook(id);
    if (!book) {
        cout << "[BookList] ERROR: Book ID " << id << " not found." << endl;
        return false;
    }
    book->isAvailable = status;
    cout << "[BookList] Availability Updated  ->  ID: " << id
         << " | \"" << book->title << "\"  ->  "
         << (status ? "Available" : "Borrowed") << endl;
    return true;
}

void BookList::displayAvailableBooks() {
    cout << "\n+--------------------------------------+\n";
    cout << "|         AVAILABLE BOOKS              |\n";
    cout << "+--------------------------------------+\n";

    bool found = false;
    BookNode* curr = head;
    while (curr) {
        if (curr->isAvailable) {
            printBook(curr);
            found = true;
        }
        curr = curr->next;
    }

    if (!found)
        cout << "  (No books currently available)\n";

    cout << "--------------------------------------\n\n";
}

void BookList::displayAllBooks() {
    cout << "\n+--------------------------------------+\n";
    cout << "|             ALL BOOKS                |\n";
    cout << "+--------------------------------------+\n";

    if (!head) {
        cout << "  (No books in the system)\n";
    } else {
        BookNode* curr = head;
        while (curr) {
            printBook(curr);
            curr = curr->next;
        }
    }

    cout << "--------------------------------------\n\n";
}

bool BookList::processRequest(int id) {
    BookNode* book = searchBook(id);

    if (!book) {
        cout << "[BookList] ERROR: Book ID " << id << " does not exist." << endl;
        return false;
    }

    if (!book->isAvailable) {
        cout << "[BookList] Book Not Available  ->  ID: " << id
             << " | \"" << book->title << "\" is currently borrowed." << endl;
        return false;
    }

    updateAvailability(id, false);
    return true;
}

bool BookList::processReturn(int id) {
    BookNode* book = searchBook(id);

    if (!book) {
        cout << "[BookList] ERROR: Book ID " << id << " does not exist." << endl;
        return false;
    }

    if (book->isAvailable) {
        cout << "[BookList] WARNING: Book ID " << id << " was not borrowed." << endl;
        return false;
    }

    updateAvailability(id, true);
    return true;
}

void BookList::printBook(BookNode* b) {
    cout << "  ID: " << b->bookID
         << "  |  " << b->title
         << "  |  " << b->author
         << "  |  [" << (b->isAvailable ? "Available" : "Borrowed") << "]\n";
}

std::vector<BookInfo> BookList::getAllBooks() const {
    std::vector<BookInfo> books;
    BookNode* curr = head;

    while (curr) {
        books.push_back(BookInfo{curr->bookID, curr->title, curr->author, curr->isAvailable});
        curr = curr->next;
    }

    return books;
}
