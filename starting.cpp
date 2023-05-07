#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <regex>
using namespace std;

/*
Database 1 has only internal_code and name
Database 2 has internal_code, quantity, rack_no, price, expiry
Database 3 has internal_code, composition1, composition2, age
Database 4 has internal_code, side_effects
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
			cout << "Enter exxpiry date (YYYY-MM-DD): ";
			getline(cin, aR_expiry);
			aR_expiry = whiteSpaceRemover(aR_expiry, 1);
			// insert data into database
		}
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	addRecords();
	return 0;
}