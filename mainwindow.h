#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "linkedlist.h"
#include "queue.h"
#include "stack.h"

class QLineEdit;
class QPushButton;
class QTableWidget;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void addBook();
    void requestBook();
    void returnBook();
    void processRequests();
    void processReturns();
    void refreshTable();

private:
    void buildUi();
    void applyTheme();
    void setupConnections();
    void loadData();
    void saveData();
    void loadQueue();
    void saveQueue();
    void loadReturns();
    void saveReturns();
    void populateRequestsTable();
    void populateReturnsTable();
    void showMessage(const QString &title, const QString &text, QMessageBox::Icon icon);

    void closeEvent(QCloseEvent *event) override;

    BookList bookList;
    Queue requestQueue;
    Stack returnStack;

    QLineEdit *bookIdEdit = nullptr;
    QLineEdit *titleEdit = nullptr;
    QLineEdit *authorEdit = nullptr;
    QLineEdit *usernameEdit = nullptr;
    QPushButton *addBookButton = nullptr;
    QPushButton *requestBookButton = nullptr;
    QPushButton *returnBookButton = nullptr;
    QPushButton *processRequestsButton = nullptr;
    QPushButton *processReturnsButton = nullptr;
    QPushButton *refreshButton = nullptr;
    QTableWidget *booksTable = nullptr;
    QTableWidget *requestsTable = nullptr;
    QTableWidget *returnsTable = nullptr;
};

#endif // MAINWINDOW_H
