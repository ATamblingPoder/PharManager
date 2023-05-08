#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <format>
#include <sqlite3.h>
// Below ones might not be needed if ncurses.h works
#include <regex>
#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
using namespace std;

sqlite3 *db;
int rc = sqlite3_open("test.db", &db);
int *temp_counter;
string password="password123";


// Copied from StackOverflow :)
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

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

/*
Database 1 has only internal_code and name
Database 2 has internal_code, quantity, rack_no, price, expiry
Database 3 has internal_code, composition1, composition2, age
Database 4 has internal_code, side_effects
Database 5 has discount_code, percentage

CREATE TABLE IF NOT EXISTS internal_data(internal_code varchar(10), name varchar(255));
CREATE TABLE IF NOT EXISTS main_data(internal_code varchar(10), quantity int, rack_no int, price decimal(5,2), expiry date);
CREATE TABLE IF NOT EXISTS composition_data(internal_code varchar(10), composition1 text, composition2 text, age int);
CREATE TABLE IF NOT EXISTS side_effects(internal_code varchar(10), side_effects text);
CREATE TABLE IF NOT EXISTS alternatives(internal_code varchar(10), internal_code2 varchar(10));
CREATE TABLE IF NOT EXISTS discounts(discount_code varchar(10), percentage int);

*/

void databasesInitializer(){
	const char * data = "Databases Initialized";
	char * zErrMsg = 0;
	string dI_query;
	dI_query = "CREATE TABLE IF NOT EXISTS internal_data(internal_code varchar(10), name varchar(255));";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
	dI_query = "CREATE TABLE IF NOT EXISTS main_data(internal_code varchar(10), quantity int, rack_no int, price decimal(5,2), expiry date);";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
	dI_query = "CREATE TABLE IF NOT EXISTS composition_data(internal_code varchar(10), composition1 text, composition2 text, age int);";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
	dI_query = "CREATE TABLE IF NOT EXISTS side_effects(internal_code varchar(10), side_effects text);";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
	dI_query = "CREATE TABLE IF NOT EXISTS alternatives(internal_code varchar(10), internal_code2 varchar(10));";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
	dI_query = "CREATE TABLE IF NOT EXISTS discounts(discount_code varchar(10), percentage int);";
	rc = sqlite3_exec(db, dI_query.c_str(), callback, (void*)data, &zErrMsg);
}

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

int checker(void *data, int argc, char **argv, char **azColName){
   int i;
   i = std::stoi(argv[0]);
   *temp_counter = i;
   return i;
}


int checkRecordsIfExists(string to_check, string table_to_check){
	*temp_counter = 0;
	string query;
	const char * data = "Checker called";
	char * zErrMsg = 0;
	query = format("SELECT COUNT(*) FROM {} WHERE internal_code='{}'", table_to_check, to_check);
	rc = sqlite3_exec(db, query.c_str(), checker, (void*)data, &zErrMsg);
	if(*temp_counter == 0)
		return 0;
	else
		return 1;
}

/*
static int updater(void *NotUsed, int argc, char **argv, char **azColName){

}
*/


// This mess of code adds records to the 
void theActualRecordAdder(string code, string name, string expiry, string comp1, string comp2, int quantity, int rack, int age, float price){
	const char * data = "Adder called";
	char * zErrMsg = 0;
	string query;
	query = format("INSERT INTO internal_data VALUES('{}', '{}');", code, name);
	rc = sqlite3_exec(db, query.c_str(), callback, (void*)data , &zErrMsg);
	query = format("INSERT INTO main_data VALUES('{}', {}, {}, {}, '{}');", code, quantity, rack, price, expiry);
	rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
	query = format("INSERT INTO composition_data VALUES('{}', '{}', '{}', {})", code, comp1, comp2, age);
	rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
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
		if(!checkRecordsIfExists(aR_internal_code, "internal_data")){
			string aR_name, aR_expiry, aR_composition1, aR_composition2;
			int aR_quantity, aR_rack, aR_age;
			float aR_price;
			cout << "Enter name of medicine: ";
			getline(cin, aR_name);
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
			cout << "Enter Minimum Age:";
			cin >> aR_age;
			cin.ignore();
			cout << "Enter Composed of (1):";
			getline(cin, aR_composition1);
			cout << "Enter Composed of (2):";
			getline(cin, aR_composition2);
			aR_name = whiteSpaceRemover(aR_name);
			aR_name = regex_replace(aR_name, regex("'"), "''");
			aR_expiry = whiteSpaceRemover(aR_expiry, 1);
			aR_composition1 = whiteSpaceRemover(aR_composition1);
			aR_composition1 = regex_replace(aR_composition1, regex("'"), "''");
			aR_composition2 = whiteSpaceRemover(aR_composition2);
			aR_composition2 = regex_replace(aR_composition2, regex("'"), "''");
			theActualRecordAdder(aR_internal_code, aR_name, aR_expiry, aR_composition1, aR_composition2, aR_quantity, aR_rack, aR_age, aR_price);
		}
		else{
			cout << "Record already exists in database(s)" << endl;
		}
	}
	return 0;
}



float temp_price = 0;
int temp_quantity = 0;
static int adder(void *data, int argc, char **argv, char **azColName){
	float temp_f;
	temp_f = stof(argv[0]);
	temp_price = temp_f;
	return 0;
}

class Transaction{
	public:
		map<string, int> T_current_list;
		string T_discount;
		float T_total_price;
		Transaction(){
			T_total_price = 0;
		}
		int addItem(string T_internal_code, int T_quantity){ // only call if item is not in list already
			const char *data = "Updater";
			char *zErrMsg = 0;
			pair<map<string,int>::iterator, bool> T_return_insert;
			int quantity_to_change = quantityChecker(T_internal_code);
			if(T_quantity > quantity_to_change)
				return 1;
			quantity_to_change -= T_quantity;
			string query;
			query = format("UPDATE main_data SET quantity={} WHERE internal_code='{}'", quantity_to_change, T_internal_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			T_return_insert = T_current_list.insert(pair<string, int>(T_internal_code, T_quantity));
			if(T_return_insert.second == false){
				cout << "Item alreay in list!" << endl;
				return 1;
			}
			return 0;
		}
		void discount(){
			if(!T_discount.empty()){
				cout << "Enter discount code: ";
				getline(cin, T_discount);
				T_discount = whiteSpaceRemover(T_discount, 1);
				if(!checkRecordsIfExists(T_discount, "discounts")){
					cout << "No such discount in the database!!";
				}
			}
			else{
				cout << "Discount has already been applied!!";
			}
		}
		static int adder(void *data, int argc, char **argv, char **azColName){
			float temp_f;
			temp_f = stof(argv[0]);
			temp_price = temp_f;
			return 0;
		}
		static int counter(void *data, int argc, char **argv, char **azColName){
			int temp_i;
			temp_i = stoi(argv[0]);
			temp_quantity = temp_i;
			return 0;
		}
		int quantityChecker(string T_internal_code){
			const char *data = "Counter called";
			char *zErrMsg = 0;
			string query;
			temp_quantity = 0;
			query = format("SELECT quantity FROM main_data WHERE internal_code='{}';", T_internal_code);
			rc = sqlite3_exec(db, query.c_str(), this->counter, (void*)data, &zErrMsg);
			return temp_quantity;
		}
		void totaler(){
			const char *data = "Adder called";
			char *zErrMsg = 0;
			string query;
			for(auto &it : T_current_list){
				query = format("SELECT price FROM main_data WHERE internal_code='{}';", it.first);
				rc = sqlite3_exec(db, query.c_str(), adder, (void*)data, &zErrMsg);
				float temp_2_price;
				temp_2_price += (temp_price) * (it.second); 
				cout << it.first << " : " << it.second << " : " << temp_2_price << endl;
				T_total_price += temp_2_price;
			}
		}
};

int loginFunction(){ // checks for password and returns 1 for correct and 0 for false
	SetStdinEcho(false);
	cout << "Enter Password(will not be echoed):";
	string inp_pass;
	getline(cin, inp_pass);
	SetStdinEcho(true);
	cout << "\033[2J\033[1;1H";
	return inp_pass.compare(password);
}

void menu(){
	int the_choice;
	cin >> the_choice;
	switch(the_choice){
		case 1:{
			Transaction trans_object;
			// transaction menu interface
		}
		case 2:{
			// alternate medicine finder
		}
		case 3:{
			// print bill
		}
		case 4:{
			// management mode
		}
	}
}

int main(int argc, char const *argv[]){
	if(loginFunction())
		return 1;
	databasesInitializer();
	temp_counter = new int;
	addRecords();
	Transaction trans1;
	trans1.addItem("A101", 5);
	trans1.addItem("C111", 9);
	trans1.totaler();
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