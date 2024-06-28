/*
Project:Book/Library/Hold thing
Author: Alec Gilbertsen
Class: The database one
Date: Somewhere in august I think
Specifications:ISBN UNIQUE, LIBRARY NAME IS UNIQUE, ALSO IF THEY DO AN IMPROPER HOLD
... USER MIGHT BE STUPID!!!LET THEM KNOW THEY DID BAD*/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <vector>  


using namespace std;

//prototypes
void bookBuilder();
void libBuilder();
void holdBuilder();
void bookAdder();
void libAdder();
void holdAdder();
void lister(string lister);
void bookLister();
void libLister();
void finder(int findThis);


//some structs for all the things in this project
//books only have an isbn, publish date, and a name, they are below
struct book{
    int isbn;
    int pubDate;
    string name;
};

//library only have a name, and some other garbage that doesn't matter too much the name is the unique part
struct library{
    string name;
    string garbage;

};

//holding have a location an isbn and a copy number that are important
struct holding{
    string location;
    int isbn;
    string copyNum;
};

//this holds books the isbn is the key value
unordered_map<int , book> bookTable;

//this holds libraries the name is the key value because its unique
unordered_map<string, library> libraryTable;

//this thing is an amlgamation!!!
//the first key value is the isbn, this lets us know what libraries
//the seconnd key value is the name of the library this way we can see how many copies
//are in one library
unordered_map<int, unordered_map<string, vector<holding>>> holdTable;

//prints a holding
ostream& operator<<(std::ostream& os, const holding& h) 
{
    os << h.location << " " << h.isbn << " " << h.copyNum << "\n";
    return os;
}

//helps print the abomination that is the unorder_map within the hold table
ostream& operator<<(std::ostream& os, const unordered_map<string, vector<holding>> h)
{
    for (auto const &pair : h)
    {
        vector<holding> temp = pair.second;
        for(auto const &abominationHelper : temp)
        {
            cout << abominationHelper;
        }
    }
    return os;
}

//helps print the abomination that is the hold table won't be used but nice to have for debugging
ostream& operator<<(std::ostream& os, const unordered_map<int, unordered_map<string, vector<holding>>> h)
{
    for (auto const &pair : h)
    {
        cout << pair.second;
    }
    return os;
}

//fills the lib table
void libBuilder()
{
    //safety first!
    libraryTable.clear();
    string libName; string garbage;
    ifstream libFile;
    libFile.open("libraries.txt");
    while(libFile)
    {
        //makes a library object from the file info, does this until the file is empty
        library temp2;
        libFile >> libName;
        getline(libFile,garbage);
        temp2.name = libName;
        temp2.garbage = garbage;
        libraryTable.insert({temp2.name, temp2});
    }
    libFile.close();
    return;
}

//fills the book table
void bookBuilder()
{
    //safety first!
    bookTable.clear();
    int isbn;int year;string bookInfo;
    ifstream bookFile;
    bookFile.open("books.txt");
    while(bookFile)
    {
        //makes a book object until the file is empty
        book temp;
        bookFile >> isbn;
        temp.isbn = isbn;
        bookFile >> year;
        temp.pubDate = year;
        getline(bookFile, bookInfo);
        temp.name = bookInfo.substr(1, bookInfo.length()-1);
        bookTable.insert({temp.isbn, temp});
    }
    bookFile.close();
    return;
}

//fills the hold table
void holdBuilder()
{
    //safety first!
    holdTable.clear();
    ifstream holdFile;
    holdFile.open("holdings.txt");
    while(holdFile)
    {
        //grab the top hold and assign all of its values
        int isbn; string location; string copyNum;
        holding holder;
        holdFile >> location; holder.location = location;
        holdFile >> isbn; holder.isbn = isbn;
        getline(holdFile, copyNum); holder.copyNum = copyNum;
        //this isbn has not been added yet
        if(holdTable.find(isbn) == holdTable.end())
        {
            //make a new map to hold the new location and vector of holdings
            unordered_map<string, vector<holding>> temp;
            vector<holding> temp2;
            //push our first value on to it
            temp2.push_back(holder);
            temp.insert({holder.location, temp2});
            //add our new map to the holdtable
            holdTable.insert({holder.isbn, temp});
        }
        //isbn is added already
        else
        {
            auto finder = holdTable.find(isbn);
            //location hasn't been added yet
            if(finder->second.find(location) == finder->second.end())
            {
                // in this case the location hasnt been added yet so we make a new vector to store
                //the holdings and than add it to the table with the location
                vector<holding> temp2;
                temp2.push_back(holder);
                holdTable.find(isbn)->second.insert({location, temp2});
            }
            //location has been found
            else
            {
                //this case the only thing that needs to happen is that the holding is added
                finder->second.find(location)->second.push_back(holder);   
            }
        }
    }
    return;
}

//makes unordered maps out of libraries, books, and holdings this makes finding dupes easy
void map_quest()
{
    bookBuilder();
    libBuilder();
    holdBuilder();
    return;

}

//function that handles any of the commands under add
void add(string prompt)
{
    switch(prompt[0])
    {
        //adding a book
        case('b'):
            //rebuild the map before the add for no dupes
            bookBuilder();
            bookAdder();
            break;
        //adding a library
        case('l'):
            //rebuild the map before the add for no dupes
            libBuilder();
            libAdder();
            break;
        //adding a holding
        case('h'):
            holdAdder();
            //holdes already handle duplicates this rebuild is for posterity
            holdBuilder();
            break;
    }
    return; 
}

//function that handles adding a book to the db
void bookAdder()
{
    //get the variables that make a book from cin
    int isbn; int year; string name; string bName;
    cin >> isbn;
    cin >> year;
    getline(cin, name);
    name = name.substr(1, name.length()-1);
    //make sure this book doesn't exist than add it to the file if it doesn't else tell the user they
    //made an oopsie
    if(bookTable.find(isbn) == bookTable.end())
    {
        book temp;
        temp.isbn = isbn; temp.pubDate = year; temp.name = name;
        bName += to_string(isbn);
        bName += " ";
        bName += to_string(year);
        bName += " ";
        bName += name;
        bookTable.insert({isbn, temp});
        //adds a newline character to a book
        bName += "\n";
        //adds book to a file by opening books.txt and outputting bName(the book string) in there
        ofstream bookFile;
        bookFile.open("books.txt",ios_base::app);
        bookFile << bName;
        //don't forget to close or things might get wacky!
        bookFile.close();
        //clear and rebuild the map
        return; 
    }
    else
    {
        cout << "ISBN is a duplicate please try again!" << endl;;
        return;
    }
}
//function that handles adding a library to the db
void libAdder()
{
    //make variables for all the libraries variables could just use object.variable but I think
    //this is more readable
    string name; string garbage; string libName;
    //assign said variables
    cin >> name;
    getline(cin, garbage);
    garbage = garbage.substr(1, garbage.length()-1);
    //if the library wasn't found already insert it
    if(libraryTable.find(name) == libraryTable.end())
    {
        //assign all the variables to the object
        library temp;
        temp.name = name; temp.garbage = garbage;
        libName += name;
        libName += " ";
        libName += garbage;
        //adds a newline character to a library so the file is formatted pretty
        libName += "\n";
        //adds library to a file by opening libraries.txt and outputting bName(the book string) in there
        ofstream libFile;
        libFile.open("libraries.txt",ios_base::app);
        libFile << libName;
        //don't forget to close or things might get wacky!
        libFile.close(); 
        //clear and rebuild the map
        return; 
    }
    //tell the user they made an oopsie
    else
    {
        cout << "This Library already exists try again" << endl;
        return;
    }
}

//function that finds the ispn of a book and adds a hold
void holdAdder()
{
    //build whatever we have
    holdTable.clear();
    holdBuilder();
    //check if the isbn is in the database
    int isbn;
    cin >> isbn;
    string library;
    cin >> library;    
    //make sure book table is update
    bookBuilder();
    if(bookTable.find(isbn) == bookTable.end())
    {
        cout << "ISBN not found" << endl;
        return;
    }
    else
    {
        //check if the book is in the database
        //make sure libTable is updated
        libBuilder();
        if(libraryTable.find(library) == libraryTable.end())
        {
            cout << "Library not found" << endl;
            return;
        }
    }
    //this book and library exist we can add a hold
    //there isn't a hold for this book at all
    if(holdTable.find(isbn) == holdTable.end())
    {
        //find the book in the table we will use the info to build this hold
        auto booked = bookTable.find(isbn)->second;
        holding holder;
        holder.isbn = isbn;
        holder.location = library;
        holder.copyNum = to_string(booked.pubDate);
        holder.copyNum += " ";
        holder.copyNum += booked.name;
        holder.copyNum += " Copy Number: 1";
        //make the unordered map that will be for this location and make and add the first value
        //to its vector
        unordered_map<string, vector<holding>> temp;
        vector<holding> temp2;
        temp2.push_back(holder);
        //add the location, vector combo to the temp map
        //then add that map into the holdTable
        temp.insert({holder.location, temp2});
        holdTable.insert({holder.isbn, temp});
        //open the file and add the new hold using the overloaded <<
        ofstream holdFile;
        holdFile.open("holdings.txt",ios_base::app);
        holdFile << holder;
        holdFile.close();
        //new hold clear the map and rebuild
        holdTable.clear();
        holdBuilder();
        return;
    }
    //there is a hold for this book somewhere
    else
    {
        auto temp = holdTable.find(isbn);
        //this library has a copy
        if(temp->second.find(library) != temp->second.end())
        {
            //once again the book will be used so find it and fill out this holds data
            auto booked = bookTable.find(isbn)->second;
            holding holder;
            holder.isbn = isbn;
            holder.location = library;
            holder.copyNum = to_string(booked.pubDate);
            holder.copyNum += " ";
            holder.copyNum += booked.name;
            //this is how we get correct copy numbers find the vector
            auto temp2 = temp->second.find(library);
            //grab the size of the vector and add 1 to it and boom book numberse
            int copyNum = temp2->second.size()+1;
            holder.copyNum = holder.copyNum + " " + "Copy Number: " + to_string(copyNum);
            //send that out to the file with overloaded <<
            ofstream holdFile;
            holdFile.open("holdings.txt",ios_base::app);
            holdFile << holder;
            holdFile.close();
            //new hold clear the map and rebuild
            holdTable.clear();
            holdBuilder();
            return;
        }
        //hold exists but library doesn't have a copy
        else
        {
            //once again find the book related to the isbn and fill out the data
            auto booked = bookTable.find(isbn)->second;
            holding holder;
            holder.isbn = isbn;
            holder.location = library;
            holder.copyNum = to_string(booked.pubDate);
            holder.copyNum += " ";
            holder.copyNum += booked.name;
            //we know this is copy one
            holder.copyNum += " Copy Number: 1";
            //make a vector so we can add this new location
            vector<holding> tempVec;
            tempVec.push_back(holder);
            //add this to the hold table, table for this isbn
            holdTable.find(isbn)->second.insert({library, tempVec});
            ofstream holdFile;
            holdFile.open("holdings.txt",ios_base::app);
            //prints the holding into the holdfile
            holdFile << holder;
            holdFile.close();
            holdTable.clear();
            holdBuilder();
            return;
        }
    }
    return;
}

//function that handles listing books or libraries based on command
void lister(string prompt)
{
    switch(prompt[0])
    {
        //adding a book
        case('b'):
            bookLister();
            break;
        //adding a library
        case('l'):
            libLister();
            break;
    }
    return;
}

//function that lists the book file
void bookLister()
{
    //not allowed to use system(), so I just read the whole file and outputted it to standard output
    ifstream bookFile("books.txt");
    if (bookFile.is_open())
        cout << bookFile.rdbuf();
    bookFile.close();
    cout << endl;
    return; 
}

//function that lists the library file
void libLister()
{
    //not allowed to use system(), so I just read the whole file and outputted it to standard output
    ifstream libFile("libraries.txt");
    if (libFile.is_open())
        cout << libFile.rdbuf();
    libFile.close();
    cout << endl;
    return; 
}

//function that finds something
void finder(int findThis)
{ 
    //make sure we have an up to date map of the holds
    holdTable.clear();
    holdBuilder();
    //isbn hasn't been added yet
    if(holdTable.find(findThis) == holdTable.end())
    {
        cout << "Isbn was not found" << endl;
        return;
    }
    //isbn is in the database, print out the map associated with that part of the database
    else
    {
        auto found = holdTable.find(findThis);
        cout << found->second;
    }
    return;   
}

int main(int argc, char *argv[])
{
    /*main loop takes in a prompt for the user 
    and handles it with a switch statement*/
    string prompt;
    int isbn;
    map_quest();
    while(prompt[0] != 'q')
    {
        prompt = " ";
        cout << ">>> ";
        cin >> prompt;
        switch(prompt[0])
        {
            case('a'):
                cin >> prompt;
                add(prompt);
                map_quest();
                break;
            case('l'):
                cin >> prompt;
                lister(prompt);
                map_quest();
                break;
            case('f'):
                cin >> isbn;
                finder(isbn);
                break;
            case('q'):
                break;
        }
    }
    return 0;
}