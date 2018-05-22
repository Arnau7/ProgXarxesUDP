#pragma once
#include <iostream>
using namespace std;

class DBManager
{
public:
	int idAccount, idSession;
	bool AltaCuenta(string, string);
	bool Login(string, string);
	bool AddMatch(int);
	void CloseSession(int);
	DBManager();
	~DBManager();
};

