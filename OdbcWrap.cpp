/*
 * OdbcWrap.cpp
 *
 *  Created on: 2017-8-7
 *      Author: user
 */
#include"OdbcWrap.h"
#include<string.h>
#include<vector>
using namespace::std;

void OdbcWrap::dealWithRet(SQLSMALLINT hdlType,SQLHANDLE handle,SQLRETURN retcode,const char mes[])
{
	if(retcode==SQL_SUCCESS)
	{
		//printf("\n%s,success",mes);
	}else if(retcode==SQL_SUCCESS_WITH_INFO)
	{
		printf("\n%s,SQL_SUCCESS_WITH_INFO",mes);
	}else if(retcode==SQL_ERROR)
	{
		printf("\n%s,SQL_ERROR",mes);
		SQLCHAR SqlState[6],SQLStmt[100],Msg[SQL_MAX_MESSAGE_LENGTH];
		memset(Msg,0,SQL_MAX_MESSAGE_LENGTH);
		SQLINTEGER NativeError;
		SQLSMALLINT i, MsgLen;
		SQLRETURN rc2;
		i = 1;
		while ((rc2 = SQLGetDiagRec(hdlType, handle, i, SqlState,
				&NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA_FOUND&&i<5)
		{
			printf("\n%s",Msg);
			//显示错误的代码
			i++;
		}
		exit(1);
	}else if(retcode==SQL_INVALID_HANDLE)
	{
		printf("\n%s,SQL_INVALID_HANDLE",mes);
	}else
		printf("\n%s,SQL_NO_DATA",mes);
}

void OdbcWrap::init()
{
	SQLRETURN retcode = 0;
	retcode = SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hENV);
	dealWithRet(SQL_HANDLE_ENV,hENV,retcode,"Env");
	retcode = SQLSetEnvAttr(hENV, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	dealWithRet(SQL_HANDLE_ENV,hENV,retcode,"ODBC Version 3.0");
	retcode = SQLAllocHandle(SQL_HANDLE_DBC,hENV,&hDBC);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"DBC");
	if(isAutoCommit)
		retcode = SQLSetConnectAttr ( hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON,SQL_IS_POINTER);
	else
		retcode = SQLSetConnectAttr ( hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF,SQL_IS_POINTER);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"SET_ATTR_AUTOCOMMIT");
	retcode = SQLConnect(hDBC, (SQLCHAR*) DNSStr.c_str(), SQL_NTS,(SQLCHAR*) userIdStr.c_str(), SQL_NTS, (SQLCHAR*)pwdStr.c_str(), SQL_NTS);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"connect");
}

long OdbcWrap::execlPrepareUpdate(string preSql,vector<vector<string> >rowsParas)
{
	SQLRETURN retcode = 0;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hDBC,retcode,"prepare stm");
	retcode = SQLPrepare(hSTMT,(SQLCHAR*)preSql.c_str(),SQL_NTS);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"prepareSql");
	SQLLEN sumRowCount = 0;
	SQLLEN everyRowCount=0;
	SQLLEN cb1=0,cb2=SQL_NTS;
	vector<string>::size_type columnLen = rowsParas.at(0).size();
	for(string::size_type j=0;j<rowsParas.size();j++)
	{
		for(vector<string>::size_type i=1;i<=columnLen;i++)
		{
			retcode = SQLBindParameter(hSTMT, i, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_VARCHAR, 5000, 0,(SQLPOINTER)rowsParas.at(j).at(i-1).c_str(), 5000, &cb2);
			stringstream s;
			s<<"SQLBindParameter column:"<<i;
			dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,s.str().c_str());
			s.str("");
		}
		retcode = SQLExecute(hSTMT);
		stringstream s;
		s<<"SQLExecute"<<":"<<j;
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,s.str().c_str());

		retcode = SQLRowCount(hSTMT,&everyRowCount);
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"prepare influcenced row count");
		sumRowCount +=everyRowCount;
		s.str("");
	}

	retcode = SQLFreeHandle(SQL_HANDLE_STMT,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"free prepare statement");
}


long OdbcWrap::execlUpdate(string sqlStr)
{
	SQLRETURN retcode=0;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"statement");
	retcode = SQLExecDirect(hSTMT,(SQLCHAR*)sqlStr.c_str(),SQL_NTS);
	stringstream ss;
	ss<<endl<<"execlUpdate:"<<sqlStr;
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,ss.str().c_str());
	SQLLEN influenceRowCount=0;;
	retcode = SQLRowCount(hSTMT,&influenceRowCount);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"get fluenced row count");
	retcode = SQLFreeHandle(SQL_HANDLE_STMT,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"free statement");
	return influenceRowCount;

}


void OdbcWrap::execSelect(string sqlStr)
{

	SQLRETURN retcode = 0;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"select statement");
	retcode = SQLExecDirect(hSTMT,(SQLCHAR*)sqlStr.c_str(),SQL_NTS);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"sqlExecDirect");

	retcode = SQLFreeHandle(SQL_HANDLE_STMT,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"free statement");
}

double OdbcWrap::getDoubleData(int columnIndex)
{
	SQLRETURN retcode = 0;
	SQLDOUBLE tDouble;
	SQLLEN cbLen;
	retcode = SQLGetData(hSTMT, columnIndex, SQL_C_DOUBLE, &tDouble, 0, &cbLen);

	if(retcode==SQL_ERROR)
	{
		stringstream ss;
		ss<<endl<<"getDoubleData,column:"<<columnIndex;
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,ss.str().c_str());
	}
	return tDouble;

}
int OdbcWrap::getIntData(int columnIndex)
{
	SQLRETURN retcode = 0;
	SQLINTEGER tInt;
	SQLLEN cbLen;
	retcode = SQLGetData(hSTMT, columnIndex, SQL_C_SLONG, &tInt, 0, &cbLen);

	if(retcode==SQL_ERROR)
	{
		stringstream ss;
		ss<<endl<<"getIntData,column:"<<columnIndex;
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,ss.str().c_str());
	}
	return tInt;
}
string OdbcWrap::getStrData(int columnIndex)
{
	SQLRETURN retcode = 0;
	SQLCHAR columnName[COLUMN_NAME_LEN];
	SQLULEN columnLen;
	SQLDescribeCol(hSTMT,columnIndex,columnName,COLUMN_NAME_LEN,NULL,NULL,&columnLen,NULL,NULL);
	char *dataRet = (char*)malloc(columnLen);
	SQLLEN cbLen;
	retcode = SQLGetData(hSTMT, columnIndex, SQL_C_CHAR, dataRet, columnLen, &cbLen);
	stringstream ss;
	ss<<endl<<"getStrData:"<<columnIndex;
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,ss.str().c_str());
	string retData(dataRet);
	if(retcode==SQL_ERROR)
	{
		stringstream ss;
		ss<<endl<<"getStrData,column:"<<columnIndex;
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,ss.str().c_str());
	}
	return retData;
}
bool OdbcWrap::hasNext()
{
	if(SQL_NO_DATA != SQLFetch(hSTMT))
		return true;
	else
		return false;
}
void OdbcWrap::roolBack()
{
	SQLRETURN retcode = 0;
	retcode = SQLEndTran(SQL_HANDLE_DBC,hDBC,SQL_ROLLBACK);//SQL_ROLLBACK
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"ROLLBACK");
}
void OdbcWrap::commit()
{
	SQLRETURN retcode = 0;
	retcode = SQLEndTran(SQL_HANDLE_DBC,hDBC,SQL_COMMIT);//SQL_COMMIT
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"COMMIT");
}
