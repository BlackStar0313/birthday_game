/*
 *  ngSqlite.cpp
 *  NextGenEngine
 *
 *  Created by Rui Xie 2015-01-29
 *  Copyright 2015 Anansi. All rights reserved.
 *
 */
#include <stdio.h>
#include <io/ngSqlite.h>
#include <core/ngDevice.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

ngSqlite::ngSqlite(NGCSTR dbname)
: m_pDB(NULL)
, m_pCallback(NULL)
, m_pStmt(NULL)
, m_dbname(dbname){
    
}

ngSqlite::~ngSqlite()
{
    CloseDB();
}

int32 ngSqlite::OpenDB()
{
    ngString path;
    ngDevice::GetInstance()->GetFileCachePath(path, m_dbname);
    
    int32 rc = sqlite3_open(path.GetCString(), &m_pDB);
    if(rc!=SQLITE_OK){
        NG_DEBUG_LOG("Error: Can't open database: %s\n", sqlite3_errmsg(m_pDB));
        sqlite3_close(m_pDB);

        m_pDB = NULL;
        return(1);
    }
    return 0;
}

int32 ngSqlite::ExcSQL(NGCSTR sql,sqlite3_callback pCallback)
{
    if (OpenDB()) {
        NG_DEBUG_LOG("Error: open db %s failed!!!\n", m_dbname.GetCString());
        return -1;
    }
    
    NG_DEBUG_LOG("SQL excuted : %s\n", sql);
    
    char* zErrMsg = NULL;
    int32 rc = 0;
    if (pCallback) {
        rc = sqlite3_exec(m_pDB, sql, pCallback, 0, &zErrMsg);
    } else {
        rc = sqlite3_exec(m_pDB, sql, callback, 0, &zErrMsg);
    }

    if(rc!=SQLITE_OK){
        NG_DEBUG_LOG("Error: SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        
        CloseDB();
        return 1;
    }
    
    CloseDB();
    return 0;
}

int32 ngSqlite::CloseDB()
{
    if (m_pDB) {
        sqlite3_close(m_pDB);
    }
    return 0;
}