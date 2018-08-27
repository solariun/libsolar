/*
 * Sqlite3DBAccess.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: gustavocampos
 */

#include "Sqlite3DBAccess.hpp"


/*
 * ==============================
 * DBAccess Implementations
 * ==============================
 * */

Sqlite3DBAccess::Sqlite3DBAccess()
{
	TRACE << "Starting up Sqlite3DBAccess" << endl;
}

Sqlite3DBAccess::~Sqlite3DBAccess()
{
	TRACE << "Deliting up Sqlite3DBAccess" << endl;

	if (db != nullptr)
	{
		sqlite3_close_v2(db);
	}
}
  

DBColumns Sqlite3DBAccess::TableRows(string strTableName)
{
	Verify (db != nullptr, "SQLite3 Fail to open, no memory was created.", DBA_ERR_FAIL_OPEN_DB, DBAccessException);

	Sqlite3DBQuery dbQuery (*this);

	DBColumns dbQueryColumns = dbQuery.query("PRAGMA table_info (" + strTableName + ");");

	DBColumns mapReturn;

	int nCount=0;
	int nIndex = dbQueryColumns ["name"];


	while (dbQuery.step())
	{
		mapReturn.emplace (dbQuery.getString(nIndex), nCount++);
	}


	return mapReturn;
}

void Sqlite3DBAccess::open(string strValue)
{
	Verify (strValue.length() > 0, "Fail to open, no Database name supplied", DBA_ERR_FAIL_OPEN_DB, DBAccessException);

	int nRc = sqlite3_open (strValue.c_str(), &db);

	Verify (db != nullptr, "SQLite3 Fail to open, no memory was created.", DBA_ERR_FAIL_OPEN_DB, DBAccessException);
	Verify (nRc == 0, "SQLite3 Fail to open: " + sqlite3_errmsg(db), DBA_ERR_FAIL_OPEN_DB, DBAccessException);
}


void Sqlite3DBAccess::close()
{
	if (db != nullptr)
	{
		sqlite3_close_v2(db);
		db = nullptr;
	}
}


// Return wether there is
// a open database context.
bool Sqlite3DBAccess::isOpen()
{
	if (db == nullptr)
	{
		return false;
	}

	return true;
}


/*
 * ==============================
 * DBQuery Implementations
 * ==============================
 * */


/*
 * Constructor
 *
 */
Sqlite3DBQuery::Sqlite3DBQuery(Sqlite3DBAccess& dbAccess) : dbAccess (dbAccess)
{
	Verify (dbAccess.isOpen() == true, "SQLite3: Error, Database not Open.", DBA_ERR_NOT_OPEN, DBAccessException);
}


/*
 * bind
 *
 * Binding functions value to the sql
 *
 * 	int
 * 	string
 * 	null
 * 	double
 */

void Sqlite3DBQuery::bind(int nIndex, int nValue)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_bind_int (dbState, nIndex, nValue) == SQLITE_OK ,"SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_BIND_ERROR, DBAccessException);
}


void Sqlite3DBQuery::bind(int nIndex, double dValue)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_bind_double (dbState, nIndex, dValue) == SQLITE_OK ,"SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_BIND_ERROR, DBAccessException);
}


/*
 * bind - String
 *
 * For compatibility reasons, the process will always
 * work with TRANSIENT calling, which mean it will always
 * create a copy of the date do be processed.
 *
 */

void Sqlite3DBQuery::bind(int nIndex, const string strValue)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_bind_text (dbState, nIndex, strValue.c_str(), (int) strValue.length(), SQLITE_TRANSIENT) == SQLITE_OK ,"SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_BIND_ERROR, DBAccessException);
}


/*
 * bind - String
 *
 * For compatibility reasons, the process will always
 * work with STAITC calling, which mean it will always
 * relay on the binded data to exist, no memory release
 * will be done on error or destructor calling.
 *


void Sqlite3DBQuery::bind(int nIndex, const string& strValue)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState == nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_bind_text (dbState, nIndex, strValue.c_str(), strValue.length(), SQLITE_STATIC) == SQLITE_OK ,"SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_BIND_ERROR, DBAccessException);
}
*/


void Sqlite3DBQuery::bindNull (int nIndex)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_bind_null (dbState, nIndex) == SQLITE_OK ,"SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_BIND_ERROR, DBAccessException);

}



int Sqlite3DBQuery::exec()
{
	static int nAccess = 0;

	int nCurrent = nAccess++;

	while (nAccess > 1 && nAccess > nCurrent)
	{
		usleep(5000);
	}

	int nReturn;

	try
	{
		Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

		Verify (dbState != nullptr, "SQLite3 Error, still a open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

		Verify (sqlite3_step (dbState) == SQLITE_DONE, "SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_QUERY_EXEC_ERROR, DBAccessException);

		nReturn = sqlite3_changes (dbAccess.db);
	}
	catch (...)
	{
		nAccess--;

		throw;
	}

	nAccess--;

	return nReturn;
}

/*
 * ===============================
 * Query
 *
 * This will return the fields
 */

DBColumns Sqlite3DBQuery::query(const string strQuery)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (strQuery.length() > 0, "SQLite3 logical error, Query must be greater than zero.", DBA_ERR_QUERY_MUST_NOT_EMPTY, DBAccessException);

	if (dbState != nullptr)
	{
		sqlite3_finalize(dbState);
		dbState = nullptr;
	}

	Verify (sqlite3_prepare_v2 (dbAccess.db, strQuery.c_str(), (int) strQuery.length(), &dbState, nullptr) == SQLITE_OK, "SQLite3 Error: " + sqlite3_errmsg(dbAccess.db), DBA_ERR_QUERY_STATE_ERROR, DBAccessException);


	cout << "Number Columns: [" << sqlite3_column_count(dbState) << "]" << endl;

	DBColumns mapDBFields;
	const char* pszTemp;

	for (int nCount=0; nCount < sqlite3_column_count(dbState); nCount++)
	{
		pszTemp = sqlite3_column_name(dbState, nCount);
		pszTemp = (pszTemp == nullptr ? "NULL" : pszTemp);

		//TRACE << "Label: [" << pszTemp << "]=(" << nCount << ")" << endl;

		mapDBFields.emplace(pszTemp, nCount);
	}


	return mapDBFields;
}


/*
 * Step
 *
 * This function is important to get all the date
 * through a loop interaction giving to the program
 * the ability to treat data as it is interacted
 *
 * Work: For every interaction it will say true,
 *       when no more data to interact it will
 *       return false;
 *
 */


int Sqlite3DBQuery::step()
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	int nReturn = sqlite3_step (dbState);

	if (nReturn == SQLITE_ROW) return true;

	Verify (nReturn == SQLITE_DONE, "SQLite3 Error: " + sqlite3_errmsg(dbAccess.db),DBA_ERR_QUERY_EXEC_ERROR,  DBAccessException);

	return false;
}

/*
 * getString
 *
 * return a new string with the index(nColumn) data
 * represented in String
 *
 */

string Sqlite3DBQuery::getString(int nColumn)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_column_count(dbState) > nColumn, "SQLite3 Error, column index invalid, max is" + to_string(sqlite3_column_count(dbState)), DBA_ERR_QUERY_INDEX_ERROR, DBAccessException);

	const char* pszValue = (const char*) sqlite3_column_text(dbState, nColumn);

	string strValue (pszValue == nullptr ? "NULL" : pszValue);

	return strValue;
}


/*
 * getStriongbyRef
 *
 * Return the value of stored in the given Index(nColumn)
 * represented in String and stored in the
 * Referenced string strReturn
 *
 */

string& Sqlite3DBQuery::getStringbyRef(int nColumn, string& strReturn)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_column_count(dbState) <= nColumn, "SQLite3 Error, column index invalid, max is" + to_string(sqlite3_column_count(dbState)), DBA_ERR_QUERY_INDEX_ERROR, DBAccessException);

	const char* pszValue = (const char*) sqlite3_column_text(dbState, nColumn);

	strReturn = (pszValue == nullptr ? "NULL" : pszValue);

	return strReturn;
}



/*
 * getInteger
 *
 * Return the value of stored in the given Index(nColumn)
 * represented in int
 *
 */

int Sqlite3DBQuery::getInteger(int nColumn)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_column_count(dbState) <= nColumn, "SQLite3 Error, column index invalid, max is" + to_string(sqlite3_column_count(dbState)), DBA_ERR_QUERY_INDEX_ERROR, DBAccessException);

	return sqlite3_column_int(dbState, nColumn);
}


/*
 * getInteger
 *
 * Return the value of stored in the given Index(nColumn)
 * represented in int
 *
 */

double Sqlite3DBQuery::getDouble(int nColumn)
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);

	Verify (sqlite3_column_count(dbState) <= nColumn, "SQLite3 Error, column index invalid, max is" + to_string(sqlite3_column_count(dbState)), DBA_ERR_QUERY_INDEX_ERROR, DBAccessException);

	return sqlite3_column_double (dbState, nColumn);
}

Sqlite3DBQuery::~Sqlite3DBQuery()
{
	if (dbState == nullptr)
	{
		//To ensure this destructor is safe
		//the finalization will not check it
		//only execute.
		sqlite3_finalize (dbState);
	}
}


void Sqlite3DBQuery::reset()
{
	Verify (dbAccess.isOpen() == true, "SQLite3 Error, Database not open.", DBA_ERR_NOT_OPEN, DBAccessException);

	Verify (dbState != nullptr, "SQLite3 Error, no open query state.", DBA_ERR_STATE_ALREAY_OPEN, DBAccessException);


}
