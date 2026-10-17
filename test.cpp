#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <limits>
#include <cmath>
#include <conio.h>

using namespace std;

struct BorrowRecord {
    string recordID;
    string memberID;
    string bookID;
    string borrowDate;
    string dueDate;
    string returnDate;
    string status;
    int renewCount = 0;
    double fineAmount = 0.0;
    string paymentStatus = "N/A";
    double amountPaid = 0.0;   // NEW: tracks money already collected on this record
};
vector<BorrowRecord> borrowRecords;

struct Member {
    string memberID;
    string memberName;
    string phoneNum;
    string email;
    string membershipType;
    string membershipStatus;
};
vector<Member> members;

struct Book {
    string bookID;
    string bookTitle;
    string author;
    string isbn;
    string category;
    int quantity = 0;
    int availableCopies = 0;
};
vector<Book> books;

const string BORROW_FILE = "borrow.txt";
const string BOOKS_FILE = "books.txt";
const int LOAN_PERIOD_DAYS = 14;
const int MAX_RENEW_COUNT = 2;
const double FINE_RATE_PER_DAY = 1.00;
const double FINE_EPSILON = 0.001;

// Admin Functions
// Functions available only to administrators
void adminBorrowMenu(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records);
void adminBorrowBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records);
void adminReturnBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records);
void adminRenewBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records);
void viewAllBorrowRecords(const vector<BorrowRecord>& records, const vector<Book>& books);
void viewOverdueBooks(const vector<Book>& books, const vector<Member>& members, const vector<BorrowRecord>& records);

// Member Functions
// Functions available only to registered members
void memberBorrowMenu(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records);
void memberBorrowBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records);
void memberReturnBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records);
void memberRenewBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records);
void viewMyBorrowHistory(const string& memberID, const vector<BorrowRecord>& records, const vector<Book>& books);
void viewMyBorrowedBooks(const string& memberID, const vector<BorrowRecord>& records, const vector<Book>& books);

// Common Functions
// Functions used by both admin and members
void checkAvailability(const vector<Book>& books);
void displayBorrowRecordTable(const vector<BorrowRecord>& records, const vector<Book>& books);

// File Operations
void loadBorrowRecords(vector<BorrowRecord>& records);
void saveBorrowRecords(const vector<BorrowRecord>& records);

//Helper Functions
string generateRecordID(const vector<BorrowRecord>& records);
string getCurrentDate();
string calculateDueDate(int daysToAdd);
string getMemberName(const string& memberID, const vector<Member>& members);
string getBookTitle(const string& bookID, const vector<Book>& books);
string toUpperCase(string text);
bool isMemberActive(const string& memberID, const vector<Member>& members);
bool isBookAvailable(const string& bookID, const vector<Book>& books, int quantityNeeded = 1);
void updateOverdueStatus();
void initBorrowReturn();
void clearScreen();
void pauseScreen();
int daysBetweenDates(const string& date1, const string& date2);
int getValidatedChoice(int minVal, int maxVal);

//MEMBER MANAGEMENT//
void addMember();
void displayMembers();
void searchMember();
void updateMember();
void deleteMember();
void memberManagement();
void viewProfile(const string& memberID);
void editProfile(const string& memberID);
void adminMenu();
void memberMenu(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records);

// file handling functions
void saveMembers();
void loadMembers();

string generateMemberID();
bool validName(const string& name, string& errorMessage);
bool validPhone(const string& phone, string& errorMessage);
bool validEmail(const string& email, string& errorMessage);

//login module
string getHiddenPassword();
void loginScreen();
bool login(string& loggedInRole, string& loggedInMemberID);

//BOOK MANAGEMENT//
void bookMenu();
void addBook(vector<Book>& books);
void updateBook(vector<Book>& books);
void deleteBook(vector<Book>& books);
void searchBook(const vector<Book>& books);
void displayAllBook(const vector<Book>& books);

// file handling functions
void loadBooksFromFile(vector<Book>& books);
void saveBooksToFile(const vector<Book>& books);
int safeStoiFromFile(const string& str, int defaultValue = 0);

//Helper Functions(Book Management)
string generateBookID(const vector<Book>& books);
bool isValidBookID(const string& id);
bool isValidPositiveNumber(const string& str);
int findBookIndex(const vector<Book>& books, const string& bookID);
int safeStoi(const string& str);
bool isWhitespaceOnly(const string& str);

//REPORTING & FINE MANAGEMENT//
void adminReportingAndFineMenu();
void memberReportingAndFineMenu(const string& memberID);
void reportingAndFineMenu();
void calculateOverdueFine();
void payFine();
void payFine(const string& memberID);
void displayBorrowingReport();
void displayPopularBooksReport();
void displayLibraryStatistics();
double calculateFineAmount(const string& dueDate, const string& returnDate);
bool applyFineRules(BorrowRecord& record, const string& asOfDate);


//Main Funtion//
int main() {
    loadBooksFromFile(books);
    updateOverdueStatus();
    loadBorrowRecords(borrowRecords);

    string memberID;
    string loggedInRole;
    string loggedInMemberID;

    while (!login(loggedInRole, loggedInMemberID)) {
    }

    clearScreen();

    if (loggedInRole == "Admin") {
        adminMenu();
    }
    else if (loggedInRole == "Member") {
        memberMenu(loggedInMemberID, books, borrowRecords);
    }
    else {
        cout << "\nInvalid user role.\n";
    }

    cout << "\nThank you for using the Smart Library Management System.\n";

    return 0;
}

//BORROW & RETURN MANAGEMENT//
//admin
void adminBorrowMenu(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records) {
    // Load existing data
    loadBooksFromFile(books);
    loadBorrowRecords(records);
    loadMembers();
    int choice;
    do {
        clearScreen();
        cout << "\n=============================================\n";
        cout << "         ADMIN - BORROW MANAGEMENT\n";
        cout << "=============================================\n";
        cout << "1. Borrow Book for Member\n";
        cout << "2. Process Book Return\n";
        cout << "3. Process Book Renewal\n";
        cout << "4. View All Borrow Records\n";
        cout << "5. View Overdue Books\n";
        cout << "6. Check Book Availability\n";
        cout << "0. Back\n";
        cout << "=============================================\n";

        choice = getValidatedChoice(0, 6);
        cin.ignore();

        switch (choice) {
        case 0:
            return;
        case 1:
            adminBorrowBook(books, members, records);
            break;
        case 2:
            adminReturnBook(books, members, records);
            break;
        case 3:
            adminRenewBook(books, members, records);
            break;
        case 4:
            viewAllBorrowRecords(records, books);
            pauseScreen();
            break;
        case 5:
            viewOverdueBooks(books, members, records);
            pauseScreen();
            break;
        case 6:
            checkAvailability(books);
            pauseScreen();
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            pauseScreen();
        }
    } while (choice != 0);
}
void adminBorrowBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "       ADMIN - BORROW BOOK FOR MEMBER\n";
    cout << "=============================================\n";

    // Check if system has members and books
    if (members.empty()) {
        cout << "\nNo members in the system. Please add members first.\n";
        return;
    }

    if (books.empty()) {
        cout << "\nNo books in the system. Please add books first.\n" << endl;
        cin.get();
        return;
    }

    loadBorrowRecords(records);

    string memberID;
    bool memberFound = false;

    // Display all members for reference
    cout << "Member List:" << endl;
    cout << string(50, '-') << endl;
    cout << left << setw(10) << " Member ID" << " | "
        << setw(25) << "Name" << " | "
        << setw(15) << "Status" << endl;
    cout << string(50, '-') << endl;

    for (const Member& member : members) {
        cout << left << "   " << setw(7) << member.memberID << " | "
            << setw(25) << member.memberName << " | "
            << setw(15) << member.membershipStatus << endl;
    }
    cout << string(50, '-') << endl;

    // Get and validate member ID
    do {
        cout << "\nEnter Member ID (0 to cancel): ";
        cin >> memberID;
        cin.get();
        memberID = toUpperCase(memberID);

        if (memberID == "0") {
            cout << "\nBorrow operation cancelled.\n";
            return;
        }

        if (memberID.empty() || isWhitespaceOnly(memberID)) {
            cout << "Input cannot be empty. Please try again.\n";
            continue;
        }

        for (const Member& member : members) {
            if (member.memberID == memberID) {
                memberFound = true;
                break;
            }
        }

        if (!memberFound) {
            cout << "Member not found. Please enter a valid Member ID.\n";
        }
    } while (!memberFound);

    // Check if member is active
    if (!isMemberActive(memberID, members)) {
        cout << "\nMember account is not active. Cannot borrow books.\n";
        cout << "\nPlease press \"Enter\" key to Continue.....";
        cin.get();
        return;
    }

    // Check if member has overdue books
    bool hasOverdue = false;
    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID && record.status == "Overdue") {
            hasOverdue = true;
            break;
        }
    }

    if (hasOverdue) {
        cout << "\nMember has overdue books. Please return them before borrowing new books.\n";
        cout << "\nPlease press \"Enter\" key to Continue.....";
        cin.get();
        return;
    }
    clearScreen();

    // Display available books
    cout << "\n=============================================" << endl;
    cout << "              Available Books" << endl;
    cout << "=============================================" << endl;
    cout << string(120, '-') << endl;
    cout << left << setw(10) << "  Book ID" << " | "
        << setw(50) << "Title" << " | "
        << setw(40) << "Author" << " | "
        << setw(10) << "Available" << endl;
    cout << string(120, '-') << endl;

    bool hasAvailableBooks = false;
    for (const Book& book : books) {
        if (book.availableCopies > 0) {
            hasAvailableBooks = true;
            cout << "   " << left << setw(7) << book.bookID << " | "
                << setw(50) << book.bookTitle << " | "
                << setw(40) << book.author << " | "
                << right
                << setw(5) << book.availableCopies << endl;
        }
    }

    if (!hasAvailableBooks) {
        cout << "\nNo books are currently available for borrowing.\n";
        return;
    }
    cout << string(120, '-') << endl;

    // Get and validate book ID
    string bookID;
    bool validBook = false;

    do {
        cout << "\nEnter Book ID to borrow (0 to cancel): ";
        getline(cin, bookID);
        bookID = toUpperCase(bookID);

        if (bookID == "0") {
            cout << "\nBorrow operation cancelled.\n";
            return;
        }

        // Check if Book ID exists
        bool bookFound = false;

        for (const Book& book : books) {
            if (book.bookID == bookID) {
                bookFound = true;
                break;
            }
        }

        if (!bookFound) {
            cout << "Book not found. Please enter a valid Book ID.\n";
            continue;
        }

        // Check book availability
        if (!isBookAvailable(bookID, books, 1)) {
            cout << "\nThis book is currently not available for borrowing.\n";
            cout << "Please enter another Book ID.\n";
            continue;
        }

        // Check if member already borrowed this book
        bool alreadyBorrowed = false;

        for (const BorrowRecord& record : records) {
            if (record.memberID == memberID &&
                record.bookID == bookID &&
                record.status != "Returned") {

                alreadyBorrowed = true;
                break;
            }
        }

        if (alreadyBorrowed) {
            cout << "\nMember has already borrowed this book. Cannot borrow again.\n";
            cout << "Please enter another Book ID.\n";
            continue;
        }

        // All checks passed
        validBook = true;

    } while (!validBook);

    // Create new borrow record
    BorrowRecord newRecord;
    newRecord.recordID = generateRecordID(records);
    newRecord.memberID = memberID;
    newRecord.bookID = bookID;
    newRecord.borrowDate = getCurrentDate();
    newRecord.dueDate = calculateDueDate(LOAN_PERIOD_DAYS);
    newRecord.returnDate = "";
    newRecord.status = "Borrowed";
    newRecord.renewCount = 0;
    newRecord.fineAmount = 0.0;
    newRecord.paymentStatus = "N/A";
    newRecord.amountPaid = 0.0;

    // Display confirmation
    cout << "\n---------------------------------------------\n";
    cout << "           Borrow Confirmation\n";
    cout << "---------------------------------------------\n";
    cout << "Member ID   : " << newRecord.memberID << endl;
    cout << "Member Name : " << getMemberName(memberID, members) << endl;
    cout << "Book ID     : " << newRecord.bookID << endl;
    cout << "Book Title  : " << getBookTitle(newRecord.bookID, books) << endl;
    cout << "Borrow Date : " << newRecord.borrowDate << endl;
    cout << "Due Date    : " << newRecord.dueDate << endl;
    cout << "---------------------------------------------\n";

    string confirm;

    do {
        cout << "\nDo you want to borrow this book? (y/n): ";
        getline(cin, confirm);

        if (confirm == "y" || confirm == "Y") {
            records.push_back(newRecord);
            saveBorrowRecords(records);

            for (Book& book : books) {
                if (book.bookID == bookID) {
                    book.availableCopies--;
                    break;
                }
            }

            saveBooksToFile(books);

            cout << "\nBook Borrowed Successfully!\n";
            cout << "---------------------------------------------\n";
            cout << "Record ID   : " << newRecord.recordID << endl;
            cout << "Due Date    : " << newRecord.dueDate << endl;
            cout << "Please remind member to return on time.\n";
            cout << "---------------------------------------------\n";

            cout << "\nPlease press \"Enter\" key to continue";
            cin.get();

            break;
        }
        else if (confirm == "n" || confirm == "N") {
            cout << "\nBorrow cancelled." << endl;
            cout << "\nPlease press \"Enter\" key to continue";
            cin.get();

            clearScreen();
            break;
        }
        else {
            cout << "Please enter 'y' for yes or 'n' for no!" << endl;
        }
    } while (true);
    return;
}
void adminReturnBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "        ADMIN - PROCESS BOOK RETURN\n";
    cout << "=============================================\n";

    loadBorrowRecords(records);

    // Find all active borrow records (not returned)
    vector<BorrowRecord> activeRecords;
    for (const BorrowRecord& record : records) {
        if (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed") {
            activeRecords.push_back(record);
        }
    }

    if (activeRecords.empty()) {
        cout << "\nNo books are currently borrowed.\n" << endl;
        cin.get();
        return;
    }

    // Display active borrow records
    cout << "Currently borrowed books:" << endl;
    displayBorrowRecordTable(activeRecords, books);

    string recordID;
    bool found = false;

    do {
        cout << "\nEnter Record ID to return (0 to cancel): ";
        getline(cin, recordID);
        recordID = toUpperCase(recordID);

        if (recordID == "0") {
            return;
        }
        else if (recordID.empty() || isWhitespaceOnly(recordID)) {
            cout << "Record ID cannot be empty!\n";
            continue;
        }

        for (const BorrowRecord& record : records) {
            if (record.recordID == recordID &&
                (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed")) {
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Record not found or already returned. Please enter a valid Record ID.\n";
        }
    } while (!found);

    // Process the return
    for (BorrowRecord& record : records) {
        if (record.recordID == recordID) {
            string returnDate = getCurrentDate();

            BorrowRecord preview = record;
            applyFineRules(preview, returnDate);
            int daysLate = daysBetweenDates(record.dueDate, returnDate);

            // Display return details
            cout << "\n---------------------------------------------\n";
            cout << "              Return Details\n";
            cout << "---------------------------------------------\n";
            cout << "Book Title  : " << getBookTitle(record.bookID, books) << endl;
            cout << "Member Name : " << getMemberName(record.memberID, members) << endl;
            cout << "Borrow Date : " << record.borrowDate << endl;
            cout << "Due Date    : " << record.dueDate << endl;
            cout << "Return Date : " << returnDate << endl;

            // Check if overdue and show the fine that will apply
            if (daysLate > 0) {
                cout << "\n*** BOOK IS OVERDUE (" << daysLate << " day(s)) ***\n";
                if (record.paymentStatus == "Paid") {
                    cout << "Fine Amount : RM " << fixed << setprecision(2)
                        << record.fineAmount << " (already paid)\n";
                }
                else {
                    cout << "Fine Amount : RM " << fixed << setprecision(2)
                        << preview.fineAmount << endl;
                    cout << "Payment Status : " << preview.paymentStatus << endl;
                }
            }
            else {
                cout << "\nBook returned on time.\n";
            }
            cout << "---------------------------------------------\n";

            string confirm;

            do {
                cout << "\nDo you want to save this return? (y/n): ";
                getline(cin, confirm);

                if (confirm == "y" || confirm == "Y") {
                    record.returnDate = returnDate;
                    record.status = "Returned";

                    applyFineRules(record, returnDate);
                    saveBorrowRecords(records);

                    for (Book& book : books) {
                        if (book.bookID == record.bookID) {
                            book.availableCopies++;
                            break;
                        }
                    }

                    saveBooksToFile(books);

                    cout << "\nBook Returned Successfully!" << endl;
                    cout << "---------------------------------------------\n";
                    cout << "Record ID   : " << record.recordID << endl;
                    cout << "Return Date : " << returnDate << endl;

                    if (record.paymentStatus == "Unpaid" &&
                        record.fineAmount > FINE_EPSILON) {

                        cout << "Fine Due    : RM "
                            << fixed << setprecision(2)
                            << record.fineAmount << endl;

                        cout << "Use Reporting & Fine Management to collect payment."
                            << endl;
                    }

                    cout << "---------------------------------------------\n";

                    cout << "\nPlease press \"Enter\" key to Continue.....";
                    cin.get();

                    break;
                }
                else if (confirm == "n" || confirm == "N") {

                    cout << "\nReturn cancelled." << endl;
                    cout << "\nPlease press \"Enter\" key  to Continue.....";

                    cin.get();
                    clearScreen();

                    break;
                }
                else {
                    cout << "Please enter 'y' for yes or 'n' for no!" << endl;
                }

            } while (true);
            return;
        }
    }
}
void adminRenewBook(vector<Book>& books, vector<Member>& members, vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "        ADMIN - PROCESS BOOK RENEWAL\n";
    cout << "=============================================\n";

    loadBorrowRecords(records);

    // Find renewable records (borrowed status, not maxed out on renewals)
    vector<BorrowRecord> renewableRecords;
    for (const BorrowRecord& record : records) {
        if ((record.status == "Borrowed" || record.status == "Renewed") &&
            record.renewCount < MAX_RENEW_COUNT) {
            renewableRecords.push_back(record);
        }
    }

    if (renewableRecords.empty()) {
        cout << "\nNo books are eligible for renewal.\n";
        cout << "\nBooks must be currently borrowed and not exceed maximum renewals.\n";
        cin.get();
        return;
    }

    cout << "BOOKS ELIGIBLE FOR RENEWAL:\n";
    displayBorrowRecordTable(renewableRecords, books);

    string recordID;
    bool found = false;

    do {
        cout << "\nEnter Record ID to renew (0 to cancel): ";
        getline(cin, recordID);
        recordID = toUpperCase(recordID);

        if (recordID == "0") {
            cout << "\nRenewal operation cancelled.\n";
            return;
        }
        else if (recordID.empty() || isWhitespaceOnly(recordID)) {
            cout << "Record ID cannot be empty!\n";
            continue;
        }

        for (const BorrowRecord& record : records) {
            if (record.recordID == recordID &&
                (record.status == "Borrowed" || record.status == "Renewed")) {
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Record not found or not eligible for renewal.\n";
        }
    } while (!found);

    // Process the renewal
    for (BorrowRecord& record : records) {
        if (record.recordID == recordID) {
            // Check renewal limit
            if (record.renewCount >= MAX_RENEW_COUNT) {
                cout << "\nMaximum renewals (" << MAX_RENEW_COUNT << ") reached.\n";
                pauseScreen();
                return;
            }

            string newDueDate = calculateDueDate(LOAN_PERIOD_DAYS);

            cout << "\n---------------------------------------------\n";
            cout << "           Renew Confirmation\n";
            cout << "---------------------------------------------\n";
            cout << "Book Title  : " << getBookTitle(record.bookID, books) << endl;
            cout << "Member Name : " << getMemberName(record.memberID, members) << endl;
            cout << "Current Due : " << record.dueDate << endl;
            cout << "New Due Date: " << newDueDate << endl;
            cout << "Renewal #   : " << (record.renewCount + 1) << " of " << MAX_RENEW_COUNT << endl;
            cout << "---------------------------------------------\n";

            string confirm;

            do {
                cout << "\nDo you want to save this renewal? (y/n): ";
                getline(cin, confirm);

                if (confirm == "y" || confirm == "Y") {
                    record.dueDate = newDueDate;
                    record.renewCount++;
                    record.status = "Renewed";

                    saveBorrowRecords(records);

                    cout << "\nBook Renewed Successfully!\n";
                    cout << "---------------------------------------------\n";
                    cout << "New Due Date        : " << newDueDate << endl;
                    cout << "Renewals Used       : " << record.renewCount
                        << " of " << MAX_RENEW_COUNT << endl;
                    cout << "Renewals Remaining  : "
                        << (MAX_RENEW_COUNT - record.renewCount) << endl;
                    cout << "---------------------------------------------\n";

                    cout << "\nPlease press \"Enter\" key back to Continue.....";
                    cin.get();

                    break;
                }
                else if (confirm == "n" || confirm == "N") {

                    cout << "\nRenewal operation cancelled." << endl;
                    cout << "\nPlease press \"Enter\" key back to Continue.....";

                    cin.get();
                    clearScreen();

                    break;
                }
                else {
                    cout << "Please enter 'y' for yes or 'n' for no!" << endl;
                }

            } while (true);
            return;
        }
    }
}
void viewAllBorrowRecords(const vector<BorrowRecord>& borrowRecords, const vector<Book>& books) {
    clearScreen();

    if (borrowRecords.empty()) {
        cout << "\nNo borrow records found.\n";
        cout << "\nPlease press \"Enter\" key back to Book Menu";
        cin.get();
        return;
    }
    else {
        cout << "\n=============================================\n";
        cout << "           ALL BORROW RECORDS\n";
        cout << "=============================================\n";



        // Calculate statistics
        int totalBorrowed = 0;
        int totalReturned = 0;
        int totalOverdue = 0;

        for (const BorrowRecord& record : borrowRecords) {
            if (record.status == "Borrowed" || record.status == "Renewed" || record.status == "Overdue") {
                totalBorrowed++;
            }
            if (record.status == "Returned") {
                totalReturned++;
            }
            if (record.status == "Overdue") {
                totalOverdue++;
            }
        }

        // Display statistics
        cout << "\n            STATISTICS\n";
        cout << "    ----------------------------\n";
        cout << "   | Total Records      : " << right << setw(4) << borrowRecords.size() << "  |" << endl;
        cout << "   | Currently Borrowed : " << right << setw(4) << totalBorrowed << "  |" << endl;
        cout << "   | Returned           : " << right << setw(4) << totalReturned << "  |" << endl;
        cout << "   | Overdue            : " << right << setw(4) << totalOverdue << "  |" << endl;
        cout << "    ----------------------------\n\n";

        // Display all records in a table
        cout << string(161, '-') << endl;
        cout << left
            << setw(10) << " Record ID" << " | "
            << setw(10) << " Member ID" << " | "
            << setw(10) << "  Book ID" << " | "
            << setw(50) << "Book Title" << " | "
            << setw(12) << "Borrow Date" << " | "
            << setw(12) << "Due Date" << " | "
            << setw(12) << "Return Date" << " | "
            << setw(12) << "Status" << " | "
            << setw(10) << "Renewals" << endl;
        cout << string(161, '-') << endl;

        for (int i = 0; i < borrowRecords.size(); i++) {
            cout << left;
            cout << "   " << setw(7) << borrowRecords[i].recordID << " | ";
            cout << "   " << setw(7) << borrowRecords[i].memberID << " | ";
            cout << "   " << setw(7) << borrowRecords[i].bookID << " | ";
            cout << setw(50) << getBookTitle(borrowRecords[i].bookID, books) << " | ";
            cout << setw(12) << borrowRecords[i].borrowDate << " | ";
            cout << setw(12) << borrowRecords[i].dueDate << " | ";
            cout << setw(12) << (borrowRecords[i].returnDate.empty() ? "-" : borrowRecords[i].returnDate) << " | ";
            cout << setw(12) << borrowRecords[i].status << " | ";
            cout << right << setw(5) << borrowRecords[i].renewCount << endl;
        }
        cout << string(161, '-') << endl;
    }
}
void viewOverdueBooks(const vector<Book>& books, const vector<Member>& members, const vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "           OVERDUE BOOKS LIST\n";
    cout << "=============================================\n";

    vector<BorrowRecord> overdueRecords;
    string currentDate = getCurrentDate();

    // Find all overdue records
    for (const BorrowRecord& record : records) {
        if ((record.status == "Borrowed" ||
            record.status == "Renewed" ||
            record.status == "Overdue") &&
            daysBetweenDates(record.dueDate, currentDate) > 0) {

            overdueRecords.push_back(record);
        }
    }

    if (overdueRecords.empty()) {
        cout << "\nNo overdue books found.\n";
        return;
    }

    cout << string(145, '-') << endl;
    cout << left
        << setw(10) << " Record ID" << " | "
        << setw(10) << " Member ID" << " | "
        << setw(25) << "Member Name" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(50) << "Book Title" << " | "
        << setw(12) << "Due Date" << " | "
        << setw(10) << "Days Late" << endl;
    cout << string(145, '-') << endl;

    for (int i = 0; i < overdueRecords.size(); i++) {
        int daysLate = daysBetweenDates(overdueRecords[i].dueDate, currentDate);
        cout << left;
        cout << "   " << setw(7) << overdueRecords[i].recordID << " | ";
        cout << "   " << setw(7) << overdueRecords[i].memberID << " | ";
        cout << setw(25) << getMemberName(overdueRecords[i].memberID, members) << " | ";
        cout << "   " << setw(7) << overdueRecords[i].bookID << " | ";
        cout << setw(50) << getBookTitle(overdueRecords[i].bookID, books) << " | ";
        cout << setw(12) << overdueRecords[i].dueDate << " | ";
        cout << right << setw(5) << daysLate << endl;
    }
    cout << string(145, '-') << endl;

    cout << "Total overdue books: " << overdueRecords.size() << endl;
    cout << "\nNote: Use \"Reporting & Fine Management\" to calculate and collect fines.\n";
}
void checkAvailability(const vector<Book>& books) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "             CHECK AVAILABILITY\n";
    cout << "=============================================\n";

    if (books.empty()) {
        cout << "\nNo books in the system.\n";
        pauseScreen();
        return;
    }

    cout << "Book Availability:\n";
    cout << string(150, '-') << endl;
    cout << left
        << setw(10) << "  Book ID" << " | "
        << setw(50) << "Title" << " | "
        << setw(40) << "Author" << " | "
        << setw(10) << "  Total" << " | "
        << setw(12) << " Available" << " | "
        << setw(10) << "  Status" << endl;
    cout << string(150, '-') << endl;

    for (int i = 0; i < books.size(); i++) {
        string status;

        if (books[i].availableCopies > 0) {
            status = "Available";
        }
        else if (books[i].quantity > 0) {
            status = "All Borrowed";
        }
        else {
            status = "Out of Stock";
        }

        cout << left;
        cout << "   " << setw(7) << books[i].bookID << " | ";
        cout << setw(50) << books[i].bookTitle << " | ";
        cout << setw(40) << books[i].author << " | ";

        cout << right << setw(5) << books[i].quantity;
        cout << setw(6) << "      | ";
        cout << right << setw(6) << books[i].availableCopies;
        cout << setw(5) << "       | ";

        cout << left << setw(10) << status << endl;
    }
    cout << string(150, '-') << endl;

    do {
        // Search for specific book
        cout << "\nDo you want to search for a specific book? (y/n): ";
        string search;
        getline(cin, search);

        if (search == "y" || search == "Y") {
            string searchTerm;
            cout << "Enter book title or ID: ";
            getline(cin, searchTerm);

            string searchLower = searchTerm;
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

            bool found = false;
            cout << "\nSearch Result: \n";
            cout << string(119, '-') << endl;
            cout << left
                << setw(10) << "  Book ID" << " | "
                << setw(50) << "Title" << " | "
                << setw(40) << "Author" << " | "
                << setw(10) << "Available" << endl;
            cout << string(119, '-') << endl;

            for (const Book& book : books) {
                string titleLower = book.bookTitle;
                transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::tolower);

                string idLower = book.bookID;
                transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);

                if (titleLower.find(searchLower) != string::npos ||
                    idLower.find(searchLower) != string::npos) {
                    found = true;
                    cout << left;
                    cout << "   " << setw(7) << book.bookID << " | ";
                    cout << setw(50) << book.bookTitle << " | ";
                    cout << setw(40) << book.author << " | ";
                    cout << right << setw(5) << book.availableCopies << endl;
                }
            }

            if (!found) {
                cout << "\nNo books found matching '" << searchTerm << "'\n";
            }
            cout << string(119, '-') << endl;
        }
        else if (search == "n" || search == "N") {
            break;
        }
        else {
            cout << "Please enter 'y' for yes or 'n' for no!" << endl;

        }
    } while (true);
}
//member
void memberBorrowMenu(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records) {
    int choice;

    do {
        clearScreen();
        cout << "\n=============================================\n";
        cout << "         MEMBER - MY BORROW MANAGEMENT\n";
        cout << "=============================================\n";
        cout << "1. Borrow Book\n";
        cout << "2. Return Book\n";
        cout << "3. Renew Book\n";
        cout << "4. View My Borrow History\n";
        cout << "5. View My Borrowed Books\n";
        cout << "6. Check Book Availability\n";
        cout << "0. Back\n";
        cout << "=============================================\n";

        choice = getValidatedChoice(0, 6);
        cin.ignore();

        switch (choice) {
        case 1:
            memberBorrowBook(memberID, books, records);
            pauseScreen();
            break;
        case 2:
            memberReturnBook(memberID, books, records);
            pauseScreen();
            break;
        case 3:
            memberRenewBook(memberID, books, records);
            pauseScreen();
            break;
        case 4:
            viewMyBorrowHistory(memberID, records, books);
            pauseScreen();
            break;
        case 5:
            viewMyBorrowedBooks(memberID, records, books);
            pauseScreen();
            break;
        case 6:
            checkAvailability(books);
            pauseScreen();
            break;
        case 0:
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
            pauseScreen();
        }
    } while (choice != 0);
}
void memberBorrowBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records) {
    clearScreen();
    loadMembers();
    cout << "\n=============================================\n";
    cout << "            MEMBER - BORROW BOOK\n";
    cout << "=============================================\n";

    // Check if member is active
    if (!isMemberActive(memberID, members)) {
        cout << "\nYour account is not active. Cannot borrow books.\n";
        cout << "Please contact the administrator.\n";
        return;
    }

    if (books.empty()) {
        cout << "\nNo books available in the system.\n";
        return;
    }

    loadBorrowRecords(records);

    // Check for overdue books
    bool hasOverdue = false;
    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID && record.status == "Overdue") {
            hasOverdue = true;
            break;
        }
    }

    if (hasOverdue) {
        cout << "\nYou have overdue books. Please return them before borrowing new books.\n";
        return;
    }

    // Display available books
    cout << "Available Books: \n";
    cout << string(120, '-') << endl;
    cout << left << setw(10) << "  Book ID" << " | "
        << setw(50) << "Title" << " | "
        << setw(40) << "Author" << " | "
        << setw(10) << "Available" << endl;
    cout << string(120, '-') << endl;

    bool hasAvailableBooks = false;
    for (const Book& book : books) {
        if (book.availableCopies > 0) {
            hasAvailableBooks = true;
            cout << left << "   " << setw(7) << book.bookID << " | "
                << setw(50) << book.bookTitle << " | "
                << setw(40) << book.author << " | "
                << right << setw(5) << book.availableCopies << endl;
        }
    }

    if (!hasAvailableBooks) {
        cout << "\nNo books are currently available.\n";
        return;
    }
    cout << string(120, '-') << endl;

    // Get book ID
    string bookID;
    bool validBook = false;

    do {
        cout << "\nEnter Book ID to borrow (0 to cancel): ";
        getline(cin, bookID);
        bookID = toUpperCase(bookID);

        if (bookID == "0") {
            cout << "\nBorrow operation cancelled.\n";
            return;
        }

        // Check if Book ID exists
        bool bookFound = false;

        for (const Book& book : books) {
            if (book.bookID == bookID) {
                bookFound = true;
                break;
            }
        }

        if (!bookFound) {
            cout << "Book not found. Please enter a valid Book ID.\n";
            continue;
        }

        // Check book availability
        if (!isBookAvailable(bookID, books, 1)) {
            cout << "\nThis book is currently not available for borrowing.\n";
            cout << "Please enter another Book ID.\n";
            continue;
        }

        // Check if member already borrowed this book
        bool alreadyBorrowed = false;

        for (const BorrowRecord& record : records) {
            if (record.memberID == memberID &&
                record.bookID == bookID &&
                record.status != "Returned") {

                alreadyBorrowed = true;
                break;
            }
        }

        if (alreadyBorrowed) {
            cout << "\nMember has already borrowed this book. Cannot borrow again.\n";
            cout << "Please enter another Book ID.\n";
            continue;
        }

        // All checks passed
        validBook = true;

    } while (!validBook);

    // Create borrow record
    BorrowRecord newRecord;
    newRecord.recordID = generateRecordID(records);
    newRecord.memberID = memberID;
    newRecord.bookID = bookID;
    newRecord.borrowDate = getCurrentDate();
    newRecord.dueDate = calculateDueDate(LOAN_PERIOD_DAYS);
    newRecord.returnDate = "";
    newRecord.status = "Borrowed";
    newRecord.renewCount = 0;
    newRecord.fineAmount = 0.0;
    newRecord.paymentStatus = "N/A";
    newRecord.amountPaid = 0.0;

    // Display confirmation
    cout << "\n---------------------------------------------\n";
    cout << "           Borrow Confirmation\n";
    cout << "---------------------------------------------\n";
    cout << "Book Title  : " << getBookTitle(newRecord.bookID, books) << endl;
    cout << "Borrow Date : " << newRecord.borrowDate << endl;
    cout << "Due Date    : " << newRecord.dueDate << endl;
    cout << "---------------------------------------------\n";

    string confirm;

    do {
        cout << "\nDo you want to borrow this book? (y/n): ";
        getline(cin, confirm);

        if (confirm == "y" || confirm == "Y") {
            records.push_back(newRecord);
            saveBorrowRecords(records);

            for (Book& book : books) {
                if (book.bookID == bookID) {
                    book.availableCopies--;
                    break;
                }
            }

            saveBooksToFile(books);

            cout << "\nBook Borrowed Successfully!\n";
            cout << "---------------------------------------------\n";
            cout << "Due Date    : " << newRecord.dueDate << endl;
            cout << "Please return on time to avoid overdue status.\n";
            cout << "---------------------------------------------\n";

            break;
        }
        else if (confirm == "n" || confirm == "N") {
            cout << "\nBorrow operation cancelled." << endl;
            break;
        }
        else {
            cout << "Please enter 'y' for yes or 'n' for no!" << endl;
        }
    } while (true);
}
void memberReturnBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "            MEMBER - RETURN BOOK\n";
    cout << "=============================================\n";

    loadBorrowRecords(records);

    // Find the member's active borrow records
    vector<BorrowRecord> myBooks;
    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID &&
            (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed")) {
            myBooks.push_back(record);
        }
    }

    if (myBooks.empty()) {
        cout << "\nYou have no books to return.\n";
        return;
    }

    cout << "Your Borrowed Books:\n";
    displayBorrowRecordTable(myBooks, books);

    string recordID;
    bool found = false;

    do {
        cout << "\nEnter Record ID to return (0 to cancel): ";
        getline(cin, recordID);
        recordID = toUpperCase(recordID);

        if (recordID == "0") {
            cout << "\nReturn operation cancelled.\n";
            return;
        }
        if (recordID.empty() || isWhitespaceOnly(recordID)) {
            cout << "Input cannot be empty. Please try again.\n";
            continue;
        }

        for (const BorrowRecord& record : records) {
            if (record.recordID == recordID && record.memberID == memberID &&
                (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed")) {
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Record not found or does not belong to you.\n";
        }
    } while (!found);

    // Process return
    for (BorrowRecord& record : records) {
        if (record.recordID == recordID) {
            string returnDate = getCurrentDate();

            // Preview the fine without mutating the real record yet.
            BorrowRecord preview = record;
            applyFineRules(preview, returnDate);
            int daysLate = daysBetweenDates(record.dueDate, returnDate);

            cout << "\n---------------------------------------------\n";
            cout << "               Return Details\n";
            cout << "---------------------------------------------\n";
            cout << "Book Title  : " << getBookTitle(record.bookID, books) << endl;
            cout << "Borrow Date : " << record.borrowDate << endl;
            cout << "Due Date    : " << record.dueDate << endl;
            cout << "Return Date : " << returnDate << endl;

            if (daysLate > 0) {
                cout << "\n*** BOOK IS OVERDUE (" << daysLate << " day(s)) ***\n";
                if (record.paymentStatus == "Paid") {
                    cout << "Fine Amount : RM " << fixed << setprecision(2)
                        << record.fineAmount << " (already paid)\n";
                }
                else {
                    cout << "Fine Amount : RM " << fixed << setprecision(2)
                        << preview.fineAmount << endl;
                    cout << "Please settle your fine via Reporting & Fine Management.\n";
                }
            }
            else {
                cout << "\nBook returned on time. Thank you!\n";
            }
            cout << "---------------------------------------------\n";

            string confirm;

            do {
                cout << "\nDo you want to return this book? (y/n): ";
                getline(cin, confirm);

                if (confirm == "y" || confirm == "Y") {
                    record.returnDate = returnDate;
                    record.status = "Returned";

                    applyFineRules(record, returnDate);

                    saveBorrowRecords(records);

                    for (Book& book : books) {
                        if (book.bookID == record.bookID) {
                            book.availableCopies++;
                            break;
                        }
                    }

                    saveBooksToFile(books);

                    cout << "\nBook Returned Successfully!\n";
                    cout << "---------------------------------------------\n";
                    cout << "Thank you for returning the book.\n";

                    if (record.paymentStatus == "Unpaid" && record.fineAmount > FINE_EPSILON) {
                        cout << "Fine Due    : RM " << fixed << setprecision(2)
                            << record.fineAmount << endl;
                    }

                    cout << "---------------------------------------------\n";

                    break;
                }
                else if (confirm == "n" || confirm == "N") {
                    cout << "\nReturn operation cancelled.\n";
                    break;
                }
                else {
                    cout << "Please enter 'y' for yes or 'n' for no!" << endl;
                }

            } while (true);
            return;
        }
    }
}
void memberRenewBook(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "             MEMBER - RENEW BOOK\n";
    cout << "=============================================\n";

    loadBorrowRecords(records);

    // Find member's renewable books
    vector<BorrowRecord> myRenewable;
    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID &&
            (record.status == "Borrowed" || record.status == "Renewed") &&
            record.renewCount < MAX_RENEW_COUNT) {
            myRenewable.push_back(record);
        }
    }

    if (myRenewable.empty()) {
        cout << "\nNo books available for renewal.\n";
        cout << "Books must be currently borrowed and not exceed maximum renewals.\n";
        return;
    }

    cout << "Books Eligible For Renewal: \n";
    displayBorrowRecordTable(myRenewable, books);

    string recordID;
    bool found = false;

    do {
        cout << "\nEnter Record ID to renew (0 to cancel): ";
        getline(cin, recordID);
        recordID = toUpperCase(recordID);

        if (recordID == "0") {
            cout << "\nRenewal cancelled.\n";
            return;
        }
        if (recordID.empty() || isWhitespaceOnly(recordID)) {
            cout << "Input cannot be empty. Please try again.\n";
            continue;
        }

        for (const BorrowRecord& record : records) {
            if (record.recordID == recordID && record.memberID == memberID &&
                (record.status == "Borrowed" || record.status == "Renewed")) {
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Record not found or not eligible for renewal.\n";
        }
    } while (!found);

    // Process renewal
    for (BorrowRecord& record : records) {
        if (record.recordID == recordID) {
            string newDueDate = calculateDueDate(LOAN_PERIOD_DAYS);

            cout << "\n---------------------------------------------\n";
            cout << "             Renew Confirmation\n";
            cout << "---------------------------------------------\n";
            cout << "Book Title  : " << getBookTitle(record.bookID, books) << endl;
            cout << "Current Due : " << record.dueDate << endl;
            cout << "New Due Date: " << newDueDate << endl;
            cout << "Renewal #   : " << (record.renewCount + 1) << " of " << MAX_RENEW_COUNT << endl;
            cout << "---------------------------------------------\n";

            string confirm;

            do {
                cout << "\nDo you want to renew this book? (y/n): ";
                getline(cin, confirm);

                if (confirm == "y" || confirm == "Y") {
                    record.dueDate = newDueDate;
                    record.renewCount++;
                    record.status = "Renewed";

                    saveBorrowRecords(records);

                    cout << "\nBook Renewed Successfully!\n";
                    cout << "---------------------------------------------\n";
                    cout << "New Due Date: " << newDueDate << endl;
                    cout << "Renewals Used : " << record.renewCount << " of " << MAX_RENEW_COUNT << endl;
                    cout << "Renewals Remaining: " << (MAX_RENEW_COUNT - record.renewCount) << endl;
                    cout << "---------------------------------------------\n";

                    break;
                }
                else if (confirm == "n" || confirm == "N") {
                    cout << "\nRenewal operation cancelled." << endl;
                    break;
                }
                else {
                    cout << "Please enter 'y' for yes or 'n' for no!" << endl;
                }

            } while (true);
            return;
        }
    }
}
void viewMyBorrowHistory(const string& memberID, const vector<BorrowRecord>& records, const vector<Book>& books) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "              MY BORROW HISTORY\n";
    cout << "=============================================\n";

    vector<BorrowRecord> myRecords;

    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID) {
            myRecords.push_back(record);
        }
    }

    if (myRecords.empty()) {
        cout << "\nYou have no borrow history.\n";
        return;
    }

    cout << "Total borrows: " << myRecords.size() << "\n";

    cout << string(150, '-') << endl;
    cout << left
        << setw(10) << " Record ID" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(50) << "Book Title" << " | "
        << setw(12) << "Borrow Date" << " | "
        << setw(12) << "Due Date" << " | "
        << setw(12) << "Return Date" << " | "
        << setw(12) << "Status" << " | "
        << setw(10) << "Renewals" << endl;
    cout << string(150, '-') << endl;

    for (int i = 0; i < myRecords.size(); i++) {
        cout << left;

        cout << "   " << setw(7) << myRecords[i].recordID << " | ";
        cout << "   " << setw(7) << myRecords[i].bookID << " | ";
        cout << setw(50) << getBookTitle(myRecords[i].bookID, books) << " | ";
        cout << setw(12) << myRecords[i].borrowDate << " | ";
        cout << setw(12) << myRecords[i].dueDate << " | ";
        cout << setw(12) << (myRecords[i].returnDate.empty() ? "-" : myRecords[i].returnDate) << " | ";
        cout << setw(12) << myRecords[i].status << " | ";

        cout << right << setw(5) << myRecords[i].renewCount << endl;
    }
    cout << string(150, '-') << endl;
}
void viewMyBorrowedBooks(const string& memberID, const vector<BorrowRecord>& records, const vector<Book>& books) {
    clearScreen();
    cout << "\n=============================================\n";
    cout << "              MY BORROWED BOOKS\n";
    cout << "=============================================\n";

    vector<BorrowRecord> myBooks;

    for (const BorrowRecord& record : records) {
        if (record.memberID == memberID &&
            (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed")) {
            myBooks.push_back(record);
        }
    }

    if (myBooks.empty()) {
        cout << "\nYou currently have no borrowed books.\n";
        return;
    }

    cout << "Total borrowed: " << myBooks.size() << "\n";

    cout << string(135, '-') << endl;
    cout << left
        << setw(10) << " Record ID" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(50) << "Book Title" << " | "
        << setw(12) << "Borrow Date" << " | "
        << setw(12) << "Due Date" << " | "
        << setw(12) << "Status" << " | "
        << setw(10) << "Renewals" << endl;
    cout << string(135, '-') << endl;

    for (int i = 0; i < myBooks.size(); i++) {
        cout << left;

        cout << "   " << setw(7) << myBooks[i].recordID << " | ";
        cout << "   " << setw(7) << myBooks[i].bookID << " | ";
        cout << setw(50) << getBookTitle(myBooks[i].bookID, books) << " | ";
        cout << setw(12) << myBooks[i].borrowDate << " | ";
        cout << setw(12) << myBooks[i].dueDate << " | ";
        cout << setw(12) << myBooks[i].status << " | ";

        cout << right << setw(5) << myBooks[i].renewCount << endl;
    }
    cout << string(135, '-') << endl;
}
void displayBorrowRecordTable(const vector<BorrowRecord>& records, const vector<Book>& books) {
    if (records.empty()) {
        cout << "No records to display.\n";
        return;
    }

    cout << string(130, '-') << endl;
    cout << left
        << setw(10) << " Record ID" << " | "
        << setw(10) << " Member ID" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(50) << "Book Title" << " | "
        << setw(12) << "Due Date" << " | "
        << setw(10) << "Status" << " | "
        << setw(10) << "Renewals" << endl;
    cout << string(130, '-') << endl;

    for (int i = 0; i < records.size(); i++) {
        cout << left;
        cout << "   " << setw(7) << records[i].recordID << " | ";
        cout << "   " << setw(7) << records[i].memberID << " | ";
        cout << "   " << setw(7) << records[i].bookID << " | ";
        cout << setw(50) << getBookTitle(records[i].bookID, books) << " | ";
        cout << setw(12) << records[i].dueDate << " | ";
        cout << setw(10) << records[i].status << " | ";
        cout << right << setw(5) << records[i].renewCount << endl;
    }
    cout << string(130, '-') << endl;
}
void loadBorrowRecords(vector<BorrowRecord>& records) {
    records.clear();
    ifstream file(BORROW_FILE);

    if (!file.is_open()) {
        // Create empty file if not exists
        ofstream newFile(BORROW_FILE);
        newFile << "RecordID|MemberID|BookID|BorrowDate|DueDate|ReturnDate|Status|RenewCount|FineAmount|PaymentStatus|AmountPaid" << endl;
        newFile.close();
        return;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;

        BorrowRecord record;
        stringstream ss(line);

        getline(ss, record.recordID, '|');
        getline(ss, record.memberID, '|');
        getline(ss, record.bookID, '|');
        getline(ss, record.borrowDate, '|');
        getline(ss, record.dueDate, '|');
        getline(ss, record.returnDate, '|');
        getline(ss, record.status, '|');

        string renewStr, fineStr;
        getline(ss, renewStr, '|');
        record.renewCount = safeStoi(renewStr);

        if (getline(ss, fineStr, '|')) {
            try {
                record.fineAmount = stod(fineStr);
            }
            catch (...) {
                record.fineAmount = 0.0;
            }

            string paymentStr, paidStr;
            if (getline(ss, paymentStr, '|')) {
                record.paymentStatus = paymentStr;

                if (getline(ss, paidStr)) {
                    try {
                        record.amountPaid = stod(paidStr);
                    }
                    catch (...) {
                        record.amountPaid = 0.0;
                    }
                }
                else {
                    // Old file format without the AmountPaid column yet
                    record.amountPaid = 0.0;
                }
            }
            else {
                record.paymentStatus = "N/A";
                record.amountPaid = 0.0;
            }
        }
        else {
            // Older-format file without fine columns at all
            record.fineAmount = 0.0;
            record.paymentStatus = "N/A";
            record.amountPaid = 0.0;
        }

        records.push_back(record);
    }

    file.close();
}
void saveBorrowRecords(const vector<BorrowRecord>& records) {
    ofstream file(BORROW_FILE);

    if (!file.is_open()) {
        cout << "Error: Unable to save borrow records.\n";
        pauseScreen();
        return;
    }

    file << "RecordID|MemberID|BookID|BorrowDate|DueDate|ReturnDate|Status|RenewCount|FineAmount|PaymentStatus|AmountPaid" << endl;

    for (const BorrowRecord& record : records) {
        file << record.recordID << "|"
            << record.memberID << "|"
            << record.bookID << "|"
            << record.borrowDate << "|"
            << record.dueDate << "|"
            << record.returnDate << "|"
            << record.status << "|"
            << record.renewCount << "|"
            << fixed << setprecision(2) << record.fineAmount << "|"
            << record.paymentStatus << "|"
            << fixed << setprecision(2) << record.amountPaid << endl;
    }

    file.close();
}

//Helper Functions
string generateRecordID(const vector<BorrowRecord>& records) {
    int highestID = 0;

    for (const BorrowRecord& record : records) {
        if (record.recordID.length() > 1 && record.recordID[0] == 'R') {
            try {
                int currentID = stoi(record.recordID.substr(1));
                if (currentID > highestID) {
                    highestID = currentID;
                }
            }
            catch (...) {
                // Ignore invalid IDs
            }
        }
    }

    highestID++;

    string recordID = "R";
    if (highestID < 10) {
        recordID += "00" + to_string(highestID);
    }
    else if (highestID < 100) {
        recordID += "0" + to_string(highestID);
    }
    else {
        recordID += to_string(highestID);
    }
    return recordID;
}
string getCurrentDate() {
    time_t now = time(nullptr);
    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &localTime);
    return string(buffer);
}
string calculateDueDate(int daysToAdd) {
    time_t now = time(nullptr);
    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    localTime.tm_mday += daysToAdd;
    mktime(&localTime);

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &localTime);
    return string(buffer);
}
int daysBetweenDates(const string& date1, const string& date2) {
    int year1 = stoi(date1.substr(0, 4));
    int month1 = stoi(date1.substr(5, 2));
    int day1 = stoi(date1.substr(8, 2));

    int year2 = stoi(date2.substr(0, 4));
    int month2 = stoi(date2.substr(5, 2));
    int day2 = stoi(date2.substr(8, 2));

    tm tm1 = {};
    tm1.tm_year = year1 - 1900;
    tm1.tm_mon = month1 - 1;
    tm1.tm_mday = day1;

    tm tm2 = {};
    tm2.tm_year = year2 - 1900;
    tm2.tm_mon = month2 - 1;
    tm2.tm_mday = day2;

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    double diff = difftime(time2, time1) / (60 * 60 * 24);
    return static_cast<int>(diff);
}
bool isMemberActive(const string& memberID, const vector<Member>& members) {
    for (const Member& member : members) {
        if (member.memberID == memberID) {
            return (member.membershipStatus == "Active");
        }
    }
    return false;
}
bool isBookAvailable(const string& bookID, const vector<Book>& books, int quantityNeeded) {
    for (const Book& book : books) {
        if (book.bookID == bookID) {
            return (book.availableCopies >= quantityNeeded);
        }
    }
    return false;
}
string getMemberName(const string& memberID, const vector<Member>& members) {
    for (const Member& member : members) {
        if (member.memberID == memberID) {
            return member.memberName;
        }
    }
    return "Unknown Member";
}
string getBookTitle(const string& bookID, const vector<Book>& books) {
    for (const Book& book : books) {
        if (book.bookID == bookID) {
            return book.bookTitle;
        }
    }
    return "Unknown Book";
}
void updateOverdueStatus() {
    vector<BorrowRecord> records;
    loadBorrowRecords(records);

    string currentDate = getCurrentDate();
    bool updated = false;

    for (BorrowRecord& record : records) {
        if (record.status == "Borrowed" || record.status == "Renewed") {
            if (daysBetweenDates(record.dueDate, currentDate) > 0) {
                // Status AND fine are updated together so a record
                // is never left "Overdue" with fineAmount still
                // sitting at 0.00 / N/A.
                if (applyFineRules(record, currentDate)) {
                    updated = true;
                }
                if (record.status != "Overdue" && record.paymentStatus != "Paid") {
                    record.status = "Overdue";
                    updated = true;
                }
            }
        }
    }

    if (updated) {
        saveBorrowRecords(records);
    }
}
void initBorrowReturn() {
    vector<BorrowRecord> temp;
    loadBorrowRecords(temp);
    updateOverdueStatus();
}
int safeStoi(const string& str) {
    try {
        size_t pos;
        int result = stoi(str, &pos);
        if (pos != str.length()) {
            return -1;
        }
        return result;
    }
    catch (...) {
        return -1;
    }
}
int getValidatedChoice(int minVal, int maxVal) {
    int choice;
    bool valid = false;
    string input;

    while (!valid) {
        cout << "Enter your choice (" << minVal << "-" << maxVal << "): ";
        cin >> input;

        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isNumber = false;
                break;
            }
        }

        if (!isNumber) {
            cout << "Please enter a number between " << minVal << " and " << maxVal << "\n" << endl;
            continue;
        }

        int tempChoice = safeStoi(input);
        if (tempChoice == -1) {
            cout << "Please enter a valid number!" << endl;
            continue;
        }

        choice = tempChoice;

        if (choice >= minVal && choice <= maxVal) {
            valid = true;
        }
        else {
            cout << "Please enter a number between " << minVal << " and " << maxVal << "\n" << endl;
        }
    }
    return choice;
}
void clearScreen() {
    system("cls");
}
void pauseScreen() {
    cout << "\nPlease press \"Enter\" key to Continue.....";
    cin.get();
}
string toUpperCase(string text) {
    for (char& c : text)
    {
        c = toupper(c);
    }
    return text;
}


//MEMBER MANAGEMENT//
// admin menu function
void adminMenu()
{
    vector<BorrowRecord> records;
    int choice;
    while (true)
    {
        clearScreen();

        cout << "\n=============================================\n";
        cout << "                 ADMIN MENU\n";
        cout << "=============================================\n";
        cout << "1. Member Management\n";
        cout << "2. Book Management\n";
        cout << "3. Borrowing & Returning Management\n";
        cout << "4. Reporting & Fine Management\n";
        cout << "0. Logout\n";
        cout << "=============================================\n";

        choice = getValidatedChoice(0, 4);
        cin.ignore();

        switch (choice) {
        case 1:
            memberManagement();
            break;
        case 2:
            bookMenu();
            break;
        case 3:
            adminBorrowMenu(books, members, records);
            break;
        case 4:
            adminReportingAndFineMenu();
            break;
        case 0:
            cout << "\nLogging out...\n";
            pauseScreen();
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
            pauseScreen();
        }
    }
}
// member management menu 
void memberManagement()
{
    int choice;
    while (true)
    {
        clearScreen();
        loadMembers();

        cout << "\n=============================================\n";
        cout << "              MEMBER MANAGEMENT\n";
        cout << "=============================================\n";
        cout << "1. Add Member\n";
        cout << "2. Display All Members\n";
        cout << "3. Search Member\n";
        cout << "4. Update Member\n";
        cout << "5. Delete Member\n";
        cout << "0. Back\n";
        cout << "=============================================\n";

        choice = getValidatedChoice(0, 5);
        cin.ignore();

        switch (choice) {
        case 0:
            return;
        case 1:
            addMember();
            pauseScreen();
            break;

        case 2:
            displayMembers();
            pauseScreen();
            break;

        case 3:
            searchMember();
            pauseScreen();
            break;

        case 4:
            updateMember();
            pauseScreen();
            break;

        case 5:
            deleteMember();
            pauseScreen();
            break;

        default:
            cout << "Invalid choice. Please try again." << endl;
            pauseScreen();
        }
    }
}
// add member function
void addMember()
{
    clearScreen();
    cout << "\n=============================================\n";
    cout << "              ADD NEW MEMBER\n";
    cout << "=============================================\n";

    Member newMember;

    // Generate Member ID
    newMember.memberID = generateMemberID();

    cout << "Member ID: " << newMember.memberID << endl;

    // Member Name
    while (true)
    {
        cout << "Enter Member Name: ";
        getline(cin, newMember.memberName);

        string errorMessage;
        if (validName(newMember.memberName, errorMessage)) {
            break;
        }
        else {
            cout << errorMessage << endl;
        }
    }

    // Phone Number
    while (true)
    {
        cout << "Enter Phone Number: ";
        getline(cin, newMember.phoneNum);

        string errorMessage;
        if (validPhone(newMember.phoneNum, errorMessage)) {
            break;
        }
        else {
            cout << errorMessage << endl;
        }
    }

    // Email
    while (true)
    {
        cout << "Enter Email: ";
        getline(cin, newMember.email);

        string errorMessage;
        if (validEmail(newMember.email, errorMessage)) {
            break;
        }
        else {
            cout << errorMessage << endl;
        }
    }

    // Membership Type
    int typeChoice;

    while (true)
    {
        cout << "\nSelect Membership Type:\n";
        cout << "1. Student\n";
        cout << "2. Staff\n";
        cout << "3. Public\n";

        typeChoice = getValidatedChoice(1, 3);

        if (typeChoice == 1) {
            newMember.membershipType = "Student";
            break;
        }
        else if (typeChoice == 2) {
            newMember.membershipType = "Staff";
            break;
        }
        else if (typeChoice == 3) {
            newMember.membershipType = "Public";
            break;
        }
        else {
            cout << "Invalid choice. Please enter 1-3.\n";
        }
    }

    // Default Status
    newMember.membershipStatus = "Active";

    // Add to vector
    members.push_back(newMember);

    // Save to file
    saveMembers();

    cout << "\nMember added successfully!\n";
    cout << "Data saved to members.txt\n";
    cout << "---------------------------------------------\n";
    cout << "Member ID       : " << newMember.memberID << endl;
    cout << "Member Name     : " << newMember.memberName << endl;
    cout << "Phone Number    : " << newMember.phoneNum << endl;
    cout << "Email           : " << newMember.email << endl;
    cout << "Membership Type : " << newMember.membershipType << endl;
    cout << "Status          : " << newMember.membershipStatus << endl;
    cout << "---------------------------------------------\n";

    cin.get();
}

// display all members function
void displayMembers()
{
    clearScreen();
    cout << "\n=============================================\n";
    cout << "                ALL MEMBERS\n";
    cout << "=============================================\n";
    cout << "----------------------------------------------------------------------------------------------------------------\n";
    if (members.empty())
    {
        cout << "\nNo members found.\n";
        return;
    }

    cout << left
        << setw(10) << "ID" << " | " << setw(20) << "Name" << " | "
        << setw(15) << "Phone" << " | " << setw(30) << "Email" << " | "
        << setw(15) << "Type" << " | " << setw(12) << "Status" << endl;

    cout << "----------------------------------------------------------------------------------------------------------------\n";

    for (const Member& member : members)
    {
        cout << left
            << setw(10) << member.memberID << " | " << setw(20) << member.memberName << " | "
            << setw(15) << member.phoneNum << " | " << setw(30) << member.email << " | "
            << setw(15) << member.membershipType << " | " << setw(12) << member.membershipStatus << endl;
    }

    cout << "----------------------------------------------------------------------------------------------------------------\n";
    cout << "Total Members: " << members.size() << endl;
}

// search member function
void searchMember()
{
    clearScreen();
    cout << "\n=============================================\n";
    cout << "              SEARCH MEMBER\n";
    cout << "=============================================\n";

    string searchID;

    while (true)
    {
        cout << "Enter Member ID: ";
        cin >> searchID;

        searchID = toUpperCase(searchID);
        cin.ignore();
        bool found = false;

        for (const Member& member : members)
        {
            if (member.memberID == searchID)
            {
                cout << "\nMember found!\n";
                cout << "---------------------------------------------\n";
                cout << "Member ID       : " << member.memberID << endl;
                cout << "Member Name     : " << member.memberName << endl;
                cout << "Phone Number    : " << member.phoneNum << endl;
                cout << "Email           : " << member.email << endl;
                cout << "Membership Type : " << member.membershipType << endl;
                cout << "Status          : " << member.membershipStatus << endl;
                cout << "---------------------------------------------\n";

                found = true;
                break;
            }
        }

        if (found) {
            break;
        }
        cout << "\nMember with ID " << searchID << " not found.\n";
        cout << "Please enter the Member ID again.\n\n";
    }

}
// update member function
void updateMember()
{
    clearScreen();
    cout << "\n=============================================\n";
    cout << "              UPDATE MEMBER\n";
    cout << "=============================================\n";

    string searchID;

    while (true)
    {
        cout << "Enter Member ID: ";
        cin >> searchID;

        searchID = toUpperCase(searchID);
        cin.ignore();
        bool found = false;

        for (Member& member : members)
        {
            if (member.memberID == searchID) {
                found = true;
                int choice;

                while (true)
                {
                    cout << "\n---------------------------------------------\n";
                    cout << "Member ID       : " << member.memberID << endl;
                    cout << "Member Name     : " << member.memberName << endl;
                    cout << "Phone Number    : " << member.phoneNum << endl;
                    cout << "Email           : " << member.email << endl;
                    cout << "Membership Type : " << member.membershipType << endl;
                    cout << "---------------------------------------------\n";

                    cout << "\nWhat do you want to update?\n";
                    cout << "1. Member Name\n";
                    cout << "2. Phone Number\n";
                    cout << "3. Email\n";
                    cout << "4. Membership Type\n";
                    cout << "5. Membership Status\n";
                    cout << "0. Back\n";
                    choice = getValidatedChoice(0, 5);

                    // Update Name
                    if (choice == 1)
                    {
                        cin.ignore();

                        string newName;
                        string errorMessage;

                        cout << "Enter New Name: ";
                        getline(cin, newName);

                        if (validName(newName, errorMessage))
                        {
                            member.memberName = newName;
                            saveMembers();
                            cout << "Name updated successfully!\n";
                        }
                        else
                        {
                            cout << errorMessage << endl;
                        }
                    }

                    // Update Phone
                    else if (choice == 2) {
                        cin.ignore();
                        string newPhone;
                        cout << "Enter New Phone Number: ";
                        getline(cin, newPhone);

                        string errorMessage;
                        if (validPhone(newPhone, errorMessage)) {
                            member.phoneNum = newPhone;
                            saveMembers();
                            cout << "Phone number updated successfully!\n";
                        }
                        else {
                            cout << errorMessage << endl;
                        }
                    }

                    // Update Email
                    else if (choice == 3)
                    {
                        cin.ignore();

                        string newEmail;
                        string errorMessage;

                        cout << "Enter New Email: ";
                        getline(cin, newEmail);

                        if (validEmail(newEmail, errorMessage))
                        {
                            member.email = newEmail;
                            saveMembers();
                            cout << "Email updated successfully!\n";
                        }
                        else
                        {
                            cout << errorMessage << endl;
                        }
                    }

                    // Update Membership Type
                    else if (choice == 4)
                    {
                        int typeChoice;

                        cout << "\n1. Student\n";
                        cout << "2. Staff\n";
                        cout << "3. Public\n";
                        typeChoice = getValidatedChoice(1, 3);

                        if (typeChoice == 1)
                        {
                            member.membershipType = "Student";
                        }
                        else if (typeChoice == 2)
                        {
                            member.membershipType = "Staff";
                        }
                        else if (typeChoice == 3)
                        {
                            member.membershipType = "Public";
                        }
                        else
                        {
                            cout << "Invalid choice.\n";
                            continue;
                        }

                        saveMembers();
                        cout << "Membership type updated successfully!\n";
                    }

                    // Update Membership Status
                    else if (choice == 5)
                    {
                        int statusChoice;

                        cout << "\nSelect Membership Status:\n";
                        cout << "1. Active\n";
                        cout << "2. Inactive\n";
                        cout << "3. Suspended\n";
                        statusChoice = getValidatedChoice(1, 3);

                        if (statusChoice == 1) {
                            member.membershipStatus = "Active";
                        }
                        else if (statusChoice == 2) {
                            member.membershipStatus = "Inactive";
                        }
                        else if (statusChoice == 3) {
                            member.membershipStatus = "Suspended";
                        }
                        else {
                            cout << "Invalid choice. Please enter 1-3.\n";
                            continue;
                        }

                        saveMembers();
                        cout << "Membership status updated successfully!\n";
                    }

                    // Back
                    else if (choice == 0)
                    {
                        return;
                    }
                }
            }
        }
        if (found)
        {
            break;
        }

        cout << "\nMember with ID " << searchID << " not found.\n";
        cout << "Please enter the Member ID again.\n\n";
    }
}

// delete member function
void deleteMember()
{
    while (true)
    {
        clearScreen();
        cout << "\n=============================================\n";
        cout << "              DELETE MEMBER\n";
        cout << "=============================================\n";

        string searchID;

        cout << "Enter Member ID (or 0 to cancel): ";
        cin >> searchID;
        searchID = toUpperCase(searchID);
        cin.ignore();

        if (searchID == "0")
        {
            cout << "\nDeletion cancelled.\n";
            return;
        }

        bool found = false;

        for (int i = 0; i < members.size(); i++)
        {
            if (members[i].memberID == searchID)
            {
                found = true;

                cout << "\nMember found!\n";
                cout << "---------------------------------------------\n";
                cout << "Member ID       : " << members[i].memberID << endl;
                cout << "Member Name     : " << members[i].memberName << endl;
                cout << "Membership Type : " << members[i].membershipType << endl;
                cout << "Status          : " << members[i].membershipStatus << endl;
                cout << "---------------------------------------------\n";

                string confirmation;

                do {
                    cout << "\nDelete this member? (y/n): ";
                    getline(cin, confirmation);

                    if (confirmation == "y" || confirmation == "Y") {
                        members.erase(members.begin() + i);
                        saveMembers();
                        cout << "\nMember deleted successfully!\n";
                        return;
                    }
                    else if (confirmation == "n" || confirmation == "N") {
                        cout << "\nDelete cancelled.\n";
                        cout << "\nPlease press \"Enter\" key to Continue.....";
                        cin.get();
                        break;
                    }
                    else {
                        cout << "\nPlease enter 'y' for yes or 'n' for no!" << endl;
                    }
                } while (true);

                break;
            }
        }

        // If ID was not found
        if (!found) {
            cout << "\nMember with ID " << searchID << " not found.\n";
            cout << "Please enter a valid Member ID.\n";
            cin.get();
        }
    }
}


// member menu function
void memberMenu(const string& memberID, vector<Book>& books, vector<BorrowRecord>& records) {
    int choice;

    while (true)
    {
        clearScreen();

        cout << "\n=============================================\n";
        cout << "                MEMBER MENU\n";
        cout << "=============================================\n";
        cout << "1. My Profile\n";
        cout << "2. Borrowing & Returning Management\n";
        cout << "3. Reporting & Fine Management\n";
        cout << "0. Logout\n";
        cout << "=============================================\n";

        choice = getValidatedChoice(0, 3);
        cin.ignore();

        switch (choice) {
        case 1:
            loadMembers();
            cout << "\n=============================================\n";
            cout << "                 MY PROFILE\n";
            cout << "=============================================\n";
            cout << "1. View Profile\n";
            cout << "2. Edit Profile\n";
            cout << "0. Back\n";
            cout << "=============================================\n";

            int profileChoice;
            profileChoice = getValidatedChoice(0, 2);

            if (profileChoice == 1) {
                viewProfile(memberID);
                pauseScreen();
            }
            else if (profileChoice == 2) {
                editProfile(memberID);
            }
            else if (profileChoice != 0) {
                cout << "\nInvalid choice.\n";
                pauseScreen();
            }
            break;

        case 2:
            memberBorrowMenu(memberID, books, records);
            pauseScreen();
            break;

        case 3:
            memberReportingAndFineMenu(memberID);
            break;

        case 0:
            cout << "\nLogging out...\n";
            pauseScreen();
            return;

        default:
            cout << "\nInvalid choice. Please enter 1-5.\n";
            pauseScreen();
        }
    }
}
// member profile functions
void viewProfile(const string& memberID)
{
    clearScreen();
    cout << "\n=============================================\n";
    cout << "                MY PROFILE\n";
    cout << "=============================================\n";

    for (const Member& member : members)
    {
        if (member.memberID == memberID)
        {
            cout << "Member ID       : " << member.memberID << endl;
            cout << "Member Name     : " << member.memberName << endl;
            cout << "Phone Number    : " << member.phoneNum << endl;
            cout << "Email           : " << member.email << endl;
            cout << "Membership Type : " << member.membershipType << endl;
            cout << "Status          : " << member.membershipStatus << endl;
            cin.get();
            return;
        }
    }
    cout << "Profile not found.\n";
    cin.get();
}
void editProfile(const string& memberID)
{
    cout << "\n=============================================\n";
    cout << "              EDIT MY PROFILE\n";
    cout << "=============================================";

    for (Member& member : members)
    {
        if (member.memberID == memberID)
        {
            int choice;

            while (true)
            {
                cout << "\n1. Edit Name\n";
                cout << "2. Edit Phone Number\n";
                cout << "3. Edit Email\n";
                cout << "0. Back\n";

                choice = getValidatedChoice(0, 3);

                if (choice == 1)
                {
                    cin.ignore();

                    while (true)
                    {
                        cout << "Enter New Name: ";
                        getline(cin, member.memberName);

                        string errorMessage;
                        if (validName(member.memberName, errorMessage))
                        {
                            saveMembers();
                            cout << "\nName updated successfully!\n";
                            break;
                        }
                        else
                        {
                            cout << errorMessage << endl;
                        }
                    }
                }

                else if (choice == 2)
                {
                    cin.ignore();

                    while (true)
                    {
                        cout << "Enter New Phone Number: ";
                        getline(cin, member.phoneNum);

                        string errorMessage;
                        if (validPhone(member.phoneNum, errorMessage))
                        {
                            saveMembers();
                            cout << "Phone number updated successfully!\n";
                            break;
                        }
                        else
                        {
                            cout << errorMessage << endl;
                        }
                    }
                }

                else if (choice == 3)
                {
                    cin.ignore();

                    while (true)
                    {
                        cout << "Enter New Email: ";
                        getline(cin, member.email);

                        string errorMessage;
                        if (validEmail(member.email, errorMessage))
                        {
                            saveMembers();
                            cout << "\nEmail updated successfully!\n";
                            break;
                        }
                        else
                        {
                            cout << errorMessage << endl;
                        }
                    }
                }

                else if (choice == 0) {
                    return;
                }

                else {
                    cout << "Invalid choice.\n";
                }
            }
        }
    }
    cout << "Profile not found.\n";
}

// file handling functions(Member Management)
void saveMembers() {
    ofstream file("members.txt");

    if (!file) {
        cout << "\nError: Unable to save member data.\n";
        pauseScreen();
        return;
    }

    for (const Member& member : members) {
        file << member.memberID << "|" << member.memberName << "|"
            << member.phoneNum << "|" << member.email << "|"
            << member.membershipType << "|" << member.membershipStatus << endl;
    }
    file.close();
}
void loadMembers() {
    ifstream file("members.txt");

    if (!file)
    {
        cout << "Cannot open members.txt!" << endl;
        system("pause");
        return;
    }

    members.clear();

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        Member member;
        stringstream ss(line);

        getline(ss, member.memberID, '|');
        getline(ss, member.memberName, '|');
        getline(ss, member.phoneNum, '|');
        getline(ss, member.email, '|');
        getline(ss, member.membershipType, '|');
        getline(ss, member.membershipStatus);

        members.push_back(member);
    }

    file.close();

    //cout << "Loaded " << members.size() << " member(s)." << endl;
}

string generateMemberID() {
    int highestID = 0;
    for (const Member& member : members)
    {
        if (member.memberID.length() > 1 &&
            member.memberID[0] == 'M')
        {
            try
            {
                int currentID = stoi(member.memberID.substr(1));

                if (currentID > highestID)
                {
                    highestID = currentID;
                }
            }
            catch (...)
            {
                // Ignore invalid Member ID
            }
        }
    }

    highestID++;

    string memberID = "M";
    if (highestID < 10)
    {
        memberID += "00" + to_string(highestID);
    }
    else if (highestID < 100)
    {
        memberID += "0" + to_string(highestID);
    }
    else
    {
        memberID += to_string(highestID);
    }
    return memberID;
}

bool validName(const string& name, string& errorMessage) {
    if (name.empty() || isWhitespaceOnly(name)) {
        errorMessage = "Invalid input. Name cannot be empty.";
        return false;
    }

    for (char c : name) {
        if (!isalpha(c) && c != ' ') {
            errorMessage = "Invalid name. Name can only contain letters and spaces.";
            return false;
        }
    }

    return true;
}

bool validPhone(const string& phone, string& errorMessage) {
    // Check if empty
    if (phone.empty()) {
        errorMessage = "Invalid input. Phone number cannot be empty.";
        return false;
    }

    // Check if all characters are digits
    for (char c : phone) {
        if (!isdigit(c)) {
            errorMessage = "Invalid phone number. Phone number can only contain digits.";
            return false;
        }
    }

    // Check length
    if (phone.length() < 10 || phone.length() > 11) {
        errorMessage = "Invalid phone number. Phone number must be between 10 and 11 digits.";
        return false;
    }
    return true;
}

bool validEmail(const string& email, string& errorMessage) {
    // Check if empty
    if (email.empty() || isWhitespaceOnly(email)) {
        errorMessage = "Invalid input. Email cannot be empty.";
        return false;
    }

    int atCount = 0;
    int dotCount = 0;
    int atPosition = -1;
    int dotPosition = -1;

    for (int i = 0; i < email.length(); i++) {
        if (email[i] == '@') {
            atCount++;
            atPosition = i;
        }

        if (email[i] == '.') {
            dotCount++;
            dotPosition = i;
        }
    }

    if (atCount != 1) {
        errorMessage = "Invalid email format. Email must contain exactly one '@' symbol.";
        return false;
    }

    if (dotCount < 1) {
        errorMessage = "Invalid email format. Email must contain at least one '.' symbol.";
        return false;
    }

    if (atPosition == 0) {
        errorMessage = "Invalid email format. Email cannot start with '@'.";
        return false;
    }

    if (dotPosition <= atPosition + 1) {
        errorMessage = "Invalid email format. There must be characters between '@' and '.'.";
        return false;
    }

    // Check if there are characters after the last dot
    if (dotPosition >= email.length() - 1) {
        errorMessage = "Invalid email format. Email must have characters after the last '.'.";
        return false;
    }

    return true;
}

//login module - Display Login Screen
void loginScreen()
{
    clearScreen();

    cout << "\n\n";
    cout << "               ----------------------------------------------\n";
    cout << "              |   ----------------------------------------   |\n";
    cout << "              |  |                                        |  |\n";
    cout << "              |  |              SMART LIBRARY             |  |\n";
    cout << "              |  |            MANAGEMENT SYSTEM           |  |\n";
    cout << "              |  |                                        |  |\n";
    cout << "              |  |    WELCOME TO THE ULTIMATE READING     |  |\n";
    cout << "              |  |               EXPERIENCE!              |  |\n";
    cout << "              |  |                                        |  |\n";
    cout << "              |  |    +===============================+   |  |\n";
    cout << "              |  |    |      Knowledge is Power.      |   |  |\n";
    cout << "              |  |    |      Read, Learn, Grow!       |   |  |\n";
    cout << "              |  |    +===============================+   |  |\n";
    cout << "              |  |                                        |  |\n";
    cout << "              |  |                                        |  |\n";
    cout << "              |   ----------------------------------------   |\n";
    cout << "               ----------------------------------------------\n";
}

//login module
bool login(string& loggedInRole, string& loggedInMemberID)
{
    string username;
    string password;

    loginScreen();

    cout << "\nWelcome to the Smart Library Management System!\n";
    cout << "Please log in to continue.\n\n";

    cout << "Username: ";
    getline(cin, username);

    cout << "Password: ";
    password = getHiddenPassword();

    if (username.empty()) {
        cout << "\n[ERROR] Username cannot be empty! Please try again.\n\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }

    if (password.empty()) {
        cout << "\n[ERROR] Password cannot be empty! Please try again.\n\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }

    ifstream file("users.txt");

    if (!file)
    {
        cout << "\nError: Unable to open users.txt\n";
        pauseScreen();
        return false;
    }

    string line;

    while (getline(file, line)) {
        if (line.empty())
        {
            continue;
        }

        string storedUsername;
        string storedPassword;
        string storedRole;
        string storedMemberID;

        stringstream ss(line);

        getline(ss, storedUsername, '|');
        getline(ss, storedPassword, '|');
        getline(ss, storedRole, '|');
        getline(ss, storedMemberID);

        // Check login
        if (username == storedUsername && password == storedPassword)
        {
            loggedInRole = storedRole;
            loggedInMemberID = toUpperCase(storedMemberID);

            file.close();

            cout << "\nLogin successful!\n";
            cout << "Welcome " << username << "!\n";

            pauseScreen();

            return true;
        }

    }

    file.close();

    cout << "\nInvalid username or password.\n";
    cout << "Please check your credentials again.\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

string getHiddenPassword() {
    string password;
    char ch;

    while (true)
    {
        ch = _getch();

        if (ch == 13) // Enter key
        {
            cout << endl;
            break;
        }
        else if (ch == 8) // Backspace
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            password += ch;
            cout << "*";
        }
    }

    return password;
}


//BOOK MANAGEMENT//
void bookMenu() {
    int choice;

    // Load books from file on startup
    loadBooksFromFile(books);

    do {
        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "                 BOOK MENU" << endl;
        cout << "=============================================" << endl;
        cout << "1. Add New Book" << endl;
        cout << "2. Display All Books" << endl;
        cout << "3. Search Book" << endl;
        cout << "4. Update Book Information" << endl;
        cout << "5. Delete Book" << endl;
        cout << "0. Back" << endl;
        cout << "=============================================" << endl;
        choice = getValidatedChoice(0, 5);

        // Clear input buffer after reading choice
        cin.ignore();

        switch (choice) {
        case 1:
            addBook(books);
            break;
        case 2:
            displayAllBook(books);
            break;
        case 3:
            searchBook(books);
            break;
        case 4:
            updateBook(books);
            break;
        case 5:
            deleteBook(books);
            break;
        case 0:
            cout << "Returning back to Main Menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again.\n" << endl;
        }

    } while (choice != 0);
}
void addBook(vector<Book>& books) {
    Book newBook;
    string input;
    bool validInput = false;
    string confirm;

    clearScreen();
    cout << "\n=============================================" << endl;
    cout << "                 ADD BOOK" << endl;
    cout << "=============================================" << endl;
    newBook.bookID = generateBookID(books);
    cout << "Generated Book ID: " << newBook.bookID << endl;

    do {
        cout << "Enter Book Title (0 to return): ";
        getline(cin, newBook.bookTitle);

        if (newBook.bookTitle == "0") {
            return;
        }

        if (newBook.bookTitle.empty() || isWhitespaceOnly(newBook.bookTitle)) {
            cout << "Title cannot be empty!" << endl;
        }
        else {

            bool duplicateFound = false;
            string newTitleLower = newBook.bookTitle;
            for (char& c : newTitleLower) {
                c = tolower(c);
            }

            for (int i = 0; i < books.size(); i++) {
                string existingTitleLower = books[i].bookTitle;
                for (char& c : existingTitleLower) {
                    c = tolower(c);
                }

                if (existingTitleLower == newTitleLower) {
                    duplicateFound = true;
                    break;
                }
            }

            if (duplicateFound) {
                cout << "Title already exists in another book!" << endl;
            }
            else {
                validInput = true;
            }
        }
    } while (!validInput);

    validInput = false;

    do {
        cout << "Enter Author Name (0 to return): ";
        getline(cin, newBook.author);

        if (newBook.author == "0") {
            return;
        }

        if (newBook.author.empty() || isWhitespaceOnly(newBook.author)) {
            cout << "Author cannot be empty!" << endl;
        }
        else {
            bool validCharacters = true;
            for (char c : newBook.author) {
                if (!isalpha(c) && c != ' ' && c != '.' && c != '\'' && c != '-' && c != '/' && c != ',') {
                    validCharacters = false;
                    break;
                }
            }

            if (!validCharacters) {
                cout << "Author name can only contain letters, spaces, ., ' and - !" << endl;
            }
            else {
                validInput = true;
            }
        }
    } while (!validInput);

    validInput = false;

    do {
        cout << "Enter ISBN (8 digits or 0 to return): ";
        getline(cin, newBook.isbn);

        if (newBook.isbn == "0") {
            return;
        }

        if (newBook.isbn.empty() || isWhitespaceOnly(newBook.isbn)) {
            cout << "ISBN cannot be empty!" << endl;
        }
        else {
            if (newBook.isbn[0] == '-') {
                cout << "ISBN cannot be negative!" << endl;
                continue;
            }

            // Remove dashes and spaces
            string cleanISBN = "";
            for (char c : newBook.isbn) {
                if (c != '-' && c != ' ') {
                    cleanISBN += c;
                }
            }

            bool isAllDigits = true;
            for (char c : cleanISBN) {
                if (!isdigit(c)) {
                    isAllDigits = false;
                    break;
                }
            }

            if (!isAllDigits || cleanISBN.length() != 8) {
                cout << "ISBN must be 8 digits!" << endl;
            }
            else {
                bool duplicateFound = false;
                for (int i = 0; i < books.size(); i++) {
                    if (books[i].isbn == cleanISBN) {
                        duplicateFound = true;
                        break;
                    }
                }
                if (duplicateFound) {
                    cout << "ISBN already exists in another book!" << endl;
                }
                else {
                    validInput = true;
                    newBook.isbn = cleanISBN;
                }
            }
        }
    } while (!validInput);

    validInput = false;

    do {
        cout << "\nSelect Category: " << endl;
        cout << "1. Fiction          2. Non-Fiction" << endl;
        cout << "3. Science Fiction  4. Fantasy" << endl;
        cout << "5. Romance          6. Mystery" << endl;
        cout << "7. Horror           8. Biography" << endl;
        cout << "9. History          10. Children's" << endl;
        cout << "11. Other (Enter manually)" << endl;
        cout << "0. Back" << endl;
        cout << "Enter your choice (0-11): ";

        string categoryChoice;
        getline(cin, categoryChoice);

        if (categoryChoice == "0") {
            return;
        }

        if (!isValidPositiveNumber(categoryChoice)) {
            cout << "Please enter a valid number!" << endl;
        }
        else {
            int choice = safeStoi(categoryChoice);
            if (choice == -1) {
                cout << "Please enter a valid number!" << endl;
                continue;
            }

            switch (choice) {
            case 1:
                newBook.category = "Fiction";
                validInput = true;
                break;
            case 2:
                newBook.category = "Non-Fiction";
                validInput = true;
                break;
            case 3:
                newBook.category = "Science Fiction";
                validInput = true;
                break;
            case 4:
                newBook.category = "Fantasy";
                validInput = true;
                break;
            case 5:
                newBook.category = "Romance";
                validInput = true;
                break;
            case 6:
                newBook.category = "Mystery";
                validInput = true;
                break;
            case 7:
                newBook.category = "Horror";
                validInput = true;
                break;
            case 8:
                newBook.category = "Biography";
                validInput = true;
                break;
            case 9:
                newBook.category = "History";
                validInput = true;
                break;
            case 10:
                newBook.category = "Children's";
                validInput = true;
                break;
            case 11:
                do {
                    cout << "Enter Category: ";
                    getline(cin, newBook.category);

                    if (newBook.category.empty() || isWhitespaceOnly(newBook.category)) {
                        cout << "Category cannot be empty" << endl;
                    }
                    else {
                        validInput = true;
                    }
                } while (!validInput);
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }
    } while (!validInput);

    validInput = false;

    do {
        cout << "Enter Quantity (0 to return): ";
        getline(cin, input);

        if (input == "0") {
            return;
        }

        if (!isValidPositiveNumber(input)) {
            cout << "Please enter a valid positive number!" << endl;
        }
        else {
            int qty = safeStoi(input);
            if (qty == -1) {
                cout << "Please enter a valid number!" << endl;
                continue;
            }
            if (qty <= 0) {
                cout << "Quantity must be greater than 0!" << endl;
            }
            else {
                newBook.quantity = qty;
                newBook.availableCopies = qty;
                validInput = true;
            }
        }
    } while (!validInput);

    cout << "\n-------------------------------------------" << endl;
    cout << "              BOOK DETAILS" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Book ID    : " << newBook.bookID << endl;
    cout << "Title      : " << newBook.bookTitle << endl;
    cout << "Author     : " << newBook.author << endl;
    cout << "ISBN       : " << newBook.isbn << endl;
    cout << "Category   : " << newBook.category << endl;
    cout << "Quantity   : " << newBook.quantity << endl;
    cout << "Available  : " << newBook.availableCopies << endl;
    cout << "-------------------------------------------" << endl;

    do {
        cout << "\nDo you want to save this book? (y/n): ";
        getline(cin, confirm);

        if (confirm == "y" || confirm == "Y") {
            books.push_back(newBook);
            cout << "Book Added Succefully!" << endl;
            cout << "\nPlease press \"Enter\" key to Continue.....";

            cin.get();
            break;
        }
        else if (confirm == "n" || confirm == "N") {
            cout << "\nBook addition cancelled." << endl;
            cout << "\nPlease press \"Enter\" key to Continue.....";

            cin.get();
            clearScreen();
            break;
        }
        else {
            cout << "Please enter 'y' for yes or 'n' for no!" << endl;
        }
    } while (true);

    saveBooksToFile(books);
}
void updateBook(vector<Book>& books) {
    string bookID;
    int choice;
    bool bookFound = false;
    string confirm;

    clearScreen();
    if (books.empty()) {
        cout << "\n--- No books in the system to update. ---" << endl;
        cout << "\nPlease press \"Enter\" key to Continue.....";
        cin.get();
        return;
    }

    sort(
        books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.bookID < b.bookID;
        }
    );

    cout << "\n=============================================" << endl;
    cout << "                UPDATE BOOK" << endl;
    cout << "=============================================" << endl;

    cout << left
        << string(152, '-') << endl
        << setw(6) << "BookID" << " | "
        << setw(50) << "Title" << " | "
        << setw(40) << "Author" << " | "
        << setw(8) << "ISBN" << " | "
        << setw(15) << "Category" << " | "
        << setw(6) << "  Qty" << " | "
        << setw(10) << "Available" << endl;
    cout << string(152, '-') << endl;

    for (int i = 0; i < books.size(); i++) {
        cout << left;
        cout << " " << setw(5) << books[i].bookID << " | ";
        cout << setw(50) << books[i].bookTitle << " | ";
        cout << setw(40) << books[i].author << " | ";
        cout << setw(8) << books[i].isbn << " | ";
        cout << setw(15) << books[i].category << " | ";
        cout << right;
        cout << setw(6) << books[i].quantity << " | ";
        cout << setw(5) << books[i].availableCopies << endl;
    }
    cout << string(152, '-') << endl;

    do {
        cout << "Enter Book ID to update (0 to return): ";
        getline(cin, bookID);

        if (bookID == "0") {
            clearScreen();
            return;
        }

        if (bookID.empty()) {
            cout << "Book ID cannot be empty!" << endl;
        }
        else {
            string upperBookID = bookID;
            for (int i = 0; i < upperBookID.length(); i++) {
                upperBookID[i] = toupper(upperBookID[i]);
            }

            if (!isValidBookID(upperBookID)) {
                cout << "Please enter a valid Book ID (format: B001, B002, etc.)" << endl;
            }
            else {
                int index = findBookIndex(books, upperBookID);
                if (index != -1) {
                    bookFound = true;
                    bookID = upperBookID;
                }
                else {
                    cout << "Book with ID '" << upperBookID << "' not found!" << endl;
                    cout << "Please enter a valid Book ID" << endl;
                }
            }
        }
    } while (!bookFound);

    int index = findBookIndex(books, bookID);

    if (index == -1) {
        cout << "Book with ID '" << bookID << "'" << endl;
        return;
    }

    Book& book = books[index];

    clearScreen();
    cout << "\nOriginal Book Information:" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Book ID   : " << book.bookID << endl;
    cout << "Title     : " << book.bookTitle << endl;
    cout << "Author    : " << book.author << endl;
    cout << "ISBN      : " << book.isbn << endl;
    cout << "Category  : " << book.category << endl;
    cout << "Quantity  : " << book.quantity << endl;
    cout << "Available : " << book.availableCopies << endl;
    cout << "---------------------------------------------" << endl;

    Book updatedBook = book;
    bool hasChanges = false;

    do {
        cout << "\nWhat would you like to update?" << endl;
        cout << "1. Title      2. Author" << endl;
        cout << "3. ISBN       4. Category" << endl;
        cout << "5. Quantity   6. Save Changes" << endl;
        cout << "0. Cancel Update" << endl;

        choice = getValidatedChoice(0, 6);
        cin.ignore();

        string input;
        bool validInput = false;

        switch (choice) {
        case 1:
            do {
                cout << "Enter new Title (0 to return): ";
                getline(cin, updatedBook.bookTitle);

                if (updatedBook.bookTitle == "0") {

                    clearScreen();
                    cout << "\nOriginal Book Information:" << endl;
                    cout << "---------------------------------------------" << endl;
                    cout << "Book ID   : " << book.bookID << endl;
                    cout << "Title     : " << book.bookTitle << endl;
                    cout << "Author    : " << book.author << endl;
                    cout << "ISBN      : " << book.isbn << endl;
                    cout << "Category  : " << book.category << endl;
                    cout << "Quantity  : " << book.quantity << endl;
                    cout << "Available : " << book.availableCopies << endl;
                    cout << "---------------------------------------------" << endl;

                    updatedBook = book;
                    validInput = true;  //  exit the loop
                    break;
                }
                if (updatedBook.bookTitle.empty() || isWhitespaceOnly(updatedBook.bookTitle)) {
                    cout << "Title cannot be empty!" << endl;
                }
                else {
                    string newTitleLower = updatedBook.bookTitle;
                    for (char& c : newTitleLower) {
                        c = tolower(c);
                    }

                    string originalTitleLower = book.bookTitle;
                    for (char& c : originalTitleLower) {
                        c = tolower(c);
                    }

                    if (newTitleLower == originalTitleLower) {
                        cout << "No change detected! Title is the same as original" << endl;
                    }
                    else {
                        bool duplicateFound = false;

                        for (int i = 0; i < books.size(); i++) {
                            if (i != index) {
                                string existingTitleLower = books[i].bookTitle;
                                for (char& c : existingTitleLower) {
                                    c = tolower(c);
                                }

                                if (existingTitleLower == newTitleLower) {
                                    duplicateFound = true;
                                    break;
                                }
                            }
                        }

                        if (duplicateFound) {
                            cout << "Title already exists in another book!" << endl;
                        }
                        else {
                            validInput = true;
                            hasChanges = true;
                            cout << "Title updated!" << endl;
                        }
                    }
                }
            } while (!validInput);
            break;

        case 2:
            do {
                cout << "Enter new Author (0 to return): ";
                getline(cin, updatedBook.author);

                if (updatedBook.author == "0") {
                    clearScreen();
                    cout << "\nOriginal Book Information:" << endl;
                    cout << "---------------------------------------------" << endl;
                    cout << "Book ID   : " << book.bookID << endl;
                    cout << "Title     : " << book.bookTitle << endl;
                    cout << "Author    : " << book.author << endl;
                    cout << "ISBN      : " << book.isbn << endl;
                    cout << "Category  : " << book.category << endl;
                    cout << "Quantity  : " << book.quantity << endl;
                    cout << "Available : " << book.availableCopies << endl;
                    cout << "---------------------------------------------" << endl;

                    updatedBook.author = book.author;
                    validInput = true;
                    break;
                }
                if (updatedBook.author.empty() || isWhitespaceOnly(updatedBook.author)) {
                    cout << "Author cannot be empty!" << endl;
                }
                else {
                    bool validCharacters = true;
                    for (char c : updatedBook.author) {
                        if (!isalpha(c) && c != ' ' && c != '.' && c != '\'' && c != '-') {
                            validCharacters = false;
                            break;
                        }
                    }

                    if (!validCharacters) {
                        cout << "Author name can only contain letters, spaces, ., ' and - !" << endl;
                    }
                    else if (updatedBook.author == book.author) {
                        cout << "No change detected! Author is the same as original" << endl;
                    }
                    else {
                        validInput = true;
                        hasChanges = true;
                        cout << "Author updated!" << endl;
                    }
                }
            } while (!validInput);
            break;

        case 3:
            do {
                cout << "Enter new ISBN (8 digits, 0 to return): ";
                getline(cin, updatedBook.isbn);

                if (updatedBook.isbn == "0") {
                    clearScreen();
                    cout << "\nOriginal Book Information:" << endl;
                    cout << "---------------------------------------------" << endl;
                    cout << "Book ID   : " << book.bookID << endl;
                    cout << "Title     : " << book.bookTitle << endl;
                    cout << "Author    : " << book.author << endl;
                    cout << "ISBN      : " << book.isbn << endl;
                    cout << "Category  : " << book.category << endl;
                    cout << "Quantity  : " << book.quantity << endl;
                    cout << "Available : " << book.availableCopies << endl;
                    cout << "---------------------------------------------" << endl;

                    updatedBook.isbn = book.isbn;
                    validInput = true;
                    break;
                }

                if (updatedBook.isbn.empty()) {
                    cout << "ISBN cannot be empty!" << endl;
                }
                else {
                    if (updatedBook.isbn[0] == '-') {
                        cout << "ISBN cannot be negative!" << endl;
                        continue;
                    }

                    string cleanISBN = "";
                    for (char c : updatedBook.isbn) {
                        if (c != '-' && c != ' ') {
                            cleanISBN += c;
                        }
                    }

                    bool isAllDigits = true;
                    for (char c : cleanISBN) {
                        if (!isdigit(c)) {
                            isAllDigits = false;
                            break;
                        }
                    }

                    if (!isAllDigits || cleanISBN.length() != 8) {
                        cout << "ISBN must be 8 digits!" << endl;
                    }
                    else {
                        if (cleanISBN == book.isbn) {
                            cout << "No change detected! ISBN is the same as original" << endl;
                        }
                        else {
                            bool duplicateFound = false;
                            for (int i = 0; i < books.size(); i++) {
                                if (i != index && books[i].isbn == cleanISBN) {
                                    duplicateFound = true;
                                    break;
                                }
                            }
                            if (duplicateFound) {
                                cout << "ISBN already exists in another book!" << endl;
                            }
                            else {
                                validInput = true;
                                hasChanges = true;
                                updatedBook.isbn = cleanISBN;
                                cout << "ISBN updated to: " << updatedBook.isbn << endl;
                            }
                        }
                    }
                }
            } while (!validInput);
            break;

        case 4:
            do {
                cout << "\nSelect Category: " << endl;
                cout << "1. Fiction          2. Non-Fiction" << endl;
                cout << "3. Science Fiction  4. Fantasy" << endl;
                cout << "5. Romance          6. Mystery" << endl;
                cout << "7. Horror           8. Biography" << endl;
                cout << "9. History          10. Children's" << endl;
                cout << "11. Other (Enter manually)" << endl;
                cout << "0. Back" << endl;
                cout << "Enter your choice (0-11): ";

                string categoryChoice;
                getline(cin, categoryChoice);

                if (categoryChoice == "0") {
                    clearScreen();
                    cout << "\nOriginal Book Information:" << endl;
                    cout << "---------------------------------------------" << endl;
                    cout << "Book ID   : " << book.bookID << endl;
                    cout << "Title     : " << book.bookTitle << endl;
                    cout << "Author    : " << book.author << endl;
                    cout << "ISBN      : " << book.isbn << endl;
                    cout << "Category  : " << book.category << endl;
                    cout << "Quantity  : " << book.quantity << endl;
                    cout << "Available : " << book.availableCopies << endl;
                    cout << "---------------------------------------------" << endl;

                    updatedBook.category = book.category;
                    validInput = true;
                    break;
                }

                if (!isValidPositiveNumber(categoryChoice)) {
                    cout << "Please enter a valid number!" << endl;
                }
                else {
                    int catChoice = safeStoi(categoryChoice);
                    string newCategory;

                    switch (catChoice) {
                    case 1:
                        newCategory = "Fiction";
                        break;
                    case 2:
                        newCategory = "Non-Fiction";
                        break;
                    case 3:
                        newCategory = "Science Fiction";
                        break;
                    case 4:
                        newCategory = "Fantasy";
                        break;
                    case 5:
                        newCategory = "Romance";
                        break;
                    case 6:
                        newCategory = "Mystery";
                        break;
                    case 7:
                        newCategory = "Horror";
                        break;
                    case 8:
                        newCategory = "Biography";
                        break;
                    case 9:
                        newCategory = "History";
                        break;
                    case 10:
                        newCategory = "Children's";
                        break;
                    case 11:
                        do {
                            cout << "Enter Category: ";
                            getline(cin, newCategory);

                            if (newCategory.empty() || isWhitespaceOnly(newCategory)) {
                                cout << "Category cannot be empty!" << endl;
                            }
                            else {
                                break;
                            }
                        } while (true);
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                        continue;
                    }

                    if (newCategory == book.category) {
                        cout << "No change detected! Category is the same as original" << endl;
                    }
                    else {
                        updatedBook.category = newCategory;
                        validInput = true;
                        hasChanges = true;
                        cout << "Category updated to: " << updatedBook.category << endl;
                    }
                }
            } while (!validInput);
            break;

        case 5:
            do {
                cout << "Enter new Quantity (0 to return): ";
                getline(cin, input);

                if (input == "0") {
                    clearScreen();
                    cout << "\nOriginal Book Information:" << endl;
                    cout << "---------------------------------------------" << endl;
                    cout << "Book ID   : " << book.bookID << endl;
                    cout << "Title     : " << book.bookTitle << endl;
                    cout << "Author    : " << book.author << endl;
                    cout << "ISBN      : " << book.isbn << endl;
                    cout << "Category  : " << book.category << endl;
                    cout << "Quantity  : " << book.quantity << endl;
                    cout << "Available : " << book.availableCopies << endl;
                    cout << "---------------------------------------------" << endl;
                    break;
                }
                if (!isValidPositiveNumber(input)) {
                    cout << "Please enter a valid positive number!" << endl;
                }
                else {
                    int qty = safeStoi(input);
                    if (qty == -1) {
                        cout << "Please enter a valid number!" << endl;
                        continue;
                    }
                    else if (qty == book.quantity) {
                        cout << "No change detected! Quantity is the same as original" << endl;
                    }
                    else {
                        int borrowed = book.quantity - book.availableCopies;
                        if (qty < borrowed) {
                            cout << "WARNING: " << borrowed << " copy(ies) are currently borrowed" << endl;
                            cout << "New quantity (" << qty << ") is less than borrowed copies." << endl;
                            cout << "Available copies will be set to 0." << endl;
                            updatedBook.quantity = qty;
                            updatedBook.availableCopies = 0;
                        }
                        else {
                            updatedBook.quantity = qty;
                            updatedBook.availableCopies = qty - borrowed;
                        }
                        validInput = true;
                        hasChanges = true;
                        cout << "Quantity updated!" << endl;
                    }
                }
            } while (!validInput);
            break;

        case 6:
            if (!hasChanges) {
                cout << "\nNo changes made to save." << endl;
                cout << "\nPlease press \"Enter\" key to Continue.....";
                cin.get();
            }
            else {
                clearScreen();
                cout << "\nUpdated Book Information:" << endl;
                cout << "---------------------------------------------" << endl;
                cout << "Book ID   : " << updatedBook.bookID << endl;
                cout << "Title     : " << updatedBook.bookTitle << endl;
                cout << "Author    : " << updatedBook.author << endl;
                cout << "ISBN      : " << updatedBook.isbn << endl;
                cout << "Category  : " << updatedBook.category << endl;
                cout << "Quantity  : " << updatedBook.quantity << endl;
                cout << "Available : " << updatedBook.availableCopies << endl;
                cout << "---------------------------------------------" << endl;

                do {
                    cout << "\nDo you want to save these changes? (y/n): ";
                    getline(cin, confirm);

                    if (confirm == "y" || confirm == "Y") {
                        book = updatedBook;
                        saveBooksToFile(books);
                        cout << "\nBook Updated Successfully!" << endl;
                        cout << "\nPlease press \"Enter\" key to Continue";

                        cin.get();
                        return;
                    }
                    else if (confirm == "n" || confirm == "N") {
                        cout << "Book Update cancelled." << endl;
                        cout << "\nPlease press \"Enter\" key to Continue";

                        cin.get();
                        return;
                    }
                    else {
                        cout << "Please enter 'y' for yes or 'n' for no!" << endl;
                    }
                } while (true);
            }
            break;

        case 0:
            cout << "\nUpdate cancelled." << endl;
            cout << "\nPlease press \"Enter\" key to Continue";

            cin.get();
            return;

        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 6 && choice != 0);
}
void deleteBook(vector<Book>& books) {
    clearScreen();
    if (books.empty()) {
        cout << "\n--- No books in the system to delete. ---" << endl;
        cout << "\nPlease press \"Enter\" key back to Book Menu";

        cin.get();
        return;
    }

    string bookID;
    bool bookFound = false;
    string confirm;

    sort(
        books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.bookID < b.bookID;
        }
    );

    cout << "\n=============================================" << endl;
    cout << "                DELETE BOOK" << endl;
    cout << "=============================================" << endl;

    cout << left
        << string(152, '-') << endl
        << setw(6) << "BookID" << " | "
        << setw(50) << "Title" << " | "
        << setw(40) << "Author" << " | "
        << setw(8) << "ISBN" << " | "
        << setw(15) << "Category" << " | "
        << setw(6) << "  Qty" << " | "
        << setw(10) << "Available" << endl;
    cout << string(152, '-') << endl;

    for (int i = 0; i < books.size(); i++) {
        cout << left;
        cout << " " << setw(5) << books[i].bookID << " | ";
        cout << setw(50) << books[i].bookTitle << " | ";
        cout << setw(40) << books[i].author << " | ";
        cout << setw(8) << books[i].isbn << " | ";
        cout << setw(15) << books[i].category << " | ";
        cout << right;
        cout << setw(6) << books[i].quantity << " | ";
        cout << setw(5) << books[i].availableCopies << endl;
    }
    cout << string(152, '-') << endl;

    do {
        cout << "\nEnter Book ID to delete (0 to return): ";
        getline(cin, bookID);

        if (bookID == "0") {
            return;
        }

        if (bookID.empty() || isWhitespaceOnly(bookID)) {
            cout << "Book ID cannot be empty!" << endl;
        }
        else {
            string upperBookID = bookID;
            for (int i = 0; i < upperBookID.length(); i++) {
                upperBookID[i] = toupper(upperBookID[i]);
            }

            if (!isValidBookID(upperBookID)) {
                cout << "Please enter a valid Book ID (format: B001, B002, etc.)" << endl;
            }
            else {
                int index = findBookIndex(books, upperBookID);
                if (index != -1) {
                    bookFound = true;
                    bookID = upperBookID;
                }
                else {
                    cout << "Book with ID '" << upperBookID << "' not found!" << endl;
                    cout << "Please enter a valid Book ID" << endl;
                }
            }
        }
    } while (!bookFound);

    int index = findBookIndex(books, bookID);

    if (index == -1) {
        cout << "Book with ID '" << bookID << "' not found!" << endl;
        cout << "\nPlease press \"Enter\" key back to Book Menu";
        cin.get();
        return;
    }

    clearScreen();
    cout << "\nCurrent Book Information To Delete:" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "Book ID:     " << books[index].bookID << endl;
    cout << "Title:       " << books[index].bookTitle << endl;
    cout << "Author:      " << books[index].author << endl;
    cout << "ISBN:        " << books[index].isbn << endl;
    cout << "Category:    " << books[index].category << endl;
    cout << "Quantity:    " << books[index].quantity << endl;
    cout << "Available:   " << books[index].availableCopies << endl;

    int borrowed = books[index].quantity - books[index].availableCopies;
    if (borrowed > 0) {
        cout << "WARNING: " << borrowed << " copy(ies) are currently borrowed!" << endl;
        cout << "Deleting this book may cause data inconsistency." << endl;
    }
    cout << "---------------------------------------------" << endl;

    do {
        cout << "\nAre you sure you want to delete this book? (y/n): ";
        getline(cin, confirm);

        if (confirm == "y" || confirm == "Y") {
            string deletedID = books[index].bookID;
            string deletedTitle = books[index].bookTitle;
            string deletedAuthor = books[index].author;
            string deletedCategory = books[index].category;

            books.erase(books.begin() + index);
            saveBooksToFile(books);

            cout << "\nBook Deletion Successfully!" << endl;
            cout << "\nPlease press \"Enter\" key to Continue";

            cin.get();
            break;
        }
        else if (confirm == "n" || confirm == "N") {
            cout << "\nBook Deletion Cancelled." << endl;
            cout << "\nPlease press \"Enter\" key to Continue";

            cin.get();
            break;
        }
        else {
            cout << "Please enter 'y' for yes or 'n' for no!" << endl;
        }
    } while (true);
}
void searchBook(const vector<Book>& books) {
    clearScreen();
    if (books.empty()) {
        cout << "\n--- No books in the system to search. ---" << endl;
        cout << "\nPlease press \"Enter\" key to Continue.....";

        cin.get();
        return;
    }

    int searchChoice;
    string searchTerm;
    bool found = false;
    int matchCount = 0;

    do {
        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "                SEARCH BOOK" << endl;
        cout << "=============================================" << endl;

        cout << string(122, '-') << endl;
        cout << left
            << setw(7) << " BookID" << " | "
            << setw(50) << "Title" << " | "
            << setw(40) << "Author" << " | "
            << setw(11) << "Category" << endl;
        cout << string(122, '-') << endl;

        for (int i = 0; i < books.size(); i++) {
            cout << left;
            cout << "  " << setw(5) << books[i].bookID << " | ";
            cout << setw(50) << books[i].bookTitle << " | ";
            cout << setw(40) << books[i].author << " | ";
            cout << setw(11) << books[i].category << endl;
        }
        cout << string(122, '-') << endl;

        cout << "\nSearch by:" << endl;
        cout << "1. Book ID    2. Title" << endl;
        cout << "3. Author     4. Category" << endl;
        cout << "0. Back" << endl;
        searchChoice = getValidatedChoice(0, 4);
        cin.ignore();

        if (searchChoice == 0) {
            return;
        }

        cout << "Enter search term: ";
        getline(cin, searchTerm);

        if (searchTerm.empty() || isWhitespaceOnly(searchTerm)) {
            cout << "Search term cannot be empty!" << endl;
            continue;
        }

        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "              SEARCH RESULT" << endl;
        cout << "=============================================" << endl;

        for (int i = 0; i < books.size(); i++) {
            bool match = false;

            string searchLower = searchTerm;
            for (int j = 0; j < searchLower.length(); j++) {
                searchLower[j] = tolower(searchLower[j]);
            }

            if (searchChoice == 1) {
                string idLower = books[i].bookID;
                for (int j = 0; j < idLower.length(); j++) {
                    idLower[j] = tolower(idLower[j]);
                }
                if (idLower.find(searchLower) != string::npos) {
                    match = true;
                }
            }
            else if (searchChoice == 2) {
                string titleLower = books[i].bookTitle;
                for (int j = 0; j < titleLower.length(); j++) {
                    titleLower[j] = tolower(titleLower[j]);
                }
                if (titleLower.find(searchLower) != string::npos) {
                    match = true;
                }
            }
            else if (searchChoice == 3) {
                string authorLower = books[i].author;
                for (int j = 0; j < authorLower.length(); j++) {
                    authorLower[j] = tolower(authorLower[j]);
                }
                if (authorLower.find(searchLower) != string::npos) {
                    match = true;
                }
            }
            else if (searchChoice == 4) {
                string categoryLower = books[i].category;
                for (int j = 0; j < categoryLower.length(); j++) {
                    categoryLower[j] = tolower(categoryLower[j]);
                }
                if (categoryLower.find(searchLower) != string::npos) {
                    match = true;
                }
            }

            if (match) {
                cout << "\nBook found!" << endl;
                cout << "---------------------------------------------" << endl;
                cout << "Book ID:     " << books[i].bookID << endl;
                cout << "Title:       " << books[i].bookTitle << endl;
                cout << "Author:      " << books[i].author << endl;
                cout << "ISBN:        " << books[i].isbn << endl;
                cout << "Category:    " << books[i].category << endl;
                cout << "Quantity:    " << books[i].quantity << endl;
                cout << "Available:   " << books[i].availableCopies << endl;
                cout << "---------------------------------------------" << endl;
                found = true;
                matchCount++;
            }
        }

        if (!found) {
            cout << "\nNo books found matching '" << searchTerm << "'" << endl;
        }
        else {
            cout << "\nTotal matches found: " << matchCount << endl;
        }

        string again;
        do {
            cout << "\nDo you want to search again? (y/n): ";
            cin >> again;
            cin.ignore();

            if (again == "y" || again == "Y") {
                break;
            }
            else if (again == "n" || again == "N") {
                return;
            }
            else {
                cout << "Please enter 'y' for yes or 'n' for no!" << endl;
            }
        } while (true);

        found = false;
        matchCount = 0;

    } while (true);
}
void displayAllBook(const vector<Book>& books) {
    clearScreen();
    if (books.empty()) {
        cout << "\n--- No books found in the system. ---" << endl;
        cout << "\nPlease press \"Enter\" key to Continue.....";

        cin.get();
        return;
    }
    else {
        vector<Book> sortedBooks = books;
        sort(
            sortedBooks.begin(), sortedBooks.end(), [](const Book& a, const Book& b) {
                return a.bookID < b.bookID;
            }
        );

        cout << "\n=============================================" << endl;
        cout << "                ALL BOOKS" << endl;
        cout << "=============================================" << endl;

        cout << string(152, '-') << endl;

        cout << left
            << setw(6) << "BookID" << " | "
            << setw(50) << "Title" << " | "
            << setw(40) << "Author" << " | "
            << setw(8) << "ISBN" << " | "
            << setw(15) << "Category" << " | "
            << setw(6) << "  Qty" << " | "
            << setw(10) << "Available" << endl;
        cout << string(152, '-') << endl;

        for (int i = 0; i < sortedBooks.size(); i++) {
            cout << left;
            cout << " " << setw(5) << sortedBooks[i].bookID << " | ";
            cout << setw(50) << sortedBooks[i].bookTitle << " | ";
            cout << setw(40) << sortedBooks[i].author << " | ";
            cout << setw(8) << sortedBooks[i].isbn << " | ";
            cout << setw(15) << sortedBooks[i].category << " | ";
            cout << right;
            cout << setw(6) << sortedBooks[i].quantity << " | ";
            cout << setw(5) << sortedBooks[i].availableCopies << endl;
        }
        cout << string(152, '-') << endl;
        cout << "Total Books: " << books.size() << " book(s)" << endl;
    }
    cout << "\nPlease press \"Enter\" key to Continue.....";

    cin.get();
    clearScreen();
    return;

}

// file handling functions(Book Management)
void loadBooksFromFile(vector<Book>& books) {
    books.clear();

    ifstream file(BOOKS_FILE);

    if (!file.is_open()) {
        cout << "books.txt not found. Creating new file..." << endl;
        ofstream newFile("books.txt");
        if (newFile.is_open()) {
            newFile << "BookID|Title|Author|ISBN|Category|Quantity|AvailableCopies" << endl;
            newFile.close();
            cout << "books.txt created successfully!" << endl;
        }
        return;
    }

    Book book;
    string line;

    if (!getline(file, line)) {
        cout << "File is empty." << endl;
        file.close();
        return;
    }

    int bookCount = 0;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);

        if (getline(ss, book.bookID, '|') &&
            getline(ss, book.bookTitle, '|') &&
            getline(ss, book.author, '|') &&
            getline(ss, book.isbn, '|') &&
            getline(ss, book.category, '|')) {

            string qtyStr, availStr;
            if (getline(ss, qtyStr, '|') && getline(ss, availStr)) {
                // Use safe conversion
                book.quantity = safeStoiFromFile(qtyStr, 0);
                book.availableCopies = safeStoiFromFile(availStr, 0);

                if (book.quantity > 0) {
                    books.push_back(book);
                    bookCount++;
                }
                else {
                    cout << "Warning: Skipping book with invalid quantity: " << book.bookID << endl;
                }
            }
        }
    }

    file.close();
    //cout << "Loaded " << bookCount << " book(s) from file." << endl;
}
void saveBooksToFile(const vector<Book>& books) {
    ofstream file("books.txt");

    if (!file.is_open()) {
        cout << "Unable to save books to file!" << endl;
        pauseScreen();
        return;
    }

    file << "BookID|Title|Author|ISBN|Category|Quantity|AvailableCopies" << endl;

    for (const Book& book : books) {
        file << book.bookID << "|"
            << book.bookTitle << "|"
            << book.author << "|"
            << book.isbn << "|"
            << book.category << "|"
            << book.quantity << "|"
            << book.availableCopies << endl;
    }

    file.close();
}
int safeStoiFromFile(const string& str, int defaultValue) {
    if (str.empty()) {
        return defaultValue;
    }

    // Check if all characters are digits
    for (char c : str) {
        if (!isdigit(c)) {
            return defaultValue;
        }
    }

    try {
        size_t pos;
        int result = stoi(str, &pos);
        if (pos != str.length()) {
            return defaultValue;
        }
        return result;
    }
    catch (...) {
        return defaultValue;
    }
}

//Helper Functions(Book Management)
string generateBookID(const vector<Book>& books) {
    if (books.empty()) {
        return "B001";
    }

    vector<int> existingNumbers;
    for (const Book& book : books) {
        string id = book.bookID;
        if (id.length() >= 2 && id[0] == 'B') {
            string numPart = id.substr(1);
            if (isValidPositiveNumber(numPart)) {
                int num = safeStoi(numPart);
                if (num != -1) {
                    existingNumbers.push_back(num);
                }
            }
        }
    }

    sort(existingNumbers.begin(), existingNumbers.end());

    int expectedNum = 1;
    for (int num : existingNumbers) {
        if (num == expectedNum) {
            expectedNum++;
        }
        else if (num > expectedNum) {
            break;
        }
    }

    stringstream ss;
    ss << "B" << setw(3) << setfill('0') << expectedNum;
    return ss.str();
}
bool isValidBookID(const string& id) {
    if (id.length() != 4) {
        return false;
    }

    if (toupper(id[0]) != 'B') {
        return false;
    }

    for (int i = 1; i < 4; i++) {
        if (!isdigit(id[i])) {
            return false;
        }
    }

    return true;
}
bool isValidPositiveNumber(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}
int findBookIndex(const vector<Book>& books, const string& bookID) {
    for (size_t i = 0; i < books.size(); i++) {
        if (books[i].bookID == bookID) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
bool isWhitespaceOnly(const string& str) {
    for (char c : str) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
    }
    return true;
}

//REPORTING & FINE MANAGEMENT//
//Admin
void adminReportingAndFineMenu() {
    int choice;

    do {
        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "      ADMIN - REPORTING & FINE MANAGEMENT" << endl;
        cout << "=============================================" << endl;
        cout << "1. Calculate Overdue Fine" << endl;
        cout << "2. Pay Fine" << endl;
        cout << "3. Display Borrowing Report" << endl;
        cout << "4. Display Popular Books Report" << endl;
        cout << "5. Display Library Statistics" << endl;
        cout << "0. Back" << endl;
        cout << "=============================================" << endl;

        choice = getValidatedChoice(0, 5);
        cin.ignore();

        switch (choice) {
        case 1:
            calculateOverdueFine();
            break;
        case 2:
            payFine();
            break;
        case 3:
            displayBorrowingReport();
            break;
        case 4:
            displayPopularBooksReport();
            break;
        case 5:
            displayLibraryStatistics();
            break;
        case 0:
            cout << "\nReturning to Admin Menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);
}

void memberReportingAndFineMenu(const string& memberID) {
    int choice;

    do {
        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "     MEMBER - REPORTING & FINE MANAGEMENT" << endl;
        cout << "=============================================" << endl;
        cout << "1. Pay My Fine" << endl;
        cout << "2. View Popular Books Report" << endl;
        cout << "0. Back" << endl;
        cout << "=============================================" << endl;

        choice = getValidatedChoice(0, 2);
        cin.ignore();

        switch (choice) {
        case 1:
            payFine(memberID);
            break;
        case 2:
            displayPopularBooksReport();
            break;
        case 0:
            cout << "\nReturning to Member Menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);
}

double calculateFineAmount(const string& dueDate, const string& returnDate) {
    int overdueDays = daysBetweenDates(dueDate, returnDate);
    if (overdueDays <= 0) {
        return 0.0;
    }
    return overdueDays * FINE_RATE_PER_DAY;
}

// FIXED: applyFineRules now subtracts whatever has already been paid
// (record.amountPaid) from the freshly-recalculated total fine, instead
// of overwriting fineAmount with the full amount every time this runs.
// This is what makes partial payments "stick" across repeated calls
// from payFine(), calculateOverdueFine(), and updateOverdueStatus().
bool applyFineRules(BorrowRecord& record, const string& asOfDate) {
    bool changed = false;

    // Once a record is fully Paid, never recalculate or reopen it here.
    if (record.paymentStatus == "Paid") {
        return false;
    }

    int overdueDays = daysBetweenDates(record.dueDate, asOfDate);

    if (overdueDays > 0) {
        double calculatedFine = calculateFineAmount(record.dueDate, asOfDate);
        double owed = calculatedFine - record.amountPaid;
        if (owed < 0.0) {
            owed = 0.0;
        }

        string oldStatus = record.paymentStatus;
        double oldFine = record.fineAmount;

        if (owed <= FINE_EPSILON) {
            record.fineAmount = 0.0;
            record.paymentStatus = "Paid";
        }
        else {
            record.fineAmount = owed;
            record.paymentStatus = "Unpaid";
        }

        if (record.paymentStatus != oldStatus || fabs(record.fineAmount - oldFine) > FINE_EPSILON) {
            changed = true;
        }
    }

    return changed;
}
void calculateOverdueFine() {
    loadBorrowRecords(borrowRecords);
    loadMembers();
    loadBooksFromFile(books);

    bool keepGoing = true;

    while (keepGoing) {
        clearScreen();
        cout << "\n=============================================\n";
        cout << "          CALCULATE OVERDUE FINE" << endl;
        cout << "=============================================\n";

        string memberID, bookID;
        bool memberValid = false;
        bool bookValid = false;
        bool cancelled = false;

        // ===== Validate Member ID =====
        while (!memberValid && !cancelled) {
            cout << "Enter Member ID (or '0' to return): ";
            cin >> memberID;

            if (memberID == "0") {
                cancelled = true;
                break;
            }

            memberID = toUpperCase(memberID);

            bool memberExists = false;
            for (const auto& member : members) {
                if (member.memberID == memberID) {
                    memberExists = true;
                    break;
                }
            }

            if (!memberExists) {
                cout << "Member ID '" << memberID
                    << "' not found. Please try again." << endl;
            }
            else {
                memberValid = true;
            }
        }

        if (cancelled) {
            cout << "\nOperation cancelled." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pauseScreen();
            return;
        }

        // ===== Validate Book ID =====
        while (!bookValid && !cancelled) {
            cout << "\nEnter Book ID (or '0' to return): ";
            cin >> bookID;

            if (bookID == "0") {
                cancelled = true;
                break;
            }

            bookID = toUpperCase(bookID);

            bool bookExists = false;
            for (const auto& book : books) {
                if (book.bookID == bookID) {
                    bookExists = true;
                    break;
                }
            }

            if (!bookExists) {
                cout << "Book ID '" << bookID
                    << "' not found. Please try again." << endl;
            }
            else {
                bookValid = true;
            }
        }

        if (cancelled) {
            cout << "\nOperation cancelled." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pauseScreen();
            return;
        }

        // ===== Find Borrowing Record =====
        bool found = false;
        bool recordUpdated = false;
        string today = getCurrentDate();

        for (size_t i = 0; i < borrowRecords.size(); i++) {
            if (borrowRecords[i].memberID == memberID &&
                borrowRecords[i].bookID == bookID &&
                (borrowRecords[i].status == "Borrowed" ||
                    borrowRecords[i].status == "Overdue" ||
                    borrowRecords[i].status == "Renewed")) {

                found = true;
                int daysDifference = daysBetweenDates(borrowRecords[i].dueDate, today);

                // ===== OVERDUE =====
                if (daysDifference > 0) {
                    if (applyFineRules(borrowRecords[i], today)) {
                        recordUpdated = true;
                    }
                    if (borrowRecords[i].status != "Overdue" && borrowRecords[i].paymentStatus != "Paid") {
                        borrowRecords[i].status = "Overdue";
                        recordUpdated = true;
                    }

                    cout << "\n---------------------------------------------\n";
                    cout << "            Overdue Fine Details" << endl;
                    cout << "---------------------------------------------\n";
                    cout << "Member ID      : " << memberID << endl;
                    cout << "Book ID        : " << bookID << endl;
                    cout << "Due Date       : " << borrowRecords[i].dueDate << endl;
                    cout << "Today's Date   : " << today << endl;
                    cout << "Days Overdue   : " << daysDifference << " days" << endl;
                    cout << "Fine Amount    : RM " << fixed << setprecision(2)
                        << borrowRecords[i].fineAmount << endl;
                    cout << "Amount Paid    : RM " << fixed << setprecision(2)
                        << borrowRecords[i].amountPaid << endl;
                    cout << "Payment Status : " << borrowRecords[i].paymentStatus << endl;
                    cout << "Status         : " << borrowRecords[i].status << endl;
                    cout << "-------------------------------------------" << endl;
                }
                // ===== NOT OVERDUE =====
                else {
                    int daysRemaining = -daysDifference;

                    cout << "\n---------------------------------------------\n";
                    cout << "              Book Not Overdue" << endl;
                    cout << "---------------------------------------------\n";
                    cout << "Member ID      : " << memberID << endl;
                    cout << "Book ID        : " << bookID << endl;
                    cout << "Due Date       : " << borrowRecords[i].dueDate << endl;
                    cout << "Today's Date   : " << today << endl;
                    cout << "Days remaining : " << daysRemaining << " days" << endl;
                    cout << "Fine Amount    : RM 0.00" << endl;
                    cout << "Status         : " << borrowRecords[i].status << endl;
                    cout << "-------------------------------------------" << endl;

                    if (borrowRecords[i].status == "Overdue") {
                        borrowRecords[i].status = "Borrowed";
                        if (borrowRecords[i].paymentStatus != "Paid") {
                            borrowRecords[i].fineAmount = 0.0;
                            borrowRecords[i].paymentStatus = "N/A";
                            borrowRecords[i].amountPaid = 0.0;
                        }
                        recordUpdated = true;
                    }
                }
                break;
            }
        }

        // ===== No Record Found =====
        if (!found) {
            cout << "\n---------------------------------------------\n";
            cout << "         No Active Borrowing Record" << endl;
            cout << "---------------------------------------------\n";
            cout << "Member ID      : " << memberID << endl;
            cout << "Book ID        : " << bookID << endl;
            cout << "Status         : No active borrowing found" << endl;
            cout << "---------------------------------------------\n";
            cout << "\nPossible reasons:" << endl;
            cout << "1. This book is not currently borrowed by this member" << endl;
            cout << "2. The book has already been returned" << endl;
            cout << "3. The member ID or book ID is incorrect" << endl;
        }

        // ===== Save Changes =====
        if (recordUpdated) {
            saveBorrowRecords(borrowRecords);
            cout << "\nRecords updated successfully." << endl;
        }
        else {
            cout << "\nNo changes needed." << endl;
        }

        // ===== Continue / Return (loop, not recursion) =====
        cout << "\nPress '1' to calculate another fine, or '0' to return: ";
        int choice;

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice." << endl;
            keepGoing = false;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            keepGoing = (choice == 1);
        }
    }

    pauseScreen();
}

void payFine() {
    clearScreen();

    loadMembers();
    loadBorrowRecords(borrowRecords);

    cout << "\n=============================================\n";
    cout << "              PAY OVERDUE FINE" << endl;
    cout << "=============================================\n";

    string memberID;

    cout << "Enter Member ID (or '0' to return): ";
    cin >> memberID;
    cin.ignore();

    if (memberID == "0") {
        cout << "\nOperation cancelled." << endl;
        pauseScreen();
        return;
    }

    memberID = toUpperCase(memberID);

    bool memberExists = false;
    string memberName = "Unknown";
    for (const auto& member : members) {
        if (member.memberID == memberID) {
            memberExists = true;
            memberName = member.memberName;
            break;
        }
    }

    if (!memberExists) {
        cout << "\nMember ID '" << memberID << "' not found." << endl;
        cout << "Please enter a valid Member ID." << endl;
        pauseScreen();
        return;
    }

    bool isActive = false;
    for (const auto& member : members) {
        if (member.memberID == memberID && member.membershipStatus == "Active") {
            isActive = true;
            break;
        }
    }

    if (!isActive) {
        cout << "\nMember account is not active." << endl;
        cout << "Please contact the administrator." << endl;
        pauseScreen();
        return;
    }

    // Refresh fines: safe to call repeatedly now that applyFineRules()
    // subtracts amountPaid instead of resetting fineAmount to the full total.
    string today = getCurrentDate();
    for (auto& record : borrowRecords) {
        if (record.memberID == memberID &&
            record.status != "Returned" &&
            record.paymentStatus != "Paid") {
            applyFineRules(record, today);
        }
    }
    saveBorrowRecords(borrowRecords);

    double totalFine = 0.0;
    bool hasUnpaid = false;

    cout << "\n---------------------------------------------" << endl;
    cout << "     Outstanding Fines for: " << memberName << " (" << memberID << ")" << endl;
    cout << "---------------------------------------------" << endl;

    cout << left
        << setw(12) << "Book ID"
        << setw(15) << "Due Date"
        << setw(10) << "Status"
        << setw(12) << "Fine (RM)"
        << endl;
    cout << "---------------------------------------------" << endl;

    for (size_t i = 0; i < borrowRecords.size(); i++) {
        if (borrowRecords[i].memberID == memberID &&
            borrowRecords[i].paymentStatus == "Unpaid" &&
            borrowRecords[i].fineAmount > FINE_EPSILON) {

            hasUnpaid = true;
            totalFine += borrowRecords[i].fineAmount;

            cout << left
                << setw(12) << borrowRecords[i].bookID
                << setw(15) << borrowRecords[i].dueDate
                << setw(10) << borrowRecords[i].status
                << setw(12) << fixed << setprecision(2) << borrowRecords[i].fineAmount
                << endl;
        }
    }

    cout << "---------------------------------------------" << endl;
    cout << "TOTAL OUTSTANDING FINE:           RM " << fixed << setprecision(2) << totalFine << endl;
    cout << "---------------------------------------------" << endl;

    if (!hasUnpaid) {
        cout << "\nNo outstanding fines for this member." << endl;
        pauseScreen();
        return;
    }

    cout << "\nDo you want to make a payment? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();

    if (toupper(confirm) != 'Y') {
        cout << "\nPayment cancelled." << endl;
        pauseScreen();
        return;
    }

    double paymentAmount;
    cout << "\nEnter payment amount (RM): ";
    cin >> paymentAmount;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInvalid payment amount." << endl;
        cout << "Please enter a valid number." << endl;
        pauseScreen();
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (paymentAmount <= 0) {
        cout << "Invalid payment amount." << endl;
        cout << "Amount must be greater than 0." << endl;
        pauseScreen();
        return;
    }

    if (paymentAmount > totalFine + FINE_EPSILON) {
        cout << "Payment amount exceeds total fine." << endl;
        cout << "Please enter RM " << fixed << setprecision(2) << totalFine << " or less." << endl;
        pauseScreen();
        return;
    }

    // FIXED: credit amountPaid so the reduction survives future recalculations.
    double remaining = paymentAmount;

    for (size_t i = 0; i < borrowRecords.size() && remaining > FINE_EPSILON; i++) {
        if (borrowRecords[i].memberID == memberID &&
            borrowRecords[i].paymentStatus == "Unpaid" &&
            borrowRecords[i].fineAmount > FINE_EPSILON) {

            double currentFine = borrowRecords[i].fineAmount;

            if (remaining + FINE_EPSILON >= currentFine) {
                borrowRecords[i].amountPaid += currentFine;
                borrowRecords[i].fineAmount = 0.0;
                borrowRecords[i].paymentStatus = "Paid";
                remaining -= currentFine;
            }
            else {
                borrowRecords[i].amountPaid += remaining;
                borrowRecords[i].fineAmount = currentFine - remaining;
                remaining = 0.0;
            }
        }
    }

    double remainingFine = 0.0;
    for (const auto& record : borrowRecords) {
        if (record.memberID == memberID &&
            record.paymentStatus == "Unpaid" &&
            record.fineAmount > FINE_EPSILON) {
            remainingFine += record.fineAmount;
        }
    }

    cout << "\n-------------------------------------------" << endl;
    cout << "           Payment Successfully!" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Member ID      : " << memberID << endl;
    cout << "Member Name    : " << memberName << endl;
    cout << "Payment Amount : RM " << fixed << setprecision(2) << paymentAmount << endl;

    if (remainingFine > FINE_EPSILON) {
        cout << "Remaining Fine : RM " << fixed << setprecision(2) << remainingFine << endl;
        cout << "Status         : Partial payment - " << fixed << setprecision(2)
            << remainingFine << " remaining" << endl;
    }
    else {
        cout << "Remaining Fine : RM 0.00" << endl;
        cout << "Status         : All fines cleared!" << endl;
    }
    cout << "-------------------------------------------" << endl;

    saveBorrowRecords(borrowRecords);
    pauseScreen();
}

void payFine(const string& memberID) {
    clearScreen();

    loadMembers();
    loadBorrowRecords(borrowRecords);

    cout << "\n=============================================\n";
    cout << "              PAY MY OVERDUE FINE\n";
    cout << "=============================================\n";

    bool memberExists = false;
    string memberName = "Unknown";
    for (const auto& member : members) {
        if (member.memberID == memberID) {
            memberExists = true;
            memberName = member.memberName;
            break;
        }
    }

    if (!memberExists) {
        cout << "\nMember record not found. Please contact administrator.\n";
        pauseScreen();
        return;
    }

    bool isActive = false;
    for (const auto& member : members) {
        if (member.memberID == memberID && member.membershipStatus == "Active") {
            isActive = true;
            break;
        }
    }

    if (!isActive) {
        cout << "\nYour account is not active. Please contact administrator.\n";
        pauseScreen();
        return;
    }

    // Refresh fines: safe to call repeatedly now that applyFineRules()
    // subtracts amountPaid instead of resetting fineAmount to the full total.
    string today = getCurrentDate();
    for (auto& record : borrowRecords) {
        if (record.memberID == memberID &&
            record.status != "Returned" &&
            record.paymentStatus != "Paid") {
            applyFineRules(record, today);
        }
    }
    saveBorrowRecords(borrowRecords);

    double totalFine = 0.0;
    bool hasUnpaid = false;

    cout << "\n---------------------------------------------" << endl;
    cout << "         Your Outstanding Fines" << endl;
    cout << "---------------------------------------------" << endl;

    cout << left
        << setw(12) << "Book ID"
        << setw(15) << "Due Date"
        << setw(10) << "Status"
        << setw(12) << "Fine (RM)"
        << endl;
    cout << "---------------------------------------------" << endl;

    for (size_t i = 0; i < borrowRecords.size(); i++) {
        if (borrowRecords[i].memberID == memberID &&
            borrowRecords[i].paymentStatus == "Unpaid" &&
            borrowRecords[i].fineAmount > FINE_EPSILON) {

            hasUnpaid = true;
            totalFine += borrowRecords[i].fineAmount;

            cout << left
                << setw(12) << borrowRecords[i].bookID
                << setw(15) << borrowRecords[i].dueDate
                << setw(10) << borrowRecords[i].status
                << setw(12) << fixed << setprecision(2) << borrowRecords[i].fineAmount
                << endl;
        }
    }

    cout << "---------------------------------------------" << endl;
    cout << "TOTAL OUTSTANDING FINE:           RM " << fixed << setprecision(2) << totalFine << endl;
    cout << "---------------------------------------------" << endl;

    if (!hasUnpaid) {
        cout << "\nYou have no outstanding fines. Great job!\n";
        pauseScreen();
        return;
    }

    cout << "\nDo you want to make a payment? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();

    if (toupper(confirm) != 'Y') {
        cout << "\nPayment cancelled.\n";
        pauseScreen();
        return;
    }

    double paymentAmount;
    cout << "\nEnter payment amount (RM): ";
    cin >> paymentAmount;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInvalid payment amount.\n";
        cout << "Please enter a valid number.\n";
        pauseScreen();
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (paymentAmount <= 0) {
        cout << "\nInvalid payment amount. Amount must be greater than 0.\n";
        pauseScreen();
        return;
    }

    if (paymentAmount > totalFine + FINE_EPSILON) {
        cout << "\nPayment amount exceeds total fine.\n";
        cout << "Please enter RM " << fixed << setprecision(2) << totalFine << " or less.\n";
        pauseScreen();
        return;
    }

    // FIXED: credit amountPaid so the reduction survives future recalculations.
    double remaining = paymentAmount;

    for (size_t i = 0; i < borrowRecords.size() && remaining > FINE_EPSILON; i++) {
        if (borrowRecords[i].memberID == memberID &&
            borrowRecords[i].paymentStatus == "Unpaid" &&
            borrowRecords[i].fineAmount > FINE_EPSILON) {

            double currentFine = borrowRecords[i].fineAmount;

            if (remaining + FINE_EPSILON >= currentFine) {
                borrowRecords[i].amountPaid += currentFine;
                borrowRecords[i].fineAmount = 0.0;
                borrowRecords[i].paymentStatus = "Paid";
                remaining -= currentFine;
            }
            else {
                borrowRecords[i].amountPaid += remaining;
                borrowRecords[i].fineAmount = currentFine - remaining;
                remaining = 0.0;
            }
        }
    }

    double remainingFine = 0.0;
    for (const auto& record : borrowRecords) {
        if (record.memberID == memberID &&
            record.paymentStatus == "Unpaid" &&
            record.fineAmount > FINE_EPSILON) {
            remainingFine += record.fineAmount;
        }
    }

    cout << "\n=============================================\n";
    cout << "              PAYMENT RECEIPT\n";
    cout << "=============================================\n";
    cout << "Member ID      : " << memberID << endl;
    cout << "Member Name    : " << memberName << endl;
    cout << "Payment Amount : RM " << fixed << setprecision(2) << paymentAmount << endl;
    cout << "-------------------------------------------\n";

    if (remainingFine > FINE_EPSILON) {
        cout << "Remaining Fine : RM " << fixed << setprecision(2) << remainingFine << endl;
        cout << "Status         : Partial payment - RM " << fixed << setprecision(2)
            << remainingFine << " remaining" << endl;
    }
    else {
        cout << "Remaining Fine : RM 0.00" << endl;
        cout << "Status         : All fines cleared! Thank you!" << endl;
    }
    cout << "=============================================\n";

    saveBorrowRecords(borrowRecords);
    pauseScreen();
}

void displayBorrowingReport() {
    loadBorrowRecords(borrowRecords);
    loadBooksFromFile(books);

    clearScreen();
    cout << "\n===========================================" << endl;
    cout << "             BORROWING REPORT" << endl;
    cout << "===========================================" << endl;

    int activeCount = 0;
    int overdueCount = 0;
    int returnedCount = 0;

    cout << "Active Borrowings: " << endl;
    cout << string(82, '-') << endl;
    cout << left << setw(10) << " Member ID" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(14) << "Due Date" << " | "
        << setw(10) << "Status" << " | "
        << setw(10) << " Fine(RM)" << " | "
        << setw(10) << "Payment" << endl;
    cout << string(82, '-') << endl;

    for (const auto& record : borrowRecords) {
        if (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed") {
            activeCount++;
            if (record.status == "Overdue") overdueCount++;

            cout << left << "   " << setw(7) << record.memberID << " | "
                << "   " << setw(7) << record.bookID << " | "
                << setw(14) << record.dueDate << " | "
                << setw(10) << record.status << " | "
                << right << setw(6) << fixed << setprecision(2) << record.fineAmount << "     | "
                << left << setw(10) << record.paymentStatus << endl;
        }
    }
    cout << string(82, '-') << endl;

    if (activeCount == 0) {
        cout << "No active borrowings." << endl;
    }

    cout << "\nReturned Items:" << endl;
    cout << string(68, '-') << endl;
    cout << left << setw(10) << " Member ID" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(14) << "Return Date" << " | "
        << setw(10) << " Fine(RM)" << " | "
        << setw(10) << "Payment" << endl;
    cout << string(68, '-') << endl;

    for (const auto& record : borrowRecords) {
        if (record.status == "Returned") {
            returnedCount++;
            cout << left << "   " << setw(7) << record.memberID << " | "
                << "   " << setw(7) << record.bookID << " | "
                << setw(14) << record.returnDate << " | "
                << right << setw(6) << fixed << setprecision(2) << record.fineAmount << "     | "
                << left << setw(10) << record.paymentStatus << endl;
        }
    }
    cout << string(68, '-') << endl;

    if (returnedCount == 0) {
        cout << "No returned items." << endl;
    }

    cout << "\n-------------------------------------------" << endl;
    cout << "              Report Summary" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Total Active Borrowings : " << activeCount << endl;
    cout << "  - Overdue Items       : " << overdueCount << endl;
    cout << "Total Returned Items    : " << returnedCount << endl;
    cout << "Total Transactions      : " << borrowRecords.size() << endl;
    cout << "-------------------------------------------" << endl;
    pauseScreen();
}

void displayPopularBooksReport() {
    loadBorrowRecords(borrowRecords);
    loadBooksFromFile(books);

    clearScreen();
    cout << "\n===========================================" << endl;
    cout << "           POPULAR BOOKS REPORT" << endl;
    cout << "===========================================" << endl;

    vector<pair<string, int>> borrowCount;

    for (const auto& record : borrowRecords) {
        bool found = false;
        for (auto& p : borrowCount) {
            if (p.first == record.bookID) {
                p.second++;
                found = true;
                break;
            }
        }
        if (!found) {
            borrowCount.push_back({ record.bookID, 1 });
        }
    }

    for (size_t i = 0; i < borrowCount.size(); i++) {
        for (size_t j = i + 1; j < borrowCount.size(); j++) {
            if (borrowCount[i].second < borrowCount[j].second) {
                swap(borrowCount[i], borrowCount[j]);
            }
        }
    }

    cout << "Top Books by Borrowing Frequency: " << endl;
    cout << string(88, '-') << endl;
    cout << left << setw(5) << " Rank" << " | "
        << setw(10) << "  Book ID" << " | "
        << setw(30) << "Title" << " | "
        << setw(20) << "Category" << " | "
        << setw(10) << "Borrows" << endl;
    cout << string(88, '-') << endl;

    int rank = 1;
    int displayCount = min(10, static_cast<int>(borrowCount.size()));

    for (int i = 0; i < displayCount; i++) {
        string title = "N/A";
        string category = "N/A";

        for (const auto& book : books) {
            if (book.bookID == borrowCount[i].first) {
                title = book.bookTitle;
                category = book.category;
                break;
            }
        }

        cout << left << "  " << setw(3) << rank << " | "
            << "   " << setw(7) << borrowCount[i].first << " | "
            << setw(30) << (title.length() > 28 ? title.substr(0, 28) + "..." : title) << " | "
            << setw(20) << category << " | "
            << right << setw(5) << borrowCount[i].second << endl;
        rank++;
    }

    if (borrowCount.empty()) {
        cout << "No borrowing records found." << endl;
    }

    cout << string(88, '-') << endl;
    cout << "Total Books Borrowed: " << borrowCount.size() << endl;
    cout << "Total Borrows       : " << borrowRecords.size() << endl;
    pauseScreen();
}

void displayLibraryStatistics() {
    loadBorrowRecords(borrowRecords);
    loadBooksFromFile(books);
    loadMembers();

    clearScreen();
    cout << "\n===========================================" << endl;
    cout << "            LIBRARY STATISTICS" << endl;
    cout << "===========================================" << endl;

    int totalBooks = static_cast<int>(books.size());
    int totalAvailable = 0;
    int totalCopies = 0;

    for (const auto& book : books) {
        totalAvailable += book.availableCopies;
        totalCopies += book.quantity;
    }
    int totalBorrowedBooks = totalCopies - totalAvailable;

    cout << "Book Statistics:" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Total Book Titles     : " << totalBooks << endl;
    cout << "Total Copies          : " << totalCopies << endl;
    cout << "Available Copies      : " << totalAvailable << endl;
    cout << "Borrowed Copies       : " << totalBorrowedBooks << endl;

    if (totalCopies > 0) {
        double availability = (static_cast<double>(totalAvailable) / totalCopies) * 100;
        cout << "Availability Rate     : " << fixed << setprecision(1) << availability << "%" << endl;
    }

    int totalMembers = static_cast<int>(members.size());
    int activeMembers = 0;

    for (const auto& member : members) {
        if (member.membershipStatus == "Active") {
            activeMembers++;
        }
    }

    cout << "\nMember Statistics:" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Total Members         : " << totalMembers << endl;
    cout << "Active Members        : " << activeMembers << endl;
    cout << "Inactive Members      : " << (totalMembers - activeMembers) << endl;

    int activeBorrowings = 0;
    int overdueItems = 0;
    int returnedItems = 0;
    double totalFineCollected = 0.0;
    double totalFineOutstanding = 0.0;

    for (const auto& record : borrowRecords) {
        if (record.status == "Borrowed" || record.status == "Overdue" || record.status == "Renewed") {
            activeBorrowings++;
            if (record.status == "Overdue") overdueItems++;
        }
        else if (record.status == "Returned") {
            returnedItems++;
        }

        // Total collected across ALL records is now amountPaid (this
        // correctly includes money collected via partial payments too),
        // and outstanding is whatever fineAmount still remains unpaid.
        totalFineCollected += record.amountPaid;

        if (record.paymentStatus == "Unpaid" && record.fineAmount > FINE_EPSILON) {
            totalFineOutstanding += record.fineAmount;
        }
    }

    cout << "\nBorrow Statistics:" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Active Borrowings     : " << activeBorrowings << endl;
    cout << "Overdue Items         : " << overdueItems << endl;
    cout << "Returned Items        : " << returnedItems << endl;
    cout << "Total Transactions    : " << borrowRecords.size() << endl;

    cout << "\nFine Statistics:" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Total Fine Collected  : RM " << fixed << setprecision(2) << totalFineCollected << endl;
    cout << "Outstanding Fine      : RM " << fixed << setprecision(2) << totalFineOutstanding << endl;
    cout << "Total Fine Amount     : RM " << fixed << setprecision(2)
        << (totalFineCollected + totalFineOutstanding) << endl;

    cout << "\n===========================================" << endl;
    cout << "             END OF STATISTICS" << endl;
    cout << "===========================================" << endl;
    pauseScreen();
}

void reportingAndFineMenu() {
    int choice;

    do {
        clearScreen();
        cout << "\n=============================================" << endl;
        cout << "         REPORTING & FINE MANAGEMENT" << endl;
        cout << "=============================================" << endl;
        cout << "1. Calculate Overdue Fine" << endl;
        cout << "2. Pay Fine" << endl;
        cout << "3. Display Borrowing Report" << endl;
        cout << "4. Display Popular Books Report" << endl;
        cout << "5. Display Library Statistics" << endl;
        cout << "0. Back" << endl;
        cout << "=============================================" << endl;

        choice = getValidatedChoice(0, 5);
        cin.ignore();

        switch (choice) {
        case 1:
            calculateOverdueFine();
            break;
        case 2:
            payFine();
            break;
        case 3:
            displayBorrowingReport();
            break;
        case 4:
            displayPopularBooksReport();
            break;
        case 5:
            displayLibraryStatistics();
            break;
        case 0:
            cout << "\nReturning to Main Menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);
}