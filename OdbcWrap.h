/*
 * OdbcWrap.h
 *
 *  Created on: 2017-8-7
 *      Author: user
 */

#ifndef ODBCWRAP_H_
#define ODBCWRAP_H_
#include<string>
#include <sql.h>
#include<sqlext.h>
#include<stdio.h>
#include<sstream>
#include<stdlib.h>
#include<vector>
#define COLUMN_NAME_LEN 1024
using namespace::std;

class OdbcWrap
{
private:
	string 		DNSStr;
	string 		userIdStr;
	string 		pwdStr;

	SQLHENV  	hENV;
	SQLHDBC    	hDBC;
	SQLHSTMT  	hSTMT;
	bool 		isAutoCommit;

	void dealWithRet(SQLSMALLINT hdlType,SQLHANDLE   handle,SQLRETURN retcode,const char mes[]);
public:
	OdbcWrap(string DNS,string user,string pwd,bool isAC=true):DNSStr(DNS),userIdStr(user),pwdStr(pwd),isAutoCommit(isAC)
	{
		hENV = NULL;
		hDBC = NULL;
		hSTMT = NULL;
	}
	void init();
	long execlPrepareUpdate(string preSql,vector<vector<string> >para);
	long execlUpdate(string sqlStr);
	void execSelect(string sqlStr);
	double getDoubleData(int columnIndex);
	int getIntData(int columnIndex);
	string getStrData(int columnIndex);
	bool hasNext();
	void roolBack();
	void commit();
	~OdbcWrap()
	{
		if(NULL!=hSTMT)
			{SQLFreeHandle(SQL_HANDLE_STMT, &hSTMT);hSTMT=NULL;}
		if(NULL!=hDBC)
			{SQLFreeHandle(SQL_HANDLE_DBC, &hDBC);hDBC=NULL;}
		if(NULL!=hENV)
			{SQLFreeHandle(SQL_HANDLE_ENV, &hENV);hENV=NULL;}
	}
};


#endif /* ODBCWRAP_H_ */
