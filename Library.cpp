#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <cctype> // For std::tolower
#include <limits> // For numeric_limits


using namespace std;

// Book structure
struct Book {
    string title;
    int availableCopies;
};

// Utility function to get current date
string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return to_string(1900 + ltm->tm_year) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(ltm->tm_mday);
}

// Convert string to lowercase for case-insensitive comparisons
string toLowerCase(const string &str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to load books from file
void loadBooks(vector<Book> &books) {
    ifstream inFile("books.txt");
    if (!inFile) return;

    books.clear();
    string title;
    int copies;
    while (getline(inFile, title, ';') && (inFile >> copies)) {
        inFile.ignore(); // Ignore the newline after the number
        books.push_back({title, copies});
    }
    inFile.close();
}

// Function to save books to file
void saveBooks(const vector<Book> &books) {
    ofstream outFile("books.txt");
    for (const auto &book : books) {
        outFile << book.title << ";" << book.availableCopies << endl;
    }
    outFile.close();
}

// Function to load borrowed books data
void loadBorrowedBooks(map<string, vector<string>> &borrowedBooks) {
    ifstream inFile("borrowed.txt");
    if (!inFile) return;

    borrowedBooks.clear();
    string title, borrower;
    while (getline(inFile, title, ';') && getline(inFile, borrower)) {
        borrowedBooks[title].push_back(borrower);
    }
    inFile.close();
}

// Function to save borrowed books data
void saveBorrowedBooks(const map<string, vector<string>> &borrowedBooks) {
    ofstream outFile("borrowed.txt");
    for (const auto &entry : borrowedBooks) {
        for (const auto &borrower : entry.second) {
            outFile << entry.first << ";" << borrower << endl;
        }
    }
    outFile.close();
}

// Function to view books in alphabetical order
void viewBooks(const vector<Book> &books) {
    cout << "\nBooks in the Library (Alphabetical Order):\n";
    vector<Book> sortedBooks = books;
    sort(sortedBooks.begin(), sortedBooks.end(), [](const Book &a, const Book &b) {
        return a.title < b.title;
    });

    for (const auto &book : sortedBooks) {
        cout << "- " << book.title << " (" << book.availableCopies << " available)\n";
    }
}

// Function to borrow a book
void borrowBook(vector<Book> &books, map<string, vector<string>> &borrowedBooks) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    string title, borrower;
    cout << "Enter book title to borrow: ";
    getline(cin, title);

    cout << "Enter your name: ";
    getline(cin, borrower);

    string lowerTitle = toLowerCase(title);
    auto it = find_if(books.begin(), books.end(), [&](const Book &b) {
        return toLowerCase(b.title) == lowerTitle;
    });

    if (it != books.end() && it->availableCopies > 0) {
        if (borrowedBooks[it->title].size() < 3) {
            it->availableCopies--;
            borrowedBooks[it->title].push_back(borrower);
            cout << "Book borrowed successfully!\n";
        } else {
            cout << "All 3 copies of this book are already borrowed.\n";
        }
    } else {
        cout << "Book not available in the library or all copies are borrowed.\n";
    }
}

// Function to return a book
void returnBook(vector<Book> &books, map<string, vector<string>> &borrowedBooks) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    string title, borrower;
    cout << "Enter book title to return: ";
    getline(cin, title);

    cout << "Enter your name: ";
    getline(cin, borrower);

    string lowerTitle = toLowerCase(title);
    auto it = find_if(borrowedBooks.begin(), borrowedBooks.end(), [&](const pair<string, vector<string>> &entry) {
        return toLowerCase(entry.first) == lowerTitle;
    });

    if (it != borrowedBooks.end()) {
        auto personIt = find(it->second.begin(), it->second.end(), borrower);
        if (personIt != it->second.end()) {
            it->second.erase(personIt);

            auto bookIt = find_if(books.begin(), books.end(), [&](const Book &b) {
                return toLowerCase(b.title) == lowerTitle;
            });

            if (bookIt != books.end()) {
                bookIt->availableCopies++;
            }

            cout << "Book returned successfully on " << getCurrentDate() << ".\n";

            if (it->second.empty()) {
                borrowedBooks.erase(it);
            }
        } else {
            cout << "This book was not borrowed by you.\n";
        }
    } else {
        cout << "Book not found in the borrowed list.\n";
    }
}

// Main Menu
void libraryMenu() {
    vector<Book> books;
    map<string, vector<string>> borrowedBooks;

    loadBooks(books);
    loadBorrowedBooks(borrowedBooks);

    int choice;
    do {
        cout << "\nLibrary Management System\n";
        cout << "1. View Books\n";
        cout << "2. Borrow Book\n";
        cout << "3. Return Book\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                viewBooks(books);
                break;
            case 2:
                borrowBook(books, borrowedBooks);
                break;
            case 3:
                returnBook(books, borrowedBooks);
                break;
            case 4:
                saveBooks(books);
                saveBorrowedBooks(borrowedBooks);
                cout << "Exiting the program. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 4);
}

int main() {
    libraryMenu();
    return 0;
}
