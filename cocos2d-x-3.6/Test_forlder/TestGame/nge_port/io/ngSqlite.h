/*
 *  ngSqlite.h
 *  NextGenEngine
 *
 *  Created by Rui Xie 2015-01-29
 *  Copyright 2015 Anansi. All rights reserved.
 *
 */

#ifndef __NGSQLITE_H__
#define __NGSQLITE_H__

#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>
#include <nsl/ngString.h>

#include <io/sqlite3/sqlite3.h>

//typedef int (*sqlite3_callback)(
//void*,    /* Data provided in the 4th argument of sqlite3_exec() */
//int,      /* The number of columns in row */
//char**,   /* An array of strings representing fields in the row */
//char**    /* An array of strings representing column names */
//);

#define QUERY_SELECTOR(_CALLBACK) (sqlite3_callback)(&_CALLBACK)

class ngSqlVar;

class ngSqlite
{
public:
	ngSqlite(NGCSTR dbname);
	~ngSqlite();
    
    int32 ExcSQL(NGCSTR sql, sqlite3_callback pCallback=NULL);
    
    int32 CloseDB();
    
protected:
    int32 OpenDB();
    
private:
    ngString m_dbname;
    sqlite3 *m_pDB;
    sqlite3_callback m_pCallback;
    sqlite3_stmt *m_pStmt;
};

#endif