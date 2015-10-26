#ifndef DBCOMMON_H
#define DBCOMMON_H

//数据库类型
enum enDB_TYPE
{
	DBTYPE_MYSQL	= 1,		//MySQL
	DBTYPE_PGSQL	= 2,		//PostgressSQL
	DBTYPE_SQLITE	= 3,		//SQLite
	DBTYPE_SQLSERVER= 4,		//SQLServer
};

#endif
