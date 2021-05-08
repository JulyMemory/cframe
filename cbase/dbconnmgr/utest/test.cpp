#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

#define OTL_ODBC_MYSQL

#include "../../../3rd/otl/otlv4.h"

using namespace std;

otl_connect db;

void insert() {
  otl_stream o(1,  // buffer size should be == 1 always on INSERT
               "insert into test_tab values(:f1<int>,:f2<char[31]>)",
               // SQL statement
               db  // connect object
  );
  char tmp[32];

  for (int i = 1; i <= 100; ++i) {
    sprintf(tmp, "Name%d", i);
    o << i << tmp;
  }
}

// update row data into table
void update(const int af1) {
  otl_stream o(1,  // buffer size should be == 1 always on UPDATE
               "UPDATE test_tab "
               "   SET f2=:f2<char[31]> "
               " WHERE f1=:f1<int>",
               // UPDATE statement
               db  // connect object
  );
  o << "Name changed" << af1;
  o << otl_null() << af1 + 1;  // set f2 to NULL
}

// MyODBC does not allow any input bind variables in the WHERE clause
// in a SELECT statement.
// Therefore, the SELECT statement has to be generated literally.
void select(const int af1) {
  char stmbuf[1024];
  sprintf(stmbuf, "select * from test_tab where f1>=%d and f1<=%d*2", af1, af1);
  otl_stream i(50,      // buffer size may be > 1
               stmbuf,  // SELECT statement
               db       // connect object
  );
  // create select stream

  int f1;
  char f2[31];

  while (!i.eof()) {  // while not end-of-data
    i >> f1;
    cout << "f1=" << f1 << ", f2=";
    i >> f2;
    if (i.is_null())
      cout << "NULL";
    else
      cout << f2;
    cout << endl;
  }
}

int main() {
  otl_connect::otl_initialize();  // initialize ODBC environment
  try {
    db.rlogon("UID=root;PWD=19960503;DSN=mysql");  // connect to ODBC
    // db.rlogon("scott/tiger@mysql"); // connect to ODBC, alternative format
    // of connect string

    // disable OTL exceptions
    // drop table
    otl_cursor::direct_exec(db, "drop table test_tab", otl_exception::disabled);

    // create table
    otl_cursor::direct_exec(db,
                            "create table test_tab(f1 int, f2 varchar(30))");

    insert();    // insert records into the table
    update(10);  // update records in the table
    select(8);   // select records from the table

  } catch (otl_exception& p) {   // intercept OTL exceptions
    cerr << p.msg << endl;       // print out error message
    cerr << p.stm_text << endl;  // print out SQL that caused the error
    cerr << p.sqlstate << endl;  // print out SQLSTATE message
    cerr << p.var_info << endl;  // print out the variable that caused the error
  }

  db.logoff();  // disconnect from ODBC

  return 0;
}