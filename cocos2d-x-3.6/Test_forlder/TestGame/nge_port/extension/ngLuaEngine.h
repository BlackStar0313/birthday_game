//
//  ngLuaEngine.h
//  pet
//
//  Created by chen on 12-4-19.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifndef __NGLUAENGINE_H__
#define __NGLUAENGINE_H__

#include "../core/ngSingleton.h"
#include "../nsl/ngStringV2.h"
#include "../nsl/ngNOHashMap.h"
#include "../nsl/ngArrayList.h"
#include "../extension/ngLuaDefs.h"

#define ENABLE_LUA_COMPILE 0
#define ENABLE_LUA_PACKAGE 1

/* 
 
 ENABLE_LUA_VAR is a flag to use ngLuaVar to handle value transfer between lua and C++.
 
 some old push & get interface will be mark as deprecated.
 
 ngLuaVar.h & ngLuaVar.cpp is required.
 
 */
#define ENABLE_LUA_VAR 1

#if ENABLE_LUA_VAR
#   include "ngLuaVar.h"
#endif

#define LuaEngine   ngLuaEngine::GetInstance()

/* convenient define to get current state. */
#define lmainState (LuaEngine->GetCurrentState()->m_st)

#define AddLuaLibInitializer(func)      LuaEngine->AddLibInitilizer(func)
#define AddLuaObject(clazz)             LuaEngine->AddLibInitilizer(clazz::LUA_LIB_INIT_FUNC)

#define BindLuaGlobalFunc(n, f)         LuaEngine->AppendGlobalFunc((n), f)
#define BindLuaFunc(n, f)               LuaEngine->AppendLibFunction((n), f##L)

#define L_RETURN(t)   LuaEngine->_return_##t##_()

#define LUA_GET(d, f, t) \
    do {   \
        LuaEngine->GetTableField(f);    \
        if (!LuaEngine->IsNilAt(-1)) {   \
            d = L_RETURN(t);            \
        } else {                        \
            LuaEngine->Pop(1);          \
        }                               \
        /*LuaEngine->Pop(1);*/  \
    } while(0);

#define LUA_MAX_RETURN_NUM  5

#define DEBUG_LUA_TOP   0

#ifdef DEBUG_LOG
#   define L_ERROR(l, msg)     printf("[lua] %s, %s\n", (msg), lua_tostring((l), -1))
#else
#   define L_ERROR(l, msg)
#endif

typedef struct __lua_function_param__ {

} LuaFunctionParam;

typedef struct __lua_function_ret__ {
    
} LuaFunctionRet;

//typedef enum __lua_function_argv_type {
//    LUA_FUNC_PARAM_TYPE_INTEGER = 0,
//} LuaFunctionParamType;

typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;

typedef struct __LUA_STATE_DATA__ {
    ngStringV2  m_id;
    lua_State*  m_st;
} LuaStateData;

//same as lua.h
typedef int (*lua_CFunction) (lua_State *L);

typedef void (*lua_lib_initializer) (void);

class ngLuaEngine : public ngSingleton {
public:
    const LuaStateData* NewState(NGCSTR name);
    void CloseState(NGCSTR name);
    void CloseAllState();

    boolean SetCurrentState(NGCSTR name);
    LuaStateData* GetCurrentState() { return m_pCurLuaState; }
    
    /* 添加全局接口到lua */
    void AppendGlobalFunc(NGCSTR n, lua_CFunction f);
    
    /**
     *  添加lib到lua
     *  调用方式：
     *      LuaEngine->BeginInitLib();
     *      BindLuaFunc(<name>, <func>);    //LuaEngine->AppendLuaFunction(<name>, <func>);
     *      ...
     *      LuaEngine->InitLib(<name>);
     */
    void BeginInitLib();
    void AppendLibFunction(NGCSTR n, lua_CFunction f);
    void InitLib(NGCSTR name);
    
    boolean LoadScript(NGCSTR fileName);
    boolean LoadPackage(NGCSTR package, NGCSTR index_script);
    
    boolean AppendCallBack(lua_CFunction f, NGCSTR cbName);

    /**
     *  paramFormat 内容为'd'、'f'、's'、'p'、'v'、'b'、'g'组合，分别顺序表示后续参数的类型。
     *
     *  最后一位为返回值个数，可忽略，最大不能超过 LUA_MAX_RETURN_NUM。
     */
    /* 直接调用 */
    boolean CallFunc(NGCSTR name, NGCSTR paramFormat, ...);
    /* 通过ngLuaTypeParser解析返回值 */
    //boolean CallFunc(NGCSTR name, ngLuaTypeParser* parser, NGCSTR paramFormat, ...);//deprecated in cocos2dx port
    /* 被ngLuaObject::CallObject()调用 */
    boolean CallFunc(NGCSTR name, NGCSTR paramFormat, va_list argv);
    
    boolean CallLuaFunc(int32 retNum = 1);
    boolean CallLuaFunc(int32 argNum, int32 retNum);
    
#if ENABLE_LUA_VAR
    /**
     * 新的lua 函数调用方式，直接返回lua调用的返回值。
     * 只支持返回值数量为1的调用。
     */
    /* 直接调用 */
    ngLuaVar CallFuncR(NGCSTR name, NGCSTR paramFormat, ...);
    /* 被ngLuaObject::CallObject()调用 */
    ngLuaVar CallFuncR(NGCSTR name, NGCSTR paramFormat, va_list argv);

    ngLuaVar CallLuaFuncR(int32 retNum = 1);

    /* 调用module方法 */
    //<<
    ngLuaVar CallModuleFuncR(NGCSTR module, NGCSTR name, NGCSTR paramFormat, ...);
    ngLuaVar CallModuleFuncR(NGCSTR module, NGCSTR name, NGCSTR paramFormat, va_list argv);
    //>>
    
#endif
    
    void* GetPointerAt(int32 index) const NGE_DEPRECATED;      /* use GetVarAt(). */
    int32 GetIntAt(int32 index) const NGE_DEPRECATED;      /* use GetVarAt(). */
    float GetFloatAt(int32 index) const NGE_DEPRECATED;      /* use GetVarAt(). */
    NGCSTR GetStringAt(int32 index) const NGE_DEPRECATED;      /* use GetVarAt(). */
    boolean GetBooleanAt(int32 index) const NGE_DEPRECATED;      /* use GetVarAt(). */
    
    /* 将stack中的index处的值放到stack顶端(-1). */
    void GetVauleAt(int32 index) const;
    
    void GetGlobal(NGCSTR name) const;
    
    boolean IsNilAt(int32 index) const;

    int32 _return_int32_() NGE_DEPRECATED;      /* use GetVarAt(). */
    uint32 _return_uint32_() NGE_DEPRECATED;      /* use GetVarAt(). */
    float _return_float_() NGE_DEPRECATED;      /* use GetVarAt(). */
    boolean _return_boolean_() NGE_DEPRECATED;      /* use GetVarAt(). */
    
#if ENABLE_LUA_VAR
    ngLuaVar GetVarAt(int32 idx) const;
#endif

    /* stack */
    static void* get_pointer_at(lua_State* l, int32 index) NGE_DEPRECATED;      /* use get_var_at(). */
    static int32 get_int32_at(lua_State* l, int32 index) NGE_DEPRECATED;        /* use get_var_at(). */
    static uint32 get_uint32_at(lua_State* l, int32 index) NGE_DEPRECATED;      /* use get_var_at(). */
    static float get_float_at(lua_State* l, int32 index) NGE_DEPRECATED;        /* use get_var_at(). */
    static boolean get_boolean_at(lua_State* l, int32 index) NGE_DEPRECATED;    /* use get_var_at(). */
    
    static void push_pointer(lua_State* l, void* p) NGE_DEPRECATED;      /* use push_var(). */
    static void push_int32_(lua_State* l, int32 n) NGE_DEPRECATED;      /* use push_var(). */
    static void push_float_(lua_State* l, float f) NGE_DEPRECATED;      /* use push_var(). */
    static void push_boolean_(lua_State* l, boolean b) NGE_DEPRECATED;      /* use push_var(). */
    static void push_NGCSTR_(lua_State* l, NGCSTR str) NGE_DEPRECATED;      /* use push_var(). */
    
#if ENABLE_LUA_VAR
    static ngLuaVar get_var_at(lua_State* l, int32 idx);
    static void push_var(lua_State* l, const ngLuaVar& var);
#endif

    /* custom lua lib */
    void AddLibInitilizer(lua_lib_initializer f) { m_libInitializerList.Add((void*)f); }
    void CLearLibInitlizerList() { m_libInitializerList.Clear(); }
    void InitLuaLibs();
    
    /* create new table data, and append value */
    /* new */
    void NewTable();
    /* for hash */
    void AppendIntegerField(NGCSTR n, int32 nValue);
    void AppendNumberField(NGCSTR n, float fValue);
    void AppendStringField(NGCSTR n, NGCSTR sValue);
    void AppendTableField(NGCSTR n, NGCSTR tValueName);
    void AppendValueField(NGCSTR n, int32 idx);
    /* for array */
    void AppendIntegerField(int32 n, int32 nValue);
    void AppendNumberField(int32 n, float fValue);
    void AppendStringField(int32 n, NGCSTR sValue);
    void AppendTableField(int32 n, NGCSTR tValueName);
    void AppendValueField(int32 n, int32 idx);
    /* set table name, and pop */
    void FinishNewTable(NGCSTR n);
    
    /*
     * load table data, with field name or index
     *
     * should push a table on the stack first. use like: GetGlobal().
     */
    void GetTableField(NGCSTR field);
    void GetTableField(int32 index);
    int32 TableSize();
    
    void Pop(int32 num) const;
    int32 top() const;

    //TODO: more interface here
    
private:
    void Initialize();
    void Clear();
    
    /* call by CallFunc() */
    void FuncArgs(NGCSTR format, va_list argv);
    
private:
    LuaStateData*   m_pCurLuaState;
    ngNOHashMap*    m_pStateMap;
    
    int32 m_argsNum;
    int32 m_retsNum;
    
    ngArrayList m_funcList;
    luaL_Reg*   m_pObjLib;
    
    ngArrayList m_libInitializerList;

#pragma mark -
#pragma mark ngSingleton

public:
    static ngLuaEngine* GetInstance();
    static void ReleaseInstance(void *pObj);
    
private:
    ngLuaEngine();
    virtual ~ngLuaEngine();

private:
    static ngLuaEngine* m_pThis;

#pragma mark -
};

#endif  //__NGLUAENGINE_H__
