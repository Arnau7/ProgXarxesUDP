#pragma once
#include <mysql_connection.h>
#include <cppconn\driver.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\exception.h>
#include <iostream>

using namespace std;
using namespace sql;

#define HOST ""
#define USER "root"
#define PWD "linux123"

class DBManager
{
private:
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	//sql::ResultSet* resultSet;
public:
	int idAccount, idSession;
	bool AltaCuenta(string, string);
	bool Login(string, string);
	bool AddMatch(int);
	void CloseSession();
	DBManager();
	~DBManager();
};

