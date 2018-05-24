#pragma once
#include <mysql_connection.h>
#include <cppconn\driver.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\exception.h>
#include <iostream>

using namespace std;
using namespace sql;

#define HOST "tcp://192.168.123.22:3306"
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
	void TestTable();
	bool AltaCuenta(string, string);
	bool Login(string, string);
	bool AddMatch(int);
	void CloseSession();
	DBManager();
	~DBManager();
};

