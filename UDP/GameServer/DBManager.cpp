#include "DBManager.h"



DBManager::DBManager()
{
	driver = get_driver_instance();
	con = driver->connect(HOST, USER, PWD);
	stmt = con->createStatement();
	stmt->execute("USE gamedb");
}


DBManager::~DBManager()
{
	delete stmt;
	delete con;
}

bool DBManager::AltaCuenta(string name, string pwd) {
	ResultSet* rs = stmt->executeQuery("select count(*) from Accounts
}
