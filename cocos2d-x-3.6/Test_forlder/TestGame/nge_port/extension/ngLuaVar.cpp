/*
 *  ngLuaVar.cpp
 *  NextGenEngine in SlamDunk
 *
 *  Created by Chen Zhen on 13-10-1.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#include <extension/ngLuaVar.h>
#include <string.h>
#include <core/NGE_Macros.h>

#include <lib/lua/lua.hpp>

#include <extension/asLuaObject.h>

asLuaObject* luaVarToObject(const ngLuaVar& var) {
    if (var.GetType() != LUA_VARTYPE_LVALUE) {
        return NULL;
    }

    lua_State* _ls = var.GetInternalState();
    var.Push(_ls);

    as_lua_c_userdata* pBlock = (as_lua_c_userdata*)lua_touserdata(_ls, -1);
    asLuaObject* pObj = (asLuaObject*) pBlock->origin;
    lua_pop(_ls, 1);

    return pObj;
}

static void make_string_value(AS_Lua_Var& var, const char* str) {
    if (str != NULL) {
        //this copy is not good.
        //m_var.mValue.as_string = strdup(str);
        
        int32 len = strlen(str);
        var.mValue.as_string = (const char*)DMALLOC(len + 1);  //last char is must '\0'
        ((char*)var.mValue.as_string)[len] = '\0';

        memcpy((void*)(var.mValue.as_string), str, len);
    
    } else {
        var.mValue.as_string = NULL;
    }
}

ngLuaVar::ngLuaVar() {
    m_var.mType = LUA_VARTYPE_UNDEFINED;
    memset(&m_var.mValue, 0x0, sizeof(m_var.mValue));
}

ngLuaVar::~ngLuaVar() {
    Clear();
}

ngLuaVar::ngLuaVar(const ngLuaVar& var) {
    Copy(var);
}

ngLuaVar::ngLuaVar(boolean b) {
    m_var.mType = LUA_VARTYPE_BOOL;
    m_var.mValue.as_bool = b;
}

ngLuaVar::ngLuaVar(int32 i) {
    m_var.mType = LUA_VARTYPE_INT32;
    m_var.mValue.as_int32 = i;
}

ngLuaVar::ngLuaVar(float f) {
    m_var.mType = LUA_VARTYPE_FLOAT;
    m_var.mValue.as_float = f;
}

ngLuaVar::ngLuaVar(const void* p) {
    m_var.mType = LUA_VARTYPE_POINTER;
    m_var.mValue.as_pointer = p;
}

ngLuaVar::ngLuaVar(const char* s) {
    m_var.mType = LUA_VARTYPE_STRING;
    make_string_value(m_var, s);
}

ngLuaVar::ngLuaVar(lua_State* l, int32 idx) {

    int32 var_type = lua_type(l, idx);
    switch (var_type) {
        case LUA_TNIL:
        case LUA_TNONE:
            m_var.mType = LUA_VARTYPE_NULL;
            memset(&m_var.mValue, 0x0, sizeof(m_var.mValue));
            break;
        case LUA_TBOOLEAN:
            m_var.mType = LUA_VARTYPE_BOOL;
            m_var.mValue.as_bool = lua_toboolean(l, idx);
            break;
        case LUA_TNUMBER:   /* int32 or float both here. */
            m_var.mType = LUA_VARTYPE_FLOAT;
            m_var.mValue.as_float = lua_tonumber(l, idx);
            break;
        case LUA_TLIGHTUSERDATA:
            m_var.mType = LUA_VARTYPE_POINTER;
            m_var.mValue.as_pointer = lua_topointer(l, idx);
            break;
        case LUA_TSTRING:
            m_var.mType = LUA_VARTYPE_STRING;
            make_string_value(m_var, lua_tostring(l, idx));
            break;
        case LUA_TUSERDATA:
        case LUA_TTABLE:
        {
            m_var.mType = LUA_VARTYPE_LVALUE;
            m_var.mValue.as_pointer = l;
            lua_pushvalue(l, idx);

            char _global_name[17];
            sprintf(_global_name, "%016llX", (uint64)this);
            lua_setglobal(l, _global_name);
        }
        default:
            break;
    }

}

void ngLuaVar::GetGlobalRefName(const ngLuaVar& var, ngStringV2& name) {
    NGASSERT(var.GetType() == LUA_VARTYPE_LVALUE);

    name.Clear();
    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)&var);
    name = _global_name;
}

void ngLuaVar::Push(lua_State* l) const {
    switch (m_var.mType) {
        case LUA_VARTYPE_BOOL:
            lua_pushboolean(l, m_var.mValue.as_bool);
            break;
        case LUA_VARTYPE_INT32:
            lua_pushinteger(l, m_var.mValue.as_int32);
            break;
        case LUA_VARTYPE_FLOAT:
            lua_pushnumber(l, m_var.mValue.as_float);
            break;
        case LUA_VARTYPE_POINTER:
            lua_pushlightuserdata(l, (void*)m_var.mValue.as_pointer);
            break;
        case LUA_VARTYPE_STRING:
            lua_pushstring(l, m_var.mValue.as_string);
            break;
        case LUA_VARTYPE_LVALUE:
        {
            char _global_name[17];
            sprintf(_global_name, "%016llX", (uint64)this);
            lua_getglobal(l, _global_name);
        }
            break;
        case LUA_VARTYPE_NULL:
        case LUA_VARTYPE_UNDEFINED:
        default:
            lua_pushnil(l);
            break;
    }
}

#pragma mark - getter

#if defined (DEBUG_LOG)
#   define DEBUG_TYPE_CHECK(_var, _type)                                                                    \
        if (_var.mType != _type) {                                                                          \
            NG_DEBUG_LOG("[LUA_VAR] type check failed! the type(%d) is not %s\n", _var.mType, #_type);      \
            NGASSERT(0); \
        }

#else

#   define DEBUG_TYPE_CHECK(_var, _type)   //nothing here.

#endif

boolean ngLuaVar::AsBoolean() const {
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_BOOL);
    return m_var.mValue.as_bool;
}

int32 ngLuaVar::AsInt32() const {
#if 0
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_INT32);
    return m_var.mValue.as_int32;
#else
    if (m_var.mType == LUA_VARTYPE_INT32) {
        return m_var.mValue.as_int32;
    } else if (m_var.mType == LUA_VARTYPE_FLOAT) {
        return m_var.mValue.as_float;   /* cast to int32 */
    } else {
        DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_FLOAT);
        DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_INT32);
        return 0;   //default 0.
    }
#endif
}

float ngLuaVar::AsFloat() const {
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_FLOAT);
    return m_var.mValue.as_float;
}

const void* ngLuaVar::AsPointer() const {
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_POINTER);
    return m_var.mValue.as_pointer;
}

const char* ngLuaVar::AsString() const {
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_STRING);
    return m_var.mValue.as_string;
}

const void* ngLuaVar::AsUserData() const {
    DEBUG_TYPE_CHECK(m_var, LUA_VARTYPE_LVALUE);

    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)this);

    lua_State* l = (lua_State*)m_var.mValue.as_pointer;
    lua_getglobal(l, _global_name);

    if (LUA_TUSERDATA != lua_type(l, -1)) {
        lua_pop(l, 1);
        return NULL;
    } else {
        void* ret = lua_touserdata(l, -1);
        lua_pop(l, 1);
        return ret;
    }
}

void ngLuaVar::Copy(const ngLuaVar& var) {
    m_var.mType = var.m_var.mType;

    if (m_var.mType == LUA_VARTYPE_STRING) {
        make_string_value(m_var, var.m_var.mValue.as_string);
    } else if (m_var.mType == LUA_VARTYPE_LVALUE) {
        lua_State* l = (lua_State*)var.m_var.mValue.as_pointer;
        char _global_name[17];
        sprintf(_global_name, "%016llX", (uint64)&var);
        lua_getglobal(l, _global_name);
        sprintf(_global_name, "%016llX", (uint64)this);
        lua_setglobal(l, _global_name);

        //copy lua_State pointer.
        memcpy(&m_var.mValue, &var.m_var.mValue, sizeof(AS_Lua_VarValue));
    } else {
        memcpy(&m_var.mValue, &var.m_var.mValue, sizeof(AS_Lua_VarValue));
    }
}

lua_State* ngLuaVar::GetInternalState() const {
    if (GetType() == LUA_VARTYPE_LVALUE) {
        return (lua_State*)m_var.mValue.as_pointer;
    } else {
        return NULL;
    }
}

void ngLuaVar::Clear() {
    if (m_var.mType == LUA_VARTYPE_STRING) {
        void* pStrBuffer = (void*)(m_var.mValue.as_string);
        SAFE_FREE(pStrBuffer);
    } else if (m_var.mType == LUA_VARTYPE_LVALUE) {
        lua_State* l = (lua_State*)m_var.mValue.as_pointer;
        lua_pushnil(l);
        char _global_name[17];
        sprintf(_global_name, "%016llX", (uint64)this);
        lua_setglobal(l, _global_name);
    }
    
    m_var.mType = LUA_VARTYPE_UNDEFINED;
    memset(&m_var.mValue, 0x0, sizeof(m_var.mValue));
}

#pragma mark - ngLuaVarList

#include <nsl/ngArrayList.h>

ngLuaVarList::ngLuaVarList() {
    
}

ngLuaVarList::~ngLuaVarList() {
    for (int32 _i = 0; _i < Size(); _i++) {
        ngLuaVar* pVar = (ngLuaVar*)Get(_i);
        SAFE_DELETE(pVar);
    }
    Clear();
}

ngLuaVarList& ngLuaVarList::AddVar(ngLuaVar* pVar) {
    if (pVar) {
        Add(pVar);
    }
    
    return *this;
}

ngLuaVarList& ngLuaVarList::AddVar(boolean b) {
    Add(DNEW(ngLuaVar)(b));
    return *this;
}

ngLuaVarList& ngLuaVarList::AddVar(int32 i) {
    Add(DNEW(ngLuaVar)(i));
    return *this;
}

ngLuaVarList& ngLuaVarList::AddVar(float f) {
    Add(DNEW(ngLuaVar)(f));
    return *this;
}

ngLuaVarList& ngLuaVarList::AddVar(const void* p) {
    Add(DNEW(ngLuaVar)(p));
    return *this;
}

ngLuaVarList& ngLuaVarList::AddVar(const char* s) {
    Add(DNEW(ngLuaVar)(s));
    return *this;
}

ngLuaVar* ngLuaVarList::GetVar(int32 index) {
    return (ngLuaVar*)Get(index);
}

const ngLuaVar* ngLuaVarList::GetVar(int32 index) const {
    return (const ngLuaVar*)(*this)[index];
}

#pragma mark - ngLuaTable

#include <nsl/ngNOHashMap.h>
#include <nsl/ngIOHashMap.h>

ngLuaTable::ngLuaTable(const ngLuaVar* var) {
    if (var == NULL) {
        m_available = FALSE;
        return;
    } else if (var->GetType() != LUA_VARTYPE_LVALUE) {
        m_available = FALSE;
        return;
    }

    m_pLuaState = var->GetInternalState();
    NGASSERT(m_pLuaState != NULL);

    var->Push(m_pLuaState);

    if (lua_type(m_pLuaState, -1) != LUA_TTABLE) {
        m_available = FALSE;
        return;
    } else {
        /* 只在lua table值有效时才保存global引用。 */
        char _global_name[17];
        sprintf(_global_name, "%016llX", (uint64)this);
        lua_setglobal(m_pLuaState, _global_name);

        m_available = TRUE;
    }
}

ngLuaTable::~ngLuaTable() {
    if (m_available) {
        lua_pushnil(m_pLuaState);
        char _global_name[17];
        sprintf(_global_name, "%016llX", (uint64)this);
        lua_setglobal(m_pLuaState, _global_name);
    }
}

ngLuaTable::ngLuaTable()
: m_available(FALSE)
, m_pLuaState(NULL) {

}

boolean ngLuaTable::IsAvailable() {
    return m_available;
}

ngLuaVar ngLuaTable::GetField(int32 nKey, boolean* ok) {

    if (!m_available) {
        if (ok) {
            *ok = FALSE;
        }
        return ngLuaVar();
    }

    PushLuaVarInternal();

    lua_pushinteger(m_pLuaState, nKey);
    lua_gettable(m_pLuaState, -2);
    ngLuaVar ret(m_pLuaState, -1);

    lua_pop(m_pLuaState, -1); //pop lua table ref

    if (ok) {
        *ok = TRUE;
    }

    return ret;
}

ngLuaVar ngLuaTable::GetField(NGCSTR sKey, boolean* ok) {

    if (!m_available) {
        if (ok) {
            *ok = FALSE;
        }
        return ngLuaVar();
    }

    PushLuaVarInternal();

    lua_pushstring(m_pLuaState, sKey);
    lua_gettable(m_pLuaState, -2); //or use lua_getfield()
    ngLuaVar ret(m_pLuaState, -1);

    lua_pop(m_pLuaState, -1); //pop lua table ref

    if (ok) {
        *ok = TRUE;
    }

    return ret;
}

boolean ngLuaTable::ToList(ngList& list) {

    if (!m_available) {
        return FALSE;
    }

    NGASSERT(list.IsEmpty() && "list should be an empty list!~");

    PushLuaVarInternal();

    lua_pushnil(m_pLuaState);

    while (lua_next(m_pLuaState, -2) != 0) {
        ngLuaVar* pVar = DNEW(ngLuaVar)(m_pLuaState, -1);
        list.Add(pVar);
        lua_pop(m_pLuaState, 1);
    }

    lua_pop(m_pLuaState, 1); //pop lua table ref

    return TRUE;
}

boolean ngLuaTable::ToHashMap(ngNOHashMap& hashmap) {

    if (!m_available) {
        return FALSE;
    }

    NGASSERT(hashmap.Size() == 0 && "hashmap should be an empty map!~");

    PushLuaVarInternal();

    lua_pushnil(m_pLuaState);

    while (lua_next(m_pLuaState, -2) != 0) {

        ngStringV2 strKey;
        int32 keyType = lua_type(m_pLuaState, -2); //check key type

        if (keyType == LUA_TSTRING) {
            strKey = lua_tostring(m_pLuaState, -2);
        } else if (keyType == LUA_TNUMBER) {
            strKey.Format("%d", (int32)lua_tointeger(m_pLuaState, -2));
        } else {
            NGASSERT(0 && "unknown lua table key type!~");
        }

        ngLuaVar* pVar = DNEW(ngLuaVar)(m_pLuaState, -1);

        hashmap.Put(strKey, pVar);

        lua_pop(m_pLuaState, 1);
    }

    lua_pop(m_pLuaState, 1); //pop lua table ref

    return TRUE;
}

boolean ngLuaTable::ToHashMap(ngIOHashMap& hashmap) {

    if (!m_available) {
        return FALSE;
    }

    NGASSERT(hashmap.Size() == 0 && "hashmap should be an empty map!~");

    PushLuaVarInternal();

    lua_pushnil(m_pLuaState);

    while (lua_next(m_pLuaState, -2) != 0) {

        if (lua_type(m_pLuaState, -2) == LUA_TNUMBER) {
            int32 nKey = lua_tointeger(m_pLuaState, -2);
            ngLuaVar* pVar = DNEW(ngLuaVar)(m_pLuaState, -1);
            hashmap.Put(nKey, pVar);
        } else {
            NG_DEBUG_LOG("[ngLuaTable] non-num key found, skipped!~");
        }

        lua_pop(m_pLuaState, 1);
    }

    lua_pop(m_pLuaState, 1); //pop lua table ref
    
    return TRUE;
}

void ngLuaTable::PushLuaVarInternal() {
    NGASSERT(m_available);

    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)this);
    lua_getglobal(m_pLuaState, _global_name);
}


