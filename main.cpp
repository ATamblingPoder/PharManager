#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <string>
using namespace std;

/*
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
*/

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

int main(int argc, char const *argv[]){
	sqlite3 *db;
   char *zErrMsg = 0;
	int rc;
   char *sql;
   rc = sqlite3_open("test.db", &db);
   if( rc ) {
   	std::cout << "Can't open database:" << sqlite3_errmsg(db);
      return(0);
   } 
   else {
      std::cout << "Opened database successfully";
   }
   sql = "CREATE TABLE IF NOT EXISTS test(" \
         "id int primary key,"\
         "name varchar(20),"\
         "expiry date);";
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if(rc != SQLITE_OK){
      cout << "SQL Error";
   }
   sql = "INSERT INTO test VALUES" \
         "(1, 'Ansh', '2023-09-09');";
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   sql = "SELECT * FROM test;";
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   sqlite3_close(db);
}