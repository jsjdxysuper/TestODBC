//Main.cpp 测试用 需要自行添加数据源 在odbc里面
#include <iostream>
#include <string.h>
#include<vector>
#include<string>
#include <sql.h>
#include<sqlext.h>
#include<stdio.h>
#include<sstream>

using namespace std;
void dealWithRet(SQLSMALLINT hdlType,SQLHANDLE   hSTMT,SQLRETURN retcode,const char mes[]);
void testPrepare();
void testSelect();
int main()
{
	testPrepare();
	//testSelect();
}
void testSelect()
{
	SQLHENV  henv =NULL;
	SQLHDBC    hDBC=NULL;
	SQLHSTMT   hSTMT=NULL;
	SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV,NULL,&henv);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"Env");
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"ODBC Version");
	retcode = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hDBC);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"DBC");

	retcode = SQLConnect(hDBC, (SQLCHAR*) "DM", SQL_NTS,(SQLCHAR*) "HLJJHDB", SQL_NTS, (SQLCHAR*)"HLJJHDB", SQL_NTS);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"connect");
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"stm");


	SQLCHAR selSql[] = "SELECT SID,SNAME,TCHAR,TFLOAT,TINT FROM HLJJHDB.HLJJHDB.TEST";

	retcode = SQLExecDirect(hSTMT,selSql,SQL_NTS);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"sqlExecDirect");


	SQLINTEGER nTINT;
	SQLCHAR szSid[50],szName[50], szTChar[50];
	SQLDOUBLE tFloat;
	SQLLEN cbSid,cbName, cbChar,cbFloat,cbInt;//用来保存得到的数据的长度
	while(SQL_NO_DATA != SQLFetch(hSTMT)) //移动光标，一直到集合末尾
	{//得到结果

		SQLGetData(hSTMT, 1, SQL_C_CHAR, szSid, 50, &cbSid);
		//此处并没有指明BufferLength 参数的值，是因为数据类型是定长的LONG 型
		SQLGetData(hSTMT, 2, SQL_C_CHAR, szName, 50, &cbName);
		SQLGetData(hSTMT, 3, SQL_C_CHAR, szTChar, 50,&cbChar);
		SQLGetData(hSTMT, 4, SQL_C_DOUBLE, &tFloat, 0,&cbChar);
		SQLGetData(hSTMT, 5, SQL_C_SLONG, &nTINT, 0,&cbChar);
		printf( "\n%s %s %s %lf %d", szSid, szName, szTChar,tFloat,nTINT);
	}


	SQLFreeHandle(SQL_HANDLE_ENV, &henv);
	SQLFreeHandle(SQL_HANDLE_DBC, &hDBC);
	SQLFreeHandle(SQL_HANDLE_STMT, &hSTMT);
}
void testPrepare()
{
	SQLHENV  henv =NULL;
	SQLHDBC    hDBC=NULL;
	SQLHSTMT   hSTMT=NULL;
	SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV,NULL,&henv);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"Env");
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"ODBC Version");
	retcode = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hDBC);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"DBC");

	retcode = SQLConnect(hDBC, (SQLCHAR*) "DM", SQL_NTS,(SQLCHAR*) "HLJJHDB", SQL_NTS, (SQLCHAR*)"HLJJHDB", SQL_NTS);
	dealWithRet(SQL_HANDLE_DBC,hDBC,retcode,"connect");
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hDBC,retcode,"stm");

	vector<vector<string> > rowsParas;
	for(int i=0;i<600;i++)
	{
		stringstream s;
		vector<string>oneRow;
		s<<i;

		oneRow.push_back(s.str());
		s.str("");
		s<<i<<"name";
		oneRow.push_back(s.str());
		s.str("");
		s<<i<<"ssssssss";
		oneRow.push_back(s.str());
		s.str("");
		s<<i*0.65;
		oneRow.push_back(s.str());
		s.str("");
		s<<i;
		oneRow.push_back(s.str());
		s.str("");
		s<<i;
		oneRow.push_back(s.str());
		s.str("");
		s<<"2017-07-25";
		oneRow.push_back(s.str());
		cout<<oneRow.at(0)<<"	"<<oneRow.at(1)<<"	"<<oneRow.at(2)<<"	"<<oneRow.at(3)<<"	"<<oneRow.at(4)<<"	"<<oneRow.at(5)<<" "<<oneRow.at(6)<<endl;
		rowsParas.push_back(oneRow);
	}

	SQLCHAR insertSql[] = "INSERT INTO HLJJHDB.HLJJHDB.TEST(SID,SNAME,TCHAR,TFLOAT,TINT,TNUMBER,SDATE) "
			"VALUES(?,?,?,?,?,?,?)";
	retcode = SQLPrepare(hSTMT,insertSql,SQL_NTS);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"prepareSql");
	SQLLEN cb1=0,cb2=SQL_NTS;

	//SQLBindParameter(hSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &para,0, &cb1);
	for(string::size_type j=0;j<rowsParas.size();j++)
	{
		for(int i=1;i<=7;i++)
		{
			retcode = SQLBindParameter(hSTMT, i, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_VARCHAR, 5000, 0,(SQLPOINTER)rowsParas.at(j).at(i-1).c_str(), 5000, &cb2);
			stringstream s;
			s<<"SQLBindParameter"<<i;
			dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,s.str().c_str());
			s.str("");
		}
		retcode = SQLExecute(hSTMT);
		stringstream s;
		s<<"SQLExecute"<<":"<<j;
		dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,s.str().c_str());
		s.str("");
	}

	SQLFreeHandle(SQL_HANDLE_STMT,&hSTMT);
	SQLFreeHandle(SQL_HANDLE_ENV, &henv);
	SQLFreeHandle(SQL_HANDLE_DBC, &hDBC);


}


void execDirect()
{
	SQLHENV  henv =NULL;
	SQLHDBC    hDBC=NULL;
	SQLHSTMT   hSTMT=NULL;
	SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV,NULL,&henv);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"Env");
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	dealWithRet(SQL_HANDLE_ENV,henv,retcode,"ODBC Version");
	retcode = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hDBC);
	dealWithRet(SQL_HANDLE_ENV,hDBC,retcode,"DBC");
	SQLSetConnectAttr ( hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF,SQL_IS_POINTER);

	retcode = SQLConnect(hDBC, (SQLCHAR*) "DM", SQL_NTS,(SQLCHAR*) "HLJJHDB", SQL_NTS, (SQLCHAR*)"HLJJHDB", SQL_NTS);
	dealWithRet(SQL_HANDLE_ENV,hDBC,retcode,"connect");
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,hDBC,&hSTMT);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"stm");




	SQLCHAR* szsql = (SQLCHAR*)"insert into HLJJHDB.HLJJHDB.TEST values('FF','FF')";
	retcode = SQLExecDirect(hSTMT,szsql,SQL_NTS);
	dealWithRet(SQL_HANDLE_STMT,hSTMT,retcode,"sqlExecDirect");
	SQLEndTran(SQL_HANDLE_DBC,hDBC,SQL_ROLLBACK);//SQL_COMMIT

	SQLFreeHandle(SQL_HANDLE_ENV, &henv);
	SQLFreeHandle(SQL_HANDLE_DBC, &hDBC);
	SQLFreeHandle(SQL_HANDLE_STMT, &hSTMT);
}
void dealWithRet(SQLSMALLINT hdlType,SQLHANDLE   handle,SQLRETURN retcode,const char mes[])
{
	if(retcode==SQL_SUCCESS)
	{
		printf("\n%s,success",mes);
	}else if(retcode==SQL_SUCCESS_WITH_INFO)
	{
		printf("\n%s,SQL_SUCCESS_WITH_INFO",mes);
	}else if(retcode==SQL_ERROR)
	{
		printf("\n%s,SQL_ERROR",mes);
		SQLCHAR SqlState[6],SQLStmt[100],Msg[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER NativeError;
		SQLSMALLINT i, MsgLen;
		SQLRETURN rc2;
		i = 1;
		while ((rc2 = SQLGetDiagRec(hdlType, handle, i, SqlState,
				&NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA)
		{
			printf("\n%s",Msg);
		//显示错误的代码
		i++;
		}
	}else if(retcode==SQL_INVALID_HANDLE)
	{
		printf("\n%s,SQL_INVALID_HANDLE",mes);
	}else
		printf("\n%s,SQL_NO_DATA",mes);
}

