#include <bits/stdc++.h>
#include <ncurses.h>
#include <sqlite3.h>
// Below ones might not be needed if ncurses.h works
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
*/

string shortener(string to_short){
    if (to_short.length() <= 30)
        return to_short;
    string final_string;
    final_string = to_short.substr(0, 27);
    final_string += "...";
    return final_string;
}


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


int checkRecordsIfExists(string to_check, string what, string table_to_check){
	*temp_counter = 0;
	string query;
	const char * data = "Checker called";
	char * zErrMsg = 0;
	query = format("SELECT COUNT(*) FROM {} WHERE {}='{}'", table_to_check, what, to_check);
	rc = sqlite3_exec(db, query.c_str(), checker, (void*)data, &zErrMsg);
	if(*temp_counter == 0)
		return 0;
	else
		return 1;
}


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
		if(!checkRecordsIfExists(aR_internal_code, "internal_code","internal_data")){
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
string temp_name;
static int adder(void *data, int argc, char **argv, char **azColName){
	float temp_f;
	temp_f = stof(argv[0]);
	temp_price = temp_f;
	return 0;
}

class Transaction{
	public:
		map<string, int> T_current_list;
		string T_discount, T_name;
		char T_gender;
		float T_total_price;
		Transaction(){
			T_total_price = 0;
			cout << "Enter Patient Name: ";
			getline(cin, T_name);
			T_name = whiteSpaceRemover(T_name);
			cout << "Enter gender: ";
			cin >> T_gender;
			cin.ignore();
			T_gender = toupper(T_gender);
			if(!(T_gender == 'M' || T_gender == 'F' || T_gender == 'O')){
				cout << "Invalid gender!! Defaulting to Others.";
				T_gender = 'o';
			}
			cout << "Transaction started." << endl;
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
			query = format("UPDATE main_data SET quantity={} WHERE internal_code='{}';", quantity_to_change, T_internal_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			T_return_insert = T_current_list.insert(pair<string, int>(T_internal_code, T_quantity));
			if(T_return_insert.second == false){
				cout << "Item alreay in list!" << endl;
				return 1;
			}
			return 0;
		}
		int remover(string T_internal_code){
			int quantity_to_change;
			std::map<string, int>::iterator remove_it = T_current_list.find(T_internal_code);
			if(remove_it == T_current_list.end()){
				cout << "Item to remove not in list!!" << endl;
			}
			else{
				quantity_to_change = quantityChecker(T_internal_code) + remove_it->second;
				const char *data = "Updater";
				char *zErrMsg = 0;
				string query;
				query = format("UPDATE main_data SET quantity={} WHERE internal_code='{}';", quantity_to_change, T_internal_code);
				rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
				T_current_list.erase(remove_it);
			}
			return 0;
		}
		void discount(){
			if(!T_discount.empty()){
				cout << "Enter discount code: ";
				getline(cin, T_discount);
				T_discount = whiteSpaceRemover(T_discount, 1);
				if(!checkRecordsIfExists(T_discount, "discount_code", "discounts")){
					cout << "No such discount in the database!!";
				}
			}
			else{
				cout << "Discount has already been applied!!";
			}
		}
		static int adder(void *data, int argc, char **argv, char **azColName){
			float temp_f;
			temp_price = 0;
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
		static int namer(void *data, int argc, char **argv, char **azColName){
			temp_name = argv[0];
			return 0;
		}
		string nameNamer(string the_code){
			char const *data = "Lol";
			char *zErrMsg = 0;
			string qq;
			qq = format("SELECT name FROM internal_data WHERE internal_code='{}';", the_code);
			rc = sqlite3_exec(db, qq.c_str(), this->namer, (void*)data, &zErrMsg);
			return temp_name;
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
		float pricer(string code_price){
			const char *data = "Adder called";
			char *zErrMsg = 0;
			string query;
			query = format("SELECT price FROM main_data WHERE internal_code='{}';", code_price);
			rc = sqlite3_exec(db, query.c_str(), this->adder, (void*)data, &zErrMsg);
			return temp_price;
		}
		void billPrinter(){
			cout << "Aapki Apni Pharmacy :)" << endl;
		   time_t now = time(0);
    		char *dt = ctime(&now);
    		cout << format("The date is {}", dt) << endl;
    		cout << format("{}:{}", T_name, T_gender) << endl;
    		int number = T_current_list.size();
    		std::map<string, int>::iterator T_iterator = T_current_list.begin();
    		for(int i = 0; i < number; i++){
    			//float bill_temp_price = stof(textFetcher("price", "main_data", "internal_code", T_iterator->first));
    			float bill_temp_price = pricer(T_iterator->first);
    			cout << format("{} | {} | {} | {} | {}", i + 1,  nameNamer(T_iterator->first), T_iterator->second, bill_temp_price, bill_temp_price*(T_iterator->second));
    			T_iterator++;
    			cout << endl;
    		}
		}
};


std::vector<string> the_names;
static int name_adder(void *NotUsed, int argc, char **argv, char **azColName){
	the_names.push_back(argv[0]);
	return 0;
}

string textFetcher(string q1, string q2, string q3, string q4){
	the_names.clear();
	const char *data = "Fetcher Called";
	char *zErrMsg = 0;
	string query;
	query = format("SELECT {} FROM {} WHERE {}='{}';", q1, q2, q3, q4);
	rc = sqlite3_exec(db, query.c_str(), name_adder, (void*)data, &zErrMsg);
	return the_names.at(0);
}


int alternateAdder(){
	string code1, code2;
	cout << "Enter code of the medicine: ";
	getline(cin, code1);
	code1 = whiteSpaceRemover(code1, 1);
	if(checkRecordsIfExists(code1, "internal_code", "alternatives") || checkRecordsIfExists(code1, "internal_code2", "alternatives")){
		cout << "Already in database!!" << endl;
		return 1;
	}
	cout << format("Medicine with code {} is '{}'", code1, textFetcher("name", "internal_data", "internal_code", code1)) << endl;
	cout << "Enter code of alternate medicine: ";
	getline(cin, code2);
	code2 = whiteSpaceRemover(code2, 1);
	if(checkRecordsIfExists(code2, "internal_code", "alternatives") || checkRecordsIfExists(code2, "internal_code2", "alternatives")){
		cout << "Already in database!!" << endl;
		return 1;
	}
	cout << format("Medicine with code {} is '{}'", code2, textFetcher("name", "internal_data", "internal_code", code2)) << endl;
	cout << "Confirm? (y/n)";
	char choice; 
	cin >> choice;
	choice = tolower(choice);
	if(choice != 'y'){
		cout << "Ok, alternate medicine not added" << endl;
		return 1;
	}
	else{
		const char *data = "Adder";
		char *zErrMsg = 0;
		string query;
		query = format("INSERT INTO alternatives VALUES('{}', '{}');", code1, code2);
		rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
		cout << "Alternatives  added successfully." << endl;
		return 0;
	}
}

std::vector<string> the_codes;
static int searcher(void *NotUsed, int argc, char **argv, char **azColName){
	int i = argc;
	for(int it = 0; it < i; it++){
		string temp = argv[it];
		the_codes.push_back(temp);
	}
	return 0;
}

std::vector<string> codes_to_names;
static int namer(void *NotUsed, int argc, char **argv, char **azColName){
	codes_to_names.push_back(argv[0]);
	return 0;
}

int searchForStuff(){
	the_codes.clear();
	codes_to_names.clear();
	string to_search, query;
	const char *data = "Searcher for stuff";
	char *zErrMsg = 0;
	cout << "Enter medicine name: ";
	getline(cin, to_search);
	to_search = whiteSpaceRemover(to_search);
	query = format("SELECT internal_code FROM internal_data WHERE name LIKE '%{}%' ORDER BY CASE WHEN name LIKE '{}' THEN 1 WHEN name LIKE '{}%' THEN 2 WHEN name LIKE '%{}' THEN 4 ELSE 3 END;", to_search, to_search, to_search, to_search);
	rc = sqlite3_exec(db, query.c_str(), searcher, (void*)data, &zErrMsg);
	for(auto &namename : the_codes){
		codes_to_names.push_back(textFetcher("name", "internal_data", "internal_code", namename));
	}
	for(int i = 0; i < the_codes.size(); i++){
		cout << format("{} has name {} with quantity {}", the_codes.at(i), codes_to_names.at(i), textFetcher("quantity", "main_data", "internal_code", the_codes.at(i))) << endl;
	}
	return 0;
}


int discountAdder(){
	string dcode;
	cout << "Enter discount code: ";
	getline(cin, dcode);
	dcode = whiteSpaceRemover(dcode, 1);
	if(!checkRecordsIfExists(dcode, "discount_code", "discounts")){
		int perc;
		cout << "Enter percentage(between 1 and 60): ";
		cin >> perc;
		cin.ignore();
		if (perc > 60 | perc < 1){
			cout << "Invalid entry." << endl;
			return -1;
		}
		char const *data = "ok";
		char * zErrMsg = 0;
		string discount_query;
		discount_query = format("INSERT INTO discounts VALUES ('{}',{});", dcode, perc);
		rc = sqlite3_exec(db, discount_query.c_str(), callback, (void*)data, &zErrMsg);
	}
	else{
		cout << "Discount code already exists!" << endl;
	}
}


string alternateFinder(string to_code){
	string the_alternate;
	if(checkRecordsIfExists(to_code, "alternatives", "internal_code")){
		the_alternate = textFetcher("internal_code2", "alternatives", "internal_code", to_code);
	}
	else
		the_alternate = textFetcher("internal_code", "alternatives", "internal_code2", to_code);
	return the_alternate;
}

int deleter(string todelete_code){
	char choice;
	cout << "Confirm deletion? :";
	cin >> choice;
	cin.ignore();
	choice = tolower(choice);
	if(choice == 'y'){
		cout << "Final Conformation: ";
		cin >> choice;
		cin.ignore();
		choice = tolower(choice);
		if(choice == 'y'){
			const char * data = "Gonna Delete";
			char *zErrMsg = 0;
			string query;
			query = format("DELETE FROM internal_data WHERE internal_code='{}'", todelete_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			query = format("DELETE FROM main_data WHERE internal_code='{}'", todelete_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			query = format("DELETE FROM composition_data WHERE internal_code='{}'", todelete_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			query = format("DELETE FROM side_effects WHERE internal_code='{}'", todelete_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
			query = format("DELETE FROM internal_data WHERE internal_code='{}' OR internal_code2='{}'", todelete_code, todelete_code);
			rc = sqlite3_exec(db, query.c_str(), callback, (void*)data, &zErrMsg);
		}
	}
}


int loginFunction(){ // checks for password and returns 1 for correct and 0 for false
	SetStdinEcho(false);
	cout << "Enter Password(will not be echoed): ";
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
			searchForStuff();
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
	searchForStuff();
	temp_counter = new int;
	Transaction trans1;
	trans1.addItem("A102", 5);
	trans1.addItem("A108", 9);
	trans1.addItem("A105", 7);
	// trans1.totaler();
	trans1.billPrinter();
	searchForStuff();
	trans1.remover("A102");
	trans1.billPrinter();
	searchForStuff();
	return 0;
}




// menu to choose stuff ~
// start transaction ~
// search for medicine ~
// view current bill ~
// remove medicine ~
// apply discount code ~
// alternatice medicine finder ~
// print bill to file


// management mode <- requires extra password
// add records to database(s) ~
// remove records from database(s)
// add discount code for discount percentage ~