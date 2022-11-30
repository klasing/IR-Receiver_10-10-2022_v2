#pragma once
//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
//****************************************************************************
//*                     Connect2SQLite
//****************************************************************************
class Connect2SQLite
{
	sqlite3* pdb = nullptr;
	int rc = -1;
	char* pszErrMsg{ 0 };
	std::string strSql = "";
public:
	//************************************************************************
	//*                 openDb
	//************************************************************************
	int openDb(const std::string& strNameDb)
	{
		rc = sqlite3_open(strNameDb.c_str()
			, &pdb
		);
		if (rc == 0)
		{
			// enable foreign key support
			rc = sqlite3_exec(pdb, "PRAGMA foreign_keys = ON", 0, 0, 0);
		}
		return rc;
	}
	//************************************************************************
	//*                 createTable
	//************************************************************************
	int createTable(const UINT& resourceIdTable
		, const std::string strNameTable
	)
	{
		switch (resourceIdTable)
		{
		case IDR_MEASUREMENT:
		{
			strSql =
				"CREATE TABLE IF NOT EXISTS measurement("
				"id INTEGER PRIMARY KEY, "
				"name TEXT NOT NULL UNIQUE"
				");";
			break;
		} // eof IDR_MEASUREMENT
		case IDR_VALUE_MEASUREMENT:
		{
			strSql =
				"CREATE TABLE IF NOT EXISTS value_measurement("
				"id INTEGER PRIMARY KEY, "
				"timestamp TEXT NOT NULL, "
				"value FLOAT, "
				"fk_measurement INTEGER, "
				"CONSTRAINT cnstrnt "
				"FOREIGN KEY(fk_measurement) REFERENCES mesurement(id)"
				");";
			break;
		} // eof IDR_VALUE_MEASUREMENT
		} // eof switch
		return rc;
	}
private:
	//************************************************************************
	//*                 execute
	//************************************************************************
	int execute(sqlite3* pdb
		, const std::string& strSql
		, void* pData
	)
	{
		rc = sqlite3_exec(pdb
			, strSql.c_str()
			, callback
			, pData
			, &pszErrMsg
		);
		return rc;
	}
	//************************************************************************
	//*                 callback
	//************************************************************************
	static int callback(void* pdata
		, int argc
		, char** argv
		, char** aszColName
	)
	{
		return EXIT_SUCCESS;
	}
};