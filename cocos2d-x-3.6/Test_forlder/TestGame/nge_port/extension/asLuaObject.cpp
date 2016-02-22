/*
 *  asLuaObject.cpp
 *  NextGenEngine from SlamDunk
 *
 *  Created by Chen Zhen on 13-10-27.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#include <extension/asLuaObject.h>
#include <lib/lua/lua.hpp>

//static char _global_name[17];   //this var is not safe in multi-thread environment. [zhen.chen]

void _as_lua_bind_metatable(lua_State* _ls, void* _obj, const char* _metatable) {
    __as_lua_bind_new_block(_obj);
    //    lua_pushvalue(_ls, -1); //make a ref copy, to register global value.
    lua_getglobal(_ls, _metatable);
    lua_setmetatable(_ls, -2);

    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)_obj);
    lua_setglobal(_ls, _global_name);
}

void _as_lua_metatable_call(lua_State* _ls, void* _obj, const char* _method, const ngLuaVarList& _var_list) {
    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)_obj);
    lua_getglobal(_ls, _global_name);

    NGASSERT(LUA_TUSERDATA == lua_type(_ls, -1));

    //lua_getmetatable(_ls, -1);
    lua_getfield(_ls, -1, _method);

    lua_pushvalue(_ls, -2); //self

    for (uint32 i = 0; i < _var_list.Size(); i++) {
        const ngLuaVar* pVar = _var_list.GetVar(i);
        pVar->Push(_ls);
    }

    
    if (lua_pcall(_ls, _var_list.Size() + 1, 0, NULL) != 0) {
        L_ERROR(_ls, "call function failed!~");
        lua_pop(_ls, 1);
        NGASSERT(0);
    }

    lua_pop(_ls, 1);    //userdata.
}

ngLuaVar _as_lua_metatable_call_for_result(lua_State* _ls, void* _obj, const char* _method, const ngLuaVarList& _var_list) {
    char _global_name[17];
    sprintf(_global_name, "%016llX", (uint64)_obj);
    lua_getglobal(_ls, _global_name);

    NGASSERT(LUA_TUSERDATA == lua_type(_ls, -1));

    //lua_getmetatable(_ls, -1);
    lua_getfield(_ls, -1, _method);
    

    lua_pushvalue(_ls, -2); //self

    for (uint32 i = 0; i < _var_list.Size(); i++) {
        const ngLuaVar* pVar = _var_list.GetVar(i);
        pVar->Push(_ls);
    }

    if (lua_pcall(_ls, _var_list.Size() + 1, 1, NULL) != 0) {
        L_ERROR(_ls, "call function failed!~");
        lua_pop(_ls, 1);
        NGASSERT(0);
    }

    ngLuaVar ret(_ls, -1);

    lua_pop(_ls, 2);    //userdata & result (ref in global).

    return ret;
}

#pragma mark - lua object interface

static const char* _lua_global_ref_format = "%016llX";

static int _lua_object_metatable_index(lua_State* _ls) {

    lua_getmetatable(_ls, 1);

    int _len = luaL_len(_ls, -1);
    boolean found = false;
    
    for (int32 i = _len; i >= 1; i--) { //1 is a nil table
        lua_rawgeti(_ls, -1, i);    //raw access, no metatable.

        lua_pushvalue(_ls, 2);
        lua_rawget(_ls, -2);

        if (!lua_isnil(_ls, -1)) {

            //cast pointer here
            as_lua_c_userdata* pBlock = (as_lua_c_userdata*)lua_touserdata(_ls, 1);
            lua_pushstring(_ls, "__cast");
            lua_rawget(_ls, -3);    //-2 here is ret value.
            lua_pushlightuserdata(_ls, pBlock->origin);
            lua_pcall(_ls, 1, 1, NULL);
            pBlock->cast = (void*)lua_topointer(_ls, -1);
            lua_pop(_ls, 1);

            lua_remove(_ls, -2);    //remove metatable(from rawgeti)
            found = TRUE;
            break;
        } else {
            lua_remove(_ls, -2);
            lua_pop(_ls, 1);    //pop nil value
        }
    }

    if(found){
        lua_remove(_ls, -2);    //remove metatable list.
        return 1;
    }else{
        return 0;
    }
}

static int _lua_object_metatable_newindex(lua_State* _ls) {
    lua_getmetatable(_ls, 1);

    int _len = luaL_len(_ls, -1);
    boolean found = false;
    for (int32 i = _len; i >= 1; i--) { //1 is a nil table
        lua_rawgeti(_ls, -1, i);    //raw access, no metatable.

        lua_pushvalue(_ls, 2);
        lua_rawget(_ls, -2);

        if (!lua_isnil(_ls, -1)) {

            lua_remove(_ls, -3);    //remove metatable list.
            lua_remove(_ls, -1);    //lua_pop(_ls, 1);

            lua_pushvalue(_ls, 2);
            lua_pushvalue(_ls, 3);
            lua_rawset(_ls, -3);    //ignore __index here.
            found = TRUE;
            break;
        } else {
            lua_remove(_ls, -2);    //remove metatable(from rawgeti)
            lua_pop(_ls, 1);    //pop nil value
        }
    }
    
    lua_pop(_ls, 1); //if not found, then pop the metatable list. if found, then pop the metatable-i
    
    if(!found){
        lua_getmetatable(_ls, 1);
        
        lua_rawgeti(_ls, -1, _len);
        
        lua_pushvalue(_ls, 2);
        lua_pushvalue(_ls, 3);
        lua_rawset(_ls, -3);
        
        lua_pop(_ls, 2);
    }

    return 0;
}

/* 返回C++指针 */
static int _lua_object_metatable_as_pointer(lua_State* _ls) {

    NGASSERT(LUA_TUSERDATA == lua_type(_ls, 1));

    as_lua_c_userdata* pBlock = (as_lua_c_userdata*)lua_touserdata(_ls, 1);

    lua_pushvalue(_ls, 1);
    lua_pushstring(_ls, "__cast");
    lua_gettable(_ls, -2);    //-2 here is ret value.
    lua_pushlightuserdata(_ls, pBlock->origin);
    lua_pcall(_ls, 1, 1, NULL);

    return 1;
}

asLuaObject::asLuaObject(lua_State* _ls)
: m_pUserData(NULL)
, m_pHostLuaState(_ls) {

    Bind(_ls);

}

asLuaObject::asLuaObject(lua_State* ls, boolean noBind)
: m_pHostLuaState(ls)
, m_pUserData(NULL){

}

asLuaObject::~asLuaObject() {

    Release(m_pHostLuaState);

}

void asLuaObject::Bind(lua_State* _ls) {

    NGASSERT(m_pUserData == NULL && "Bind() has been called!~");

    m_pUserData =
        (as_lua_c_userdata*)lua_newuserdata(_ls, sizeof(as_lua_c_userdata));
    m_pUserData->origin = this;
    m_pUserData->cast = this;

    lua_pushvalue(_ls, -1); //push ref to set global.
    char _global_name[17];
    sprintf(_global_name, _lua_global_ref_format, (uint64)this); //use 64bit pointer to make glboal idx to avoid name conflict.
    lua_setglobal(_ls, _global_name);

    /*
     初始化metatable
     asLuaObject的metatable维护一个metatable的列表，派生类通过SetMetadata或AS_LUA_METATABLE方式添加的metatable按照加载顺序存入。
     该metatable的"__index"触发时按照加载顺序遍历所有存在的metatable寻找匹配的方法。
     
        遍历顺序按照加载顺序的倒序，完整遍历，所以有如下覆盖关系：
     
            1. 父类的metatable定义的方法会被子类覆盖
            2. 如果有多个父类带有metatable并且定义了同名方法，后载入的(方法)将会覆盖先载入的(方法)。

     */
    lua_newtable(_ls);

    /* push a nil value as start of metatable list. */
    /*
    lua_pushinteger(_ls, 1);
    lua_pushnil(_ls);
    lua_settable(_ls, -3);
     */

    lua_pushstring(_ls, "__index");
    lua_pushcfunction(_ls, _lua_object_metatable_index);
    lua_settable(_ls, -3);

    lua_pushstring(_ls, "__newindex");
    lua_pushcfunction(_ls, _lua_object_metatable_newindex);
    lua_settable(_ls, -3);

    /* basic metatable, provide basic interface. */
    //<<
    lua_newtable(_ls);
    lua_pushstring(_ls, "AsPointer");
    lua_pushcfunction(_ls, _lua_object_metatable_as_pointer);
    lua_settable(_ls, -3);

    lua_pushinteger(_ls, luaL_len(_ls, -2) + 1);
    lua_pushvalue(_ls, -2); //push basic metatable to top.
    lua_remove(_ls, -3);    //remove old basic metatable.
    lua_settable(_ls, -3);
    //>>

    lua_setmetatable(_ls, -2);

    lua_pop(_ls, 1);    //pop new userdata.
}

void asLuaObject::Release(lua_State* _ls) {

    lua_pushnil(_ls);
    char _global_name[17];
    sprintf(_global_name, _lua_global_ref_format, (uint64)this);
    lua_setglobal(_ls, _global_name);   //update global ref to nil.

    //SAFE_FREE(m_pUserData); //free block memory.
}

void asLuaObject::Invoke(lua_State* _ls, NGCSTR func, const ngLuaVarList& args) const {
    _as_lua_metatable_call(_ls,
                           (void*)this,
                           func,
                           args);
}

ngLuaVar asLuaObject::InvokeForResult(lua_State* _ls, NGCSTR func, const ngLuaVarList& args) const {
    return _as_lua_metatable_call_for_result(_ls,
                                             (void*)this,
                                             func,
                                             args);
}

void asLuaObject::PushLuaVal(lua_State* _ls) {
    char _global_name[17];
    sprintf(_global_name, _lua_global_ref_format, (uint64)this);
    lua_getglobal(_ls, _global_name);
}

void asLuaObject::GetRefName(ngString& refName) const{
    refName.Format(_lua_global_ref_format, (uint64)this);
}

void asLuaObject::SetMetatable(lua_State* _ls, NGCSTR table_name) {

    //1. push self
    PushLuaVal(_ls);

    //2. get metatable
    lua_getmetatable(_ls, -1);
    int _idx = luaL_len(_ls, -1) + 1;    //(len + 1) to new index.

    //3. push new metatable and dup it to a new table.
    lua_getglobal(_ls, "table_deep_copy");    //table duplicate funciton, make it as a internal function.
    lua_getglobal(_ls, table_name);
    lua_pcall(_ls, 1, 1, NULL);    //duplicate table as metatable.

    //4. set new metatable
    lua_pushinteger(_ls, _idx);
    lua_pushvalue(_ls, -2); //push metatable ref.
    lua_settable(_ls, -4);

    //5. pop metatable ref, metatable list & self
    lua_pop(_ls, 3);
}

void asLuaObject::SetDefaultMetatable(lua_State* _ls, NGCSTR table_name) {

    static const char* strDefaultMetatableFormat = "_%s_metatable";

    NGASSERT(table_name != NULL && strlen(table_name) > 0 && "[asLuaObject] incorrect table name!~");

    char* strName = (char*)DMALLOC(strlen(table_name) + 12);   //here 12 is enough to store format str.
    sprintf(strName, strDefaultMetatableFormat, table_name);

    SetMetatable(_ls, strName);

    SAFE_FREE(strName);
}

