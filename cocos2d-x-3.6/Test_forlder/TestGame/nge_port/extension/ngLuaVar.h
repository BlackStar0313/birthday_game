/*
 *  ngLuaVar.h
 *  NextGenEngine in SlamDunk
 *
 *  Created by Chen Zhen on 13-10-1.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#ifndef __ngLuaValue_h__
#define __ngLuaValue_h__

#include "../core/NGE_Types.h"

#define tid_ltoc(_tid) ((_tid) - 1)
#define tid_ctol(_tid) ((_tid) + 1)

/* forward declare lua_State. */
typedef struct lua_State lua_State;

#pragma mark - type define

/* type enum */
typedef enum {
    LUA_VARTYPE_UNDEFINED   = 0,    /* undefined value */
    LUA_VARTYPE_NULL        = 1,    /* null value, like undefined. */
    LUA_VARTYPE_BOOL        = 2,    /* boolean */
    LUA_VARTYPE_INT32       = 3,    /* int32 */
    LUA_VARTYPE_FLOAT       = 4,    /* float */
    LUA_VARTYPE_POINTER     = 5,    /* light pointer */
    LUA_VARTYPE_STRING      = 6,    /* string */
    LUA_VARTYPE_LVALUE      = 7,    /* lua value, may be userdata or table. */
} AS_Lua_VarType;

/* value union */
typedef union {

    boolean         as_bool;        /* LUA_VARTYPE_BOOL */
    int32           as_int32;       /* LUA_VARTYPE_INT32 */
    float           as_float;       /* LUA_VARTYPE_FLOAT */
    const void*     as_pointer;     /* LUA_VARTYPE_POINTER */
    const char*     as_string;      /* LUA_VARTYPE_STRING */
    
} AS_Lua_VarValue;

/* var struct */
typedef struct {
    AS_Lua_VarType  mType;
    AS_Lua_VarValue mValue;
} AS_Lua_Var;

class ngLuaVar;
class asLuaObject;
class ngStringV2;
/* 通过ngLuaVar获取到的userdata取得asLuaObject对象指针。 */
asLuaObject* luaVarToObject(const ngLuaVar& var);

#pragma mark - ngLuaVar
/*!
 @class ngLuaVar
 @abstract 处理lua变量和引擎C++变量的转换
 @discussion
 */
class ngLuaVar {
public:
    ngLuaVar();
    /*virtual */~ngLuaVar();
    
public:
    /* constructor */
    //ngLuaVar(const ngLuaVar* var);
    ngLuaVar(const ngLuaVar& var);
    
    /* C++ var */
    explicit ngLuaVar(boolean b);        /* boolean */
    explicit ngLuaVar(int32 i);          /* integer */
    explicit ngLuaVar(float f);          /* float */
    explicit ngLuaVar(const void* p);    /* light pointer */
    explicit ngLuaVar(const char* s);    /* string */
    
    /* lua var */
    explicit ngLuaVar(lua_State* l, int32 idx = -1);    /* default get value from stack top, not pop it. */
    
public:
    /* push lua var to lua stack. */
    void Push(lua_State* l) const;
    
public:
    /* getter */

    /* 获取var的lua“地址”(全局引用名)，保存到name。只在var为LUA_VARTYPE_LVALUE时有效。 */
    static void GetGlobalRefName(const ngLuaVar& var, ngStringV2& name);
    
    AS_Lua_VarType GetType() const;
    
    boolean AsBoolean() const;
    int32 AsInt32() const;
    float AsFloat() const;
    const void* AsPointer() const;
    const char* AsString() const;
    const void* AsUserData() const;

    /* operator */
    operator boolean() { return AsBoolean(); }
	operator int32() { return AsInt32(); }
	operator float() { return AsFloat(); }
	operator const void*() { return AsPointer(); }
	operator const char*() { return AsString(); }

    /* setter */
    void Copy(const ngLuaVar& var);

    /* operator */
	ngLuaVar& operator = (const ngLuaVar& var) { Copy(var); return *this; }

    /*! 获取内部保存的lua_State，在GetType() == LUA_VARTYPE_LVALUE时有效。 */
    lua_State* GetInternalState() const;

private:
    /* clear value, free buffer if need. */
    void Clear();

private:
    
    AS_Lua_Var m_var;
    char m_globalRefName[17];
};

#pragma mark - inline implement

inline AS_Lua_VarType ngLuaVar::GetType() const {
    return m_var.mType;
}

#pragma mark - ngLuaVarList

#include <nsl/ngArrayList.h>

/*!
 @class ngLuaVarList
 @abstract C/C++ -> lua 方法调用的参数列表
 @discussion 通过调用AddVar()构建
 */

class ngLuaVarList : protected ngArrayList {
public:
    ngLuaVarList();
    virtual ~ngLuaVarList();
    
public:
    /*!
     @function AddVar
     @param pVar ngLuaVar对象指针
     @result 返回自身的引用
     @abstract 添加ngLuaVar到list
     @discussion pVar指向的对象将交由list管理，最终释放
     */
    ngLuaVarList& AddVar(ngLuaVar* pVar);

    //<<
    ngLuaVarList& AddVar(boolean b);
    ngLuaVarList& AddVar(int32 i);
    ngLuaVarList& AddVar(float f);
    ngLuaVarList& AddVar(const void* p);
    ngLuaVarList& AddVar(const char* s);
    //>>

	/*!
     @function GetVar
     @param index 索引
     @abstract 获取index位置的ngLuaVar对象
     @discussion
     */
    ngLuaVar* GetVar(int32 index);
    const ngLuaVar* GetVar(int32 index) const;
    
	virtual uint32 Size() const { return ngArrayList::Size(); }

};

/* 提供一个空的ngLuaVar列表，用于空的lua方法调用，不要尝试为其增加参数(AddVar)。 */
static ngLuaVarList sSharedEmptyLuaVarList;

#define EmptyLuaVarList (sSharedEmptyLuaVarList)
//#define EmptyLuaVarList (ngLuaVarList().AddVar(NULL))

#pragma mark - ngLuaTable

class ngNOHashMap;
class ngIOHashMap;

/*!
 @class ngLuaTable
 @abstract 保存一个lua table的glbal引用，提供相应的方法访问lua table的数据
 @discussion
 */
class ngLuaTable {

public:
    /*!
     @function ngLuaTable
     @param var ngLuaVar实例，如果不是table类型，当前ngLuaTable置为无效。
     @abstract 构造方法，通过ngLuaVar创建
     @discussion
     */
    explicit ngLuaTable(const ngLuaVar* var);
    virtual ~ngLuaTable();
private:
    /*! 保护构造方法。 */
    ngLuaTable();

public:
    /*!
     @function IsAvailable
     @return 有效返回TRUE，否则返回FALSE
     @abstract 判断lua table是否有效
     @discussion 有效的定义：lua值为LUA_TTABLE类型
     */
    boolean IsAvailable();

    /*! 获取lua table数据 */
    ngLuaVar GetField(int32 nKey, boolean* ok = NULL);
    /*! 获取lua table数据 */
    ngLuaVar GetField(NGCSTR sKey, boolean* ok = NULL);

    /*!
     @function ToList
     @param list 输出参数，保存table数据，类型为ngLuaVar
     @return 输出成功返回TRUE，失败返回FALSE
     @abstract 将lua table数据以ngLuaVar的形式逐条输出到list
     @discussion list中的ngLuaVar需要在上层代码中销毁
     */
    boolean ToList(ngList& list);

    /*!
     @function ToHashMap
     @param hashmap 输出参数，保存table数据，类型为ngLuaVar，(key为字符串类型)
     @return 输出成功返回TRUE，失败返回FALSE
     @abstract 将lua table数据以ngLuaVar的形式逐条输出到hashmap
     @discussion hashmap中的ngLuaVar需要在上层代码中销毁
     */
    boolean ToHashMap(ngNOHashMap& hashmap);

    /*!
     @function ToHashMap
     @param hashmap 输出参数，保存table数据，类型为ngLuaVar，(key为整型)
     @return 输出成功返回TRUE，失败返回FALSE
     @abstract 将lua table数据以ngLuaVar的形式逐条输出到hashmap
     @discussion hashmap中的ngLuaVar需要在上层代码中销毁
     
        （该方法只输出key类型为LUA_TNUMBER的内容。）
     */
    boolean ToHashMap(ngIOHashMap& hashmap);

private:
    /*! 内部操作时，将global引用入栈 */
    void PushLuaVarInternal();

private:
    boolean m_available;

    lua_State* m_pLuaState;
};

#endif	//__ngLuaValue_h__
