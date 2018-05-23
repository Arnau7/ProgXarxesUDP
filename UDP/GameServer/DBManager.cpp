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

//Funció per comprovar si el nom amb el que vol registrar-se un usuari ja existeix
bool DBManager::AltaCuenta(string name, string pwd) {
	ResultSet* rs = stmt->executeQuery("select count(*) from Accounts where username = 'name'");
	rs->next();
	int num = rs->getInt(1);
	delete rs;
	if (num == 0) //Em puc donar d'alta, ja que 0 significa que no existeix cap ususari amb el nom introduït
	{
		//Introduïm nom i contrassenya a la base de dades
		stmt->execute("insert into Accounts (username, userPwd) values ('name', 'pwd')");
		return true;
	}
	//El nom ja existeix
	return false;
}

bool DBManager::Login(string name, string pwd) {
	ResultSet* rs = stmt->executeQuery("select count(*) from Accounts where username = 'name' AND userPwd = 'pwd'");
	rs->next();
	int num = rs->getInt(1);
	if (num == 1) {
		return true;
	}
	return false;
}

bool DBManager::AddMatch(int idUser) {

}

void DBManager::CloseSession() {

}
