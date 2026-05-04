#include "MainWindow.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QTableWidgetItem>

namespace {
constexpr int kBookIdColumn = 0;
constexpr int kTitleColumn = 1;
constexpr int kAuthorColumn = 2;
constexpr int kStatusColumn = 3;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
    applyTheme();
    setupConnections();
    loadData();
    refreshTable();
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::loadData()
{
    const auto booksPath = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "books.json";
    QFile file(booksPath);
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly)) {
        showMessage(tr("Load Error"), tr("Unable to open books.json for reading."), QMessageBox::Warning);
        return;
    }

    const QByteArray bytes = file.readAll();
    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isArray())
        return;

    // Clear current books
    const std::vector<BookInfo> current = bookList.getAllBooks();
    for (const BookInfo &b : current) {
        bookList.removeBook(b.bookID);
    }

    const QJsonArray arr = doc.array();
    for (const QJsonValue &v : arr) {
        if (!v.isObject())
            continue;
        const QJsonObject obj = v.toObject();
        const int id = obj.value("bookID").toInt();
        const QString title = obj.value("title").toString();
        const QString author = obj.value("author").toString();
        const bool isAvailable = obj.value("isAvailable").toBool(true);

        bookList.addBook(id, title.toStdString(), author.toStdString());
        if (!isAvailable)
            bookList.updateAvailability(id, false);
    }

    // load queue and returns
    loadQueue();
    loadReturns();
}

void MainWindow::saveData()
{
    QJsonArray arr;
    const std::vector<BookInfo> books = bookList.getAllBooks();
    for (const BookInfo &b : books) {
        QJsonObject obj;
        obj["bookID"] = b.bookID;
        obj["title"] = QString::fromStdString(b.title);
        obj["author"] = QString::fromStdString(b.author);
        obj["isAvailable"] = b.isAvailable;
        arr.append(obj);
    }

    const QJsonDocument doc(arr);
    const auto booksPath = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "books.json";
    QFile file(booksPath);
    if (!file.open(QIODevice::WriteOnly)) {
        showMessage(tr("Save Error"), tr("Unable to write books.json."), QMessageBox::Warning);
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    // save queue and returns
    saveQueue();
    saveReturns();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveData();
    QMainWindow::closeEvent(event);
}

void MainWindow::buildUi()
{
    setWindowTitle(tr("Library Management System"));
    resize(1240, 820);
    setMinimumSize(1024, 700);

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    auto *headerLabel = new QLabel(tr("Library Management System"), this);
    headerLabel->setObjectName("headerLabel");
    headerLabel->setWordWrap(true);

    auto *subHeaderLabel = new QLabel(
        tr("Manage books with a linked list, borrow requests with a queue, and returns with a stack."),
        this);
    subHeaderLabel->setObjectName("subHeaderLabel");
    subHeaderLabel->setWordWrap(true);

    rootLayout->addWidget(headerLabel);
    rootLayout->addWidget(subHeaderLabel);

    auto *topRowLayout = new QHBoxLayout();
    topRowLayout->setSpacing(16);

    auto *bookBox = new QGroupBox(tr("Book Management"), this);
    auto *bookForm = new QFormLayout(bookBox);
    bookForm->setLabelAlignment(Qt::AlignLeft);
    bookForm->setFormAlignment(Qt::AlignTop);

    bookIdEdit = new QLineEdit(this);
    titleEdit = new QLineEdit(this);
    authorEdit = new QLineEdit(this);
    bookIdEdit->setMinimumHeight(36);
    titleEdit->setMinimumHeight(36);
    authorEdit->setMinimumHeight(36);

    bookIdEdit->setPlaceholderText(tr("e.g. 1001"));
    titleEdit->setPlaceholderText(tr("Book title"));
    authorEdit->setPlaceholderText(tr("Author name"));
    bookIdEdit->setValidator(new QIntValidator(1, 2147483647, this));

    addBookButton = new QPushButton(tr("Add Book"), this);
    requestBookButton = new QPushButton(tr("Request Book"), this);
    returnBookButton = new QPushButton(tr("Return Book"), this);
    addBookButton->setMinimumHeight(38);
    requestBookButton->setMinimumHeight(38);
    returnBookButton->setMinimumHeight(38);

    bookForm->addRow(tr("Book ID"), bookIdEdit);
    bookForm->addRow(tr("Title"), titleEdit);
    bookForm->addRow(tr("Author"), authorEdit);
    bookForm->addRow(addBookButton);
    bookForm->addRow(requestBookButton);
    bookForm->addRow(returnBookButton);

    auto *operationsBox = new QGroupBox(tr("Queue and Stack Operations"), this);
    auto *operationsLayout = new QVBoxLayout(operationsBox);
    operationsLayout->setSpacing(12);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText(tr("Borrower username"));
    usernameEdit->setMinimumHeight(36);

    auto *usernameLabel = new QLabel(tr("Username"), this);

    auto *usernameRow = new QVBoxLayout();
    usernameRow->addWidget(usernameLabel);
    usernameRow->addWidget(usernameEdit);

    processRequestsButton = new QPushButton(tr("Process Requests"), this);
    processReturnsButton = new QPushButton(tr("Process Returns"), this);
    refreshButton = new QPushButton(tr("Refresh Table"), this);
    processRequestsButton->setMinimumHeight(38);
    processReturnsButton->setMinimumHeight(38);
    refreshButton->setMinimumHeight(38);

    operationsLayout->addLayout(usernameRow);
    operationsLayout->addWidget(processRequestsButton);
    operationsLayout->addWidget(processReturnsButton);
    operationsLayout->addWidget(refreshButton);
    operationsLayout->addStretch(1);

    topRowLayout->addWidget(bookBox, 5);
    topRowLayout->addWidget(operationsBox, 5);
    rootLayout->addLayout(topRowLayout);

    // Pending requests and returns tables
    auto *pendingBox = new QGroupBox(tr("Pending Requests & Returns"), this);
    auto *pendingLayout = new QHBoxLayout(pendingBox);

    requestsTable = new QTableWidget(this);
    requestsTable->setColumnCount(2);
    requestsTable->setHorizontalHeaderLabels({tr("Username"), tr("Book ID")});
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    requestsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    requestsTable->setMinimumHeight(140);

    returnsTable = new QTableWidget(this);
    returnsTable->setColumnCount(1);
    returnsTable->setHorizontalHeaderLabels({tr("Book ID")});
    returnsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    returnsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    returnsTable->setMinimumHeight(140);

    pendingLayout->addWidget(requestsTable, 1);
    pendingLayout->addWidget(returnsTable, 1);
    rootLayout->addWidget(pendingBox);

    booksTable = new QTableWidget(this);
    booksTable->setColumnCount(4);
    booksTable->setHorizontalHeaderLabels({tr("Book ID"), tr("Title"), tr("Author"), tr("Status")});
    booksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    booksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    booksTable->setAlternatingRowColors(true);
    booksTable->setSortingEnabled(false);
    booksTable->horizontalHeader()->setStretchLastSection(true);
    booksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    booksTable->verticalHeader()->setVisible(false);
    booksTable->setMinimumHeight(320);

    rootLayout->addWidget(booksTable, 1);
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow {
            background: #111827;
            color: #e5e7eb;
        }
        QLabel {
            color: #e5e7eb;
        }
        QLabel#headerLabel {
            font-size: 28px;
            font-weight: 700;
        }
        QLabel#subHeaderLabel {
            color: #9ca3af;
            font-size: 14px;
        }
        QGroupBox {
            color: #f9fafb;
            border: 1px solid #374151;
            border-radius: 12px;
            margin-top: 12px;
            padding: 12px;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
        QLineEdit {
            background: #1f2937;
            color: #f9fafb;
            border: 1px solid #374151;
            border-radius: 8px;
            padding: 6px 10px;
            selection-background-color: #2563eb;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 1px solid #60a5fa;
        }
        QPushButton {
            background: #2563eb;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 7px 12px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #3b82f6;
        }
        QPushButton:pressed {
            background: #1d4ed8;
        }
        QTableWidget {
            background: #0f172a;
            color: #e5e7eb;
            gridline-color: #334155;
            border: 1px solid #374151;
            border-radius: 10px;
            alternate-background-color: #111827;
            selection-background-color: #1d4ed8;
            selection-color: #ffffff;
            font-size: 13px;
        }
        QHeaderView::section {
            background: #1f2937;
            color: #f9fafb;
            padding: 8px;
            border: none;
            border-right: 1px solid #374151;
            font-weight: 700;
        }
        QStatusBar {
            background: #0f172a;
            color: #cbd5e1;
        }
        QMessageBox {
            background: #f3f4f6;
        }
        QMessageBox QLabel {
            color: #111827;
        }
        QMessageBox QPushButton {
            min-width: 80px;
            padding: 6px 10px;
        }
    )");
}

void MainWindow::setupConnections()
{
    connect(addBookButton, &QPushButton::clicked, this, &MainWindow::addBook);
    connect(requestBookButton, &QPushButton::clicked, this, &MainWindow::requestBook);
    connect(returnBookButton, &QPushButton::clicked, this, &MainWindow::returnBook);
    connect(processRequestsButton, &QPushButton::clicked, this, &MainWindow::processRequests);
    connect(processReturnsButton, &QPushButton::clicked, this, &MainWindow::processReturns);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshTable);
}

void MainWindow::showMessage(const QString &title, const QString &text, QMessageBox::Icon icon)
{
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(title);
    messageBox.setText(text);
    messageBox.setIcon(icon);
    // Ensure dialogs stay readable even with the app dark theme.
    messageBox.setStyleSheet(R"(
        QMessageBox {
            background: #f3f4f6;
        }
        QMessageBox QLabel {
            color: #111827;
            font-size: 13px;
        }
        QMessageBox QPushButton {
            min-width: 80px;
            padding: 6px 10px;
            border-radius: 6px;
        }
    )");
    messageBox.exec();
}

void MainWindow::addBook()
{
    const QString bookIdText = bookIdEdit->text().trimmed();
    const QString titleText = titleEdit->text().trimmed();
    const QString authorText = authorEdit->text().trimmed();

    if (bookIdText.isEmpty() || titleText.isEmpty() || authorText.isEmpty()) {
        showMessage(tr("Missing Data"), tr("Please provide Book ID, Title, and Author."), QMessageBox::Warning);
        return;
    }

    bool ok = false;
    const int bookId = bookIdText.toInt(&ok);
    if (!ok || bookId <= 0) {
        showMessage(tr("Invalid Book ID"), tr("Book ID must be a positive integer."), QMessageBox::Warning);
        return;
    }

    if (bookList.searchBook(bookId) != nullptr) {
        showMessage(tr("Duplicate Book"), tr("A book with this ID already exists."), QMessageBox::Warning);
        return;
    }

    bookList.addBook(bookId, titleText.toStdString(), authorText.toStdString());

    bookIdEdit->clear();
    titleEdit->clear();
    authorEdit->clear();
    refreshTable();
    statusBar()->showMessage(tr("Book added"), 3000);
}

void MainWindow::requestBook()
{
    QString bookIdText = bookIdEdit->text().trimmed();
    QString usernameText = usernameEdit->text().trimmed();

    // Allow using the selected row when Book ID input is empty.
    if (bookIdText.isEmpty()) {
        const int selectedRow = booksTable->currentRow();
        if (selectedRow >= 0) {
            QTableWidgetItem *item = booksTable->item(selectedRow, kBookIdColumn);
            if (item != nullptr) {
                bookIdText = item->text().trimmed();
            }
        }
    }

    if (bookIdText.isEmpty()) {
        showMessage(tr("Missing Data"), tr("Please provide Book ID or select a book row first."), QMessageBox::Warning);
        bookIdEdit->setFocus();
        return;
    }

    // Username is optional; fallback to Guest.
    if (usernameText.isEmpty()) {
        usernameText = tr("Guest");
        usernameEdit->setText(usernameText);
    }

    bool ok = false;
    const int bookId = bookIdText.toInt(&ok);
    if (!ok || bookId <= 0) {
        showMessage(tr("Invalid Book ID"), tr("Book ID must be a positive integer."), QMessageBox::Warning);
        return;
    }

    requestQueue.enqueue(usernameText.toStdString(), bookId);
    usernameEdit->clear();
    statusBar()->showMessage(tr("Request queued"), 3000);
    populateRequestsTable();
    saveQueue();
}

void MainWindow::returnBook()
{
    QString bookIdText = bookIdEdit->text().trimmed();

    // Allow returning the currently selected book row when the input is empty.
    if (bookIdText.isEmpty()) {
        const int selectedRow = booksTable->currentRow();
        if (selectedRow >= 0) {
            QTableWidgetItem *item = booksTable->item(selectedRow, kBookIdColumn);
            if (item != nullptr) {
                bookIdText = item->text().trimmed();
            }
        }
    }

    if (bookIdText.isEmpty()) {
        showMessage(tr("Missing Data"), tr("Please provide Book ID or select a book row first."), QMessageBox::Warning);
        bookIdEdit->setFocus();
        return;
    }

    bool ok = false;
    const int bookId = bookIdText.toInt(&ok);
    if (!ok || bookId <= 0) {
        showMessage(tr("Invalid Book ID"), tr("Book ID must be a positive integer."), QMessageBox::Warning);
        return;
    }

    returnStack.push(bookId);
    statusBar()->showMessage(tr("Return added to stack"), 3000);
    populateReturnsTable();
    saveReturns();
}

void MainWindow::processRequests()
{
    if (requestQueue.isEmpty()) {
        showMessage(tr("Queue Empty"), tr("There are no pending requests to process."), QMessageBox::Information);
        return;
    }

    int processedCount = 0;
    int rejectedCount = 0;

    Request request;
    while (requestQueue.dequeue(request)) {
        if (bookList.processRequest(request.bookID)) {
            ++processedCount;
        } else {
            ++rejectedCount;
        }
    }
    refreshTable();
    populateRequestsTable();
    saveQueue();
    statusBar()->showMessage(tr("Processed %1 request(s), rejected %2.").arg(processedCount).arg(rejectedCount), 5000);

    if (rejectedCount > 0) {
        showMessage(tr("Requests Processed"),
                    tr("Finished processing the queue. Some requests could not be fulfilled because the book was missing or unavailable."),
                    QMessageBox::Warning);
    } else {
        showMessage(tr("Requests Processed"), tr("All pending requests were processed."), QMessageBox::Information);
    }
}

void MainWindow::processReturns()
{
    if (returnStack.isEmpty()) {
        showMessage(tr("Stack Empty"), tr("There are no pending returns to process."), QMessageBox::Information);
        return;
    }

    int processedCount = 0;
    int rejectedCount = 0;

    while (!returnStack.isEmpty()) {
        const int bookId = returnStack.pop();
        if (bookList.processReturn(bookId)) {
            ++processedCount;
        } else {
            ++rejectedCount;
        }
    }
    refreshTable();
    populateReturnsTable();
    saveReturns();
    statusBar()->showMessage(tr("Processed %1 return(s), rejected %2.").arg(processedCount).arg(rejectedCount), 5000);

    if (rejectedCount > 0) {
        showMessage(tr("Returns Processed"),
                    tr("Finished processing the return stack. Some books were already available or not found."),
                    QMessageBox::Warning);
    } else {
        showMessage(tr("Returns Processed"), tr("All pending returns were processed."), QMessageBox::Information);
    }
}

void MainWindow::refreshTable()
{
    const std::vector<BookInfo> books = bookList.getAllBooks();

    booksTable->setRowCount(static_cast<int>(books.size()));

    for (int row = 0; row < static_cast<int>(books.size()); ++row) {
        const BookInfo &book = books[static_cast<std::size_t>(row)];

        auto *idItem = new QTableWidgetItem(QString::number(book.bookID));
        auto *titleItem = new QTableWidgetItem(QString::fromStdString(book.title));
        auto *authorItem = new QTableWidgetItem(QString::fromStdString(book.author));
        auto *statusItem = new QTableWidgetItem(book.isAvailable ? tr("Available") : tr("Borrowed"));

        idItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setTextAlignment(Qt::AlignCenter);

        if (!book.isAvailable) {
            statusItem->setForeground(QBrush(QColor("#f59e0b")));
        } else {
            statusItem->setForeground(QBrush(QColor("#22c55e")));
        }

        booksTable->setItem(row, kBookIdColumn, idItem);
        booksTable->setItem(row, kTitleColumn, titleItem);
        booksTable->setItem(row, kAuthorColumn, authorItem);
        booksTable->setItem(row, kStatusColumn, statusItem);
    }

    booksTable->resizeRowsToContents();
}

void MainWindow::populateRequestsTable()
{
    const auto requests = requestQueue.getPendingRequests();
    requestsTable->setRowCount(static_cast<int>(requests.size()));
    for (int i = 0; i < static_cast<int>(requests.size()); ++i) {
        const Request &r = requests[static_cast<std::size_t>(i)];
        auto *userItem = new QTableWidgetItem(QString::fromStdString(r.userName));
        auto *idItem = new QTableWidgetItem(QString::number(r.bookID));
        userItem->setTextAlignment(Qt::AlignCenter);
        idItem->setTextAlignment(Qt::AlignCenter);
        requestsTable->setItem(i, 0, userItem);
        requestsTable->setItem(i, 1, idItem);
    }
    requestsTable->resizeRowsToContents();
}

void MainWindow::populateReturnsTable()
{
    const auto returns = returnStack.getPendingReturns();
    returnsTable->setRowCount(static_cast<int>(returns.size()));
    for (int i = 0; i < static_cast<int>(returns.size()); ++i) {
        int id = returns[static_cast<std::size_t>(i)];
        auto *idItem = new QTableWidgetItem(QString::number(id));
        idItem->setTextAlignment(Qt::AlignCenter);
        returnsTable->setItem(i, 0, idItem);
    }
    returnsTable->resizeRowsToContents();
}

void MainWindow::loadQueue()
{
    const auto path = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "queue.json";
    QFile file(path);
    if (!file.exists())
        return;
    if (!file.open(QIODevice::ReadOnly))
        return;

    const QByteArray bytes = file.readAll();
    file.close();
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isArray())
        return;

    // clear existing queue
    Request tmp;
    while (requestQueue.dequeue(tmp)) {}

    const QJsonArray arr = doc.array();
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        const QJsonObject obj = v.toObject();
        const QString user = obj.value("user").toString();
        const int bookId = obj.value("bookID").toInt();
        requestQueue.enqueue(user.toStdString(), bookId);
    }
    populateRequestsTable();
}

void MainWindow::saveQueue()
{
    const auto requests = requestQueue.getPendingRequests();
    QJsonArray arr;
    for (const Request &r : requests) {
        QJsonObject obj;
        obj["user"] = QString::fromStdString(r.userName);
        obj["bookID"] = r.bookID;
        arr.append(obj);
    }

    const QJsonDocument doc(arr);
    const auto path = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "queue.json";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

void MainWindow::loadReturns()
{
    const auto path = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "returns.json";
    QFile file(path);
    if (!file.exists())
        return;
    if (!file.open(QIODevice::ReadOnly))
        return;

    const QByteArray bytes = file.readAll();
    file.close();
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isArray())
        return;

    // clear existing stack
    while (!returnStack.isEmpty()) returnStack.pop();

    const QJsonArray arr = doc.array();
    // arr expected: top element first; push in reverse to restore same order
    for (int i = static_cast<int>(arr.size()) - 1; i >= 0; --i) {
        const QJsonValue &v = arr.at(i);
        const int id = v.toInt();
        if (id <= 0) continue;  // Validate that we got a valid book ID
        returnStack.push(id);
    }
    populateReturnsTable();
}

void MainWindow::saveReturns()
{
    const auto returns = returnStack.getPendingReturns();
    QJsonArray arr;
    for (int id : returns) arr.append(id);

    const QJsonDocument doc(arr);
    const auto path = QDir(QCoreApplication::applicationDirPath()).filePath("..") + QDir::separator() + "returns.json";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}
