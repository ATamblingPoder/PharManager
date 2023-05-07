#include <iostream>
#include <map>
#include <vector>
#include <string>

// Below ones might not be needed if ncurses.h works
#include <regex>
#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
using namespace std;

void SetStdinEcho(bool enable = true){
	#ifdef WIN32
	    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
	    DWORD mode;
	    GetConsoleMode(hStdin, &mode);

	    if( !enable )
	        mode &= ~ENABLE_ECHO_INPUT;
	    else
	        mode |= ENABLE_ECHO_INPUT;

	    SetConsoleMode(hStdin, mode );

	#else
	    struct termios tty;
	    tcgetattr(STDIN_FILENO, &tty);
	    if( !enable )
	        tty.c_lflag &= ~ECHO;
	    else
	        tty.c_lflag |= ECHO;

	    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
	#endif
}

/*
Database 1 has only internal_code and name
Database 2 has internal_code, quantity, rack_no, price, expiry
Database 3 has internal_code, composition1, composition2, age
Database 4 has internal_code, side_effects

CREATE TABLE internal_data(internal_code varchar(10), name varchar(255));
CREATE TABLE main_data(internal_code varchar(10), quantity int, rack_no int, price decimal(5,2), expiry date);
CREATE TABLE composition_data(internal_code varchar(10), composition1 blob, composition2 blob, age int);
CREATE TABLE side_effects(internal_code varchar(10), side_effects blob);
CREATE TABLE alternatives(internal_code varchar(10), internal_code2 varchar(10));

*/



string whiteSpaceRemover(string to_clean, int how_many = 0){
	// how_many = 1 for all
	if(how_many){
		to_clean = regex_replace(to_clean, regex("^ +| +$|( ) +"), "$1");
		to_clean = regex_replace(to_clean, regex(" "), "");
		return to_clean;
	}
	to_clean = regex_replace(to_clean, std::regex("^ +"), "");
	to_clean = regex_replace(to_clean, std::regex(" +$"), "");
	return to_clean;
}

int checkRecordsIfExists(string to_check){
	// code to check 
	// return 0 if does not exist
	// return 1 if exists
	return 0;
}

int addRecords(){ // This function adds records to database
	int aR_number;
	cout << "Number of records to add: ";
	cin >> aR_number;
	cin.ignore();
	for(int aR_i = 0; aR_i < aR_number; aR_i++){
		cout << "Enter internal code of medicine: ";
		string aR_internal_code;
		getline(cin, aR_internal_code);
		aR_internal_code = whiteSpaceRemover(aR_internal_code, 1);
		if(!checkRecordsIfExists(aR_internal_code)){
			string aR_name, aR_expiry;
			int aR_quantity, aR_rack;
			float aR_price;
			cout << "Enter name of medicine: ";
			getline(cin, aR_name);
			aR_name = whiteSpaceRemover(aR_name);
			cout << "Enter price: ";
			cin >> aR_price;
			cin.ignore();
			cout << "Enter quantity: ";
			cin >> aR_quantity;
			cin.ignore();
			cout << "Enter rack number: ";
			cin >> aR_rack;
			cin.ignore();
			cout << "Enter expiry date (YYYY-MM-DD): ";
			getline(cin, aR_expiry);
			aR_expiry = whiteSpaceRemover(aR_expiry, 1);
			// insert data into database
		}
	}
	return 0;
}

class Transaction{
	public:
		map<string, int> T_current_list;
		string T_discount;
		Transaction(){
		}
		int addItem(string T_internal_code, int T_quantity){ // only call if item is not in list already
			pair<map<string,int>::iterator, bool> T_return_insert;
			T_return_insert = T_current_list.insert(pair<string, int>(T_internal_code, T_quantity));
			if(T_return_insert.second == false)
				return 1;
			return 0;
		}
		void discount(string T_discount){
			if(!T_discount.empty()){
				// check if it exists and add it
			}
			else{
				// print wherever that discount has already been applied
			}
		}
};

int loginFunction(){ // checks for password and returns 1 for correct and 0 for false
	// code
	return 1;
}

int main(int argc, char const *argv[])
{
	if(loginFunction()){
	}
	return 0;
}

// menu to choose stuff
// start transaction
// search for medicine
// view current bill
// remove medicine
// apply discount code
// alternatice medicine finder 
// print bill to file
// management mode <- requires extra password
// add records to database(s)
// remove records from database(s)
// add discount code for discount percentage