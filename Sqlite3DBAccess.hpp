/*
 * Sqlite3DBAccess.h
 *
 *  Created on: Aug 21, 2018
 *      Author: gustavocampos
 */

#ifndef SQLITE3DBACCESS_HPP_
#define SQLITE3DBACCESS_HPP_

#include <sqlite3.h>

#include "DBAccess.hpp"


class Sqlite3DBAccess: public DBAccess {
public:
	Sqlite3DBAccess();
	virtual ~Sqlite3DBAccess();

	void open(string strValue);
	void close();

	bool isOpen();

	DBColumns TableRows(string strTableName);

protected:
	sqlite3 *db = nullptr;

	friend class Sqlite3DBQuery;
};


class Sqlite3DBQuery : Sqlite3DBAccess
{

public:
	Sqlite3DBQuery(Sqlite3DBAccess& dbAccess);
	Sqlite3DBQuery() = delete;
	virtual ~Sqlite3DBQuery ();


	void bind (int nIndex, int nValue);
	void bind (int nIndex, double dValue);
	void bind (int nIndex, const string strValue);

	void bindNull (int nIndex);

	//Execute only and return number affected rows
	int exec ();

	DBColumns query (const string strQuery);

	int step ();

	void reset ();

	string getString (int nColumn);
	string& getStringbyRef (int nColumn, string& strReturn);

	int getInteger (int nColumn);
	double getDouble  (int nColumn);

protected:
	Sqlite3DBAccess &dbAccess;
	sqlite3_stmt* dbState = nullptr;
};

#endif /* SQLITE3DBACCESS_HPP_ */
