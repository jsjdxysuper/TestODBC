/*
 * main.cpp
 *
 *  Created on: 2017-8-7
 *      Author: user
 */
#include<string>
#include<iostream>
#include<string.h>
#include"OdbcWrap.h"
using namespace::std;
void testInsert();
int main()
{

	OdbcWrap odbcW("DM","HLJJHDB","HLJJHDB",false);
	odbcW.init();
	string insertSql = "INSERT INTO HLJJHDB.HLJJHDB.TEST(SID,SNAME,TCHAR,TFLOAT,TINT,TNUMBER,SDATE) VALUES(?,?,?,?,?,?,?)";
	vector<vector<string> >paras;
	for(int i=0;i<1000;i++)
	{
		stringstream ss;
		vector<string>oneRow;
		ss<<"sid"<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<"sname"<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<"tchar"<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<"23.0"<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<"32."<<i;
		oneRow.push_back(ss.str());
		ss.str("");
		ss<<"2017-07-12";
		oneRow.push_back(ss.str());
		//cout<<endl<<oneRow.at(0)<<"	"<<oneRow.at(1)<<"	"<<oneRow.at(2)<<"	"<<oneRow.at(3)<<"	"<<oneRow.at(4)<<"	"<<oneRow.at(5)<<" "<<oneRow.at(6)<<endl;
		paras.push_back(oneRow);
	}
	odbcW.execlPrepareUpdate(insertSql,paras);
	odbcW.commit();
}
void testSelect()
{
	OdbcWrap odbcW("DM","HLJJHDB","HLJJHDB",false);
	odbcW.init();
	string selectSql = "SELECT SID,SNAME,TCHAR,TFLOAT,TINT,TNUMBER FROM HLJJHDB.HLJJHDB.TEST";
	odbcW.execSelect(selectSql);
	while(odbcW.hasNext())
	{

		string sid = odbcW.getStrData(1);
		string sname=odbcW.getStrData(2);
		string tchar = odbcW.getStrData(3);
		double tfloat = odbcW.getDoubleData(4);
		int tint = odbcW.getIntData(5);
		double tnumber = odbcW.getDoubleData(6);
		cout<<endl;
		cout<<"sid:"<<sid<<",sname:"<<sname<<",tchar:"<<tchar<<",tfloat:"<<tfloat<<",tint:"<<tint<<",tnumber:"<<tnumber;
	}
}
void testInsert()
{
	OdbcWrap odbcW("DM","HLJJHDB","HLJJHDB",false);
	odbcW.init();
	long sumInfluence = 0;
	for(int i=0;i<20000;i++)
	{
		string insertSql = "INSERT INTO HLJJHDB.HLJJHDB.TEST(SID,SNAME,TCHAR,TFLOAT,TINT,TNUMBER) VALUES("
				"'sid','sname','tchar','23.03','43','23.32322')";
		long row = odbcW.execlUpdate(insertSql);
		sumInfluence+=row;
	}
	cout<<endl<<"影响了"<<sumInfluence<<"hang";
	//odbcW.commit();
}

