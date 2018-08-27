/*
 DBAccess.hpp
 libsolar

 Created by gustavocampos on Aug 14, 2018
 Copyright © 2018 GUSTAVO CAMPOS. 
 All rights reserved.

 ----------------------------------------------

 MIT License
 
 Copyright (c) [year] [fullname]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef DBACCESS_HPP_
#define DBACCESS_HPP_

#include "Exception.hpp"
#include "Util.hpp"

#include <vector>
#include <map>

//Exception class
class DBAccessException : public Exception
{
public:
	virtual ~DBAccessException(){}
	DBAccessException (std::string strMessage, uint nErrorID);
};



//ERROR ENUMS
enum __DBAccessErr
{
	DBA_ERR_NOT_OPEN = 10,
	DBA_ERR_COULD_NOT_OPEN,
	DBA_ERR_FAIL_OPEN_DB,

	DBA_ERR_STATE_ALREAY_OPEN = 100,
	DBA_ERR_STATE_CLOSE_ERROR,

	DBA_ERR_QUERY_MUST_NOT_EMPTY = 500,
	DBA_ERR_QUERY_STATE_ERROR,
	DBA_ERR_QUERY_EXEC_ERROR,
	DBA_ERR_QUERY_INDEX_ERROR,

	DBA_ERR_BIND_ERROR = 1000
};


//typedef implementations

typedef map<string, int> DBColumns;
typedef vector<string> DBRowNames;

//Implementation Class
class DBAccess
{
public:
	DBAccess();
	virtual ~DBAccess();

	virtual void open(string strValue) = 0;
	virtual void close()=0;

	virtual bool isOpen()=0;


	virtual DBColumns TableRows(string strTableName)=0;

};

class DBQuery
{

public:

	virtual ~DBQuery();

	virtual void bind (int nIndex, int nValue)=0;
	virtual void bind (int nIndex, double dValue)=0;
	virtual void bind (int nIndex, const string strValue)=0;

	virtual void bindNull (int nIndex)=0;

	//Execute only and return number affected rows
	virtual int exec ()=0;

	virtual DBColumns query (const string strQuery)=0;

	virtual int step ()=0;

	virtual void reset ()=0;

	virtual string getString (int nColumn)=0;
	virtual string& getStringbyRef (int nColumn, string& strReturn)=0;

	virtual int getInteger (int nColumn)=0;
	virtual double getDouble  (int nColumn)=0;

};



#endif /* DBACCESS_HPP_ */
