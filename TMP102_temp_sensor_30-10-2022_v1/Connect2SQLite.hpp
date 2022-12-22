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
	std::string strNameHdrTable = "";
	std::string strNameValTable = "";
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
		, const std::string& strNameTable
	)
	{
		switch (resourceIdTable)
		{
		case IDR_HDR_TABLE:
		{
			strNameHdrTable = strNameTable;
			strSql =
				"CREATE TABLE IF NOT EXISTS "
				+ strNameHdrTable + "("
				"id INTEGER PRIMARY KEY, "
				"name TEXT NOT NULL UNIQUE, "
				"tlo REAL, "
				"thi REAL"
				");";
			break;
		} // eof IDR_HDR_TABLE
		case IDR_VAL_TABLE:
		{
			strNameValTable = strNameTable;
			strSql =
				"CREATE TABLE IF NOT EXISTS "
				+ strNameValTable + "("
				"id INTEGER PRIMARY KEY, "
				"timestamp TEXT NOT NULL, "
				"value REAL, "
				"device INTEGER, "
				"fk_header INTEGER, "
				"CONSTRAINT cnstrnt "
				"FOREIGN KEY(fk_header) REFERENCES " 
				+ strNameHdrTable + "(id)"
				");";
			break;
		} // eof IDR_VAL_TABLE
		} // eof switch
		rc = execute(pdb, strSql, nullptr);
		return rc;
	}
	//************************************************************************
	//*                 insertTuple
	//************************************************************************
	int insertTuple(const UINT& resourceIdTable
		, std::initializer_list<std::string>& list
	)
	{
		std::vector<std::string> vector_list{ list };
		switch (resourceIdTable)
		{
		case IDR_HDR_TABLE:
		{
			strSql = "INSERT INTO "
				+ strNameHdrTable
				+ "(name, tlo, thi) VALUES('"
				+ vector_list[0] + "', "
				+ vector_list[1] + ", "
				+ vector_list[2]
				+ ");";
			break;
		} // eof IDR_HDR_TABLE
		case IDR_VAL_TABLE:
		{
			strSql = "INSERT INTO "
				+ strNameValTable
				+ "(timestamp, value, device, fk_header) VALUES('"
				+ vector_list[0] + "', "
				+ vector_list[1] + ", "
				+ vector_list[2] + ", ("
				+ "SELECT id FROM " + strNameHdrTable  + " WHERE name='"
				+ vector_list[3] + "')"
				+ ");";			
			break;
		} // eof IDR_VAL_TABLE
		} // eof switch
		rc = execute(pdb, strSql, nullptr);
		return rc;
	}
	//************************************************************************
	//*                 closeDb
	//*
	//* not yet in use, because it is unclear where to close the database
	//************************************************************************
	int closeDb()
	{
		rc = sqlite3_close(pdb);
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

//****************************************************************************
//*                     date_for_http_responseA
//****************************************************************************
std::string
date_for_http_responseA()
{
	const std::string dow[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	const std::string month[] = { "Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	time_t tt;
	time(&tt);
	tm t;
	localtime_s(&t, &tt);
	struct tm gmt;
	gmtime_s(&gmt, &tt);
	std::ostringstream oss;
	oss << dow[gmt.tm_wday] << ", "
		<< std::setw(2) << std::setfill('0') << gmt.tm_mday << " "
		<< month[gmt.tm_mon] << " "
		<< gmt.tm_year + 1900 << " "
		<< std::setw(2) << std::setfill('0') << gmt.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << gmt.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << gmt.tm_sec << " "
		<< "GMT";
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
// waste
/*
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
		, const std::string& strNameTable
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
				"value REAL, "
				"fk_measurement INTEGER, "
				"CONSTRAINT cnstrnt "
				"FOREIGN KEY(fk_measurement) REFERENCES measurement(id)"
				");";
			break;
		} // eof IDR_VALUE_MEASUREMENT
		} // eof switch
		rc = execute(pdb, strSql, nullptr);
		return rc;
	}
	//************************************************************************
	//*                 insertTuple
	//************************************************************************
	int insertTuple(const UINT& resourceIdTable
		, const std::string& strNameTable
		, std::initializer_list<std::string>& list
	)
	{
		std::vector<std::string> vector_list{ list };
		switch (resourceIdTable)
		{
		case IDR_MEASUREMENT:
		{
			strSql = "INSERT INTO "
				+ strNameTable
				+ "(name) VALUES('"
				+ vector_list[0]
				+ "');";
			break;
		} // eof IDR_MEASUREMENT
		case IDR_VALUE_MEASUREMENT:
		{
			//insert into value_measurement(timestamp, value, fk_measurement) 
			//values('bla', 'bla', (select id from measurement where name = 'measurement1'));			
			strSql = "INSERT INTO "
				+ strNameTable
				+ "(timestamp, value, fk_measurement) VALUES('"
				+ vector_list[0] + "', "
				+ vector_list[1] + ", ("
				+ "SELECT id FROM measurement WHERE name='"
				+ vector_list[2] + "')"
				+ ");";
			break;
		} // eof IDR_VALUE_MEASUREMENT
		} // eof switch
		rc = execute(pdb, strSql, nullptr);
		return rc;
	}
	//************************************************************************
	//*                 closeDb
	//*
	//* not yet in use, because it is unclear where to close the database
	//************************************************************************
	int closeDb()
	{
		rc = sqlite3_close(pdb);
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
*/