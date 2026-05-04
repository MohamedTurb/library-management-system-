#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <string>
#include <vector>

using std::string;

struct BookNode {
    int bookID;
    string title;
    string author;
    bool isAvailable;
    BookNode* next;
    BookNode(int id, const string& t, const string& a);
};

struct BookInfo {
    int bookID;
    string title;
    string author;
    bool isAvailable;
};

class BookList {
private:
    BookNode* head;
    void printBook(BookNode* b);

public:
    BookList();
    ~BookList();

    void addBook(int id, const string& title, const string& author);
    void removeBook(int id);
    BookNode* searchBook(int id);
    bool updateAvailability(int id, bool status);
    void displayAvailableBooks();
    void displayAllBooks();
    bool processRequest(int id);
    bool processReturn(int id);
    std::vector<BookInfo> getAllBooks() const;
};

#endif // LINKEDLIST_H
