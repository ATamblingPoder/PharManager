#include <iostream>
#include <fstream>
#include <sqlite3.h>
using namespace std;

int main(int argc, char const *argv[]){
	cout << "First" << endl;
	sqlite3 *db;
   	char *zErrMsg = 0;
   	int rc;
   	rc = sqlite3_open("test.db", &db);
   	if( rc ) {
   		std::cout << "Can't open database:" << sqlite3_errmsg(db);
      return(0);
   } else {
      std::cout << "Opened database successfully";
   }
   sqlite3_close(db);
}