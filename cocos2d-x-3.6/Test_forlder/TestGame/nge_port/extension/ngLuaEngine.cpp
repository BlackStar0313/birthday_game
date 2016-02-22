//
//  ngLuaEngine.cpp
//  pet
//
//  Created by chen on 12-4-19.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include <extension/ngLuaEngine.h>

#include <core/NGE_Macros.h>
#include <core/ngDevice.h>
#include <nsl/ngStringV2.h>
#include <nsl/ngHashMapIterator.h>
#include <nsl/ngLinkedList.h>
#include <io/ngFileManager.h>

#include <lib/lua/lua.hpp>

#if ENABLE_LUA_PACKAGE
#include <io/ngZip.h>
#endif

#define SAFE_CLOSE_STATE(st)    if (st) {           \
                                    lua_close(st);  \
                                }                   \
                                st = NULL;

#define LUA_FUNC_PARAM_TYPE_INTEGER         'd'
#define LUA_FUNC_PARAM_TYPE_FLOAT           'f'
#define LUA_FUNC_PARAM_TYPE_STRING          's'
#define LUA_FUNC_PARAM_TYPE_POINTER         'p'
#define LUA_FUNC_PARAM_TYPE_STACK_VALUE     'v'
#define LUA_FUNC_PARAM_TYPE_BOOLEAN         'b'
#define LUA_FUNC_PARAM_TYPE_GOLBAL_VALUE    'g'

#define CurLuaState m_pCurLuaState->m_st

#if defined (DEBUG_LOG) && (DEBUG_LUA_TOP)
#   define LUA_TOP_LOG(l, p)  printf("[lua] top: %d, at: %s\n", lua_gettop(l), #p);
#else
#   define LUA_TOP_LOG(l, p)
#endif

static const char* luaFileRoot = "script";  //relative by internal storage root.

int32 l_require(lua_State* l) { 

    NGCSTR fileName = lua_tolstring(l, 1, NULL);
    
//    NG_DEBUG_LOG("[lua] require file: %s \n", fileName);
    
    NGCSTR ext = strstr(fileName, ".lua");
    NGASSERT(ext != NULL);
#if ENABLE_LUA_COMPILE
    strcpy(ext, "._lc");
#endif
    NGASSERT(LuaEngine->GetCurrentState()->m_st == l);  //we just use a lua_State now.
    
    LuaEngine->LoadScript(fileName);
    
    return 1;
}

#ifndef DEBUG_LOG
int32 l_print(lua_State* l) {
    //do nothing, just replace lua default.
    return 0;
}
int32 l_print_r(lua_State* l) {
    //do nothing, just replace lua default.
    return 0;
}
#endif

int32 l_test_time(lua_State* l) {
    static clock_t t = 0;
    
    clock_t t_e = clock();
    
    NG_DEBUG_LOG("debug_log, %s, %0.2f", __FUNCTION__, (float)(t_e - t) / CLOCKS_PER_SEC);
    
    t = clock();
    
    return 0;
}

const LuaStateData* ngLuaEngine::NewState(NGCSTR name) {
    if (m_pStateMap == NULL) {
        m_pStateMap = DNEW(ngNOHashMap);
    }

    m_pCurLuaState = static_cast<LuaStateData*>(m_pStateMap->Get(name));
    if (m_pCurLuaState == NULL) {
        m_pCurLuaState = DNEW(LuaStateData);
        m_pCurLuaState->m_id = name;
        CurLuaState = luaL_newstate();
        
        if (CurLuaState) {
            luaL_openlibs(CurLuaState);

#if 1
            /* add an extra load path in '[internal root path].script'. */
            lua_getglobal(CurLuaState, "package");
            lua_pushstring(CurLuaState, "path");
            lua_gettable(CurLuaState, -2);
            NGCSTR l_path = lua_tostring(CurLuaState, -1);
            
            //temp app package path
            ngStringV2 strInternalRootPath;
            ngDevice::GetInstance()->GetFileWritePath(strInternalRootPath, "");

            ngStringV2 strLuaPath;
            strLuaPath.Format("%s/?.lua;%s", strInternalRootPath.GetCString(), l_path); //FIXME: check here '/' in WindowsPhone later. [zhen.chen]
            lua_pop(CurLuaState, 1);
            
            lua_pushstring(CurLuaState, "path");
            lua_pushstring(CurLuaState, strLuaPath.GetCString());
            lua_settable(CurLuaState, -3);
            
            lua_pop(CurLuaState, 1);
            
            NG_DEBUG_LOG("[lua] load path: %s\n", strLuaPath.GetCString());
#endif
            
            /* 
             replace lua require function.
             
             or should not replace require and set lua package.searchpath.
             
             http://www.lua.org/manual/5.2/manual.html#pdf-package.searchpath

             */
//            AppewndGlobalFunc("require", l_require);
#ifndef DEBUG_LOG
            AppendGlobalFunc("print", l_print);
            AppendGlobalFunc("print_r", l_print_r);
#endif
            AppendGlobalFunc("test_time", l_test_time);
            
            /* use global table __loaded_script__ store loaded script file, avoid reload. */
            NewTable();
            FinishNewTable("__loaded_script__");
            
            m_pStateMap->Put(name, m_pCurLuaState);
        } else {
            SAFE_DELETE(m_pCurLuaState);
            NGASSERT(0);
        }
    }
    
    LUA_TOP_LOG(CurLuaState, NewState)
    
    return m_pCurLuaState;
}

void ngLuaEngine::CloseState(NGCSTR name) {
    LuaStateData* st = static_cast<LuaStateData*>(m_pStateMap->Remove(name));
    if (st != NULL) {
        if (m_pCurLuaState == st) {
            m_pCurLuaState = NULL;
        }

        SAFE_CLOSE_STATE(st->m_st);
        SAFE_DELETE(st);
    }
}

void ngLuaEngine::CloseAllState() {
    Clear();
}

boolean ngLuaEngine::SetCurrentState(NGCSTR name) {
    m_pCurLuaState = static_cast<LuaStateData*>(m_pStateMap->Get(name));
    return (m_pCurLuaState != NULL);
}

boolean ngLuaEngine::LoadScript(NGCSTR fileName) {
    if (m_pCurLuaState == NULL) {
        return FALSE;
    }
    
#if ENABLE_LUA_COMPILE
    NGSTR ext = strstr(fileName, ".lua");
    if (ext != NULL) {
        strcpy(ext, "._lc");
    };
#endif

    ngStringV2 file;
    ngDevice::GetInstance()->GetFilePath(file, fileName);

    lua_getglobal(CurLuaState, "__loaded_script__");
    
//    int32 num = TableSize();
//    if (num > 0) {
        /* there is some loaded script. */

        GetTableField(fileName);
        if (!IsNilAt(-1)) {
            Pop(2);
            /* have loaded this file. */
            return TRUE;
        }
        Pop(1);
//    }

    NG_DEBUG_LOG("[lua] load script: %s\n", fileName);
    
    if (luaL_dofile(CurLuaState, file)) {
        L_ERROR(CurLuaState, "load script file failed!~");
        NGASSERT(0);
        return FALSE;
    }
    
    AppendIntegerField(fileName, 1);
    Pop(1);
    
    LUA_TOP_LOG(CurLuaState, LoadScript)

    return TRUE;
}

#if ENABLE_LUA_PACKAGE
#define LUA_PACKAGE_PWD "AnansiLua"

boolean ngLuaEngine::LoadPackage(NGCSTR package, NGCSTR index_script) {
    ngZip zip;
    if (!zip.OpenZipFile(package, LUA_PACKAGE_PWD)) return FALSE;

    ngLinkedList files;
    zip.Unzip(&files);
    zip.CloseZipFile();
    
    boolean ret = FALSE;
    
    NG_DEBUG_LOG("[lua] unpacked %d lua file.\n", files.Size());
    
    if (LoadScript(index_script)) {
        ret = TRUE;
        
        ngIterator* itr = files.Iterator();
        while (itr->HasNext()) {
            ngStringV2* fullFilePath = (ngStringV2*)itr->Next();
            ngFileManager::GetInstance()->RemoveFile(fullFilePath->GetCString());
            SAFE_DELETE(fullFilePath);
        }
        SAFE_DELETE(itr);
        files.Clear();
    }
    
    return ret;
}
#endif

void ngLuaEngine::AppendGlobalFunc(NGCSTR n, lua_CFunction f) {
    if (m_pCurLuaState == NULL) {
        NGASSERT(0);
        return;
    }
    
    lua_pushcfunction(CurLuaState, f);
    lua_setglobal(CurLuaState, n);

    LUA_TOP_LOG(CurLuaState, AppendGlobalFunc)
}

void ngLuaEngine::BeginInitLib() {
    LIST_CLEAR(&m_funcList, luaL_Reg);  //should use for to clear array list.
    SAFE_DELETE_ARRAY(m_pObjLib);
}

void ngLuaEngine::AppendLibFunction(NGCSTR n, lua_CFunction f) {
    luaL_Reg* pData = DNEW(luaL_Reg);
    pData->name = n;
    pData->func = (lua_CFunction)f;
    m_funcList.Add(pData);
}

void ngLuaEngine::InitLib(NGCSTR name) {
    if (m_pCurLuaState == NULL) {
        NGASSERT(m_pCurLuaState != 0);
        return;
    }
    SAFE_DELETE_ARRAY(m_pObjLib);
    m_pObjLib = DNEWARR(luaL_Reg, m_funcList.Size() + 1);
    for (int32 i = 0; i < m_funcList.Size(); i++) {
        memcpy(&m_pObjLib[i], m_funcList[i], sizeof(luaL_Reg));
        //NGDBGPRINTF("append func: %s\n", m_pObjLib[i].name);
    }
    
    m_pObjLib[m_funcList.Size()].name = NULL;
    m_pObjLib[m_funcList.Size()].func = NULL;

    luaL_newlib(CurLuaState, m_pObjLib);
    lua_setglobal(CurLuaState, name);

#if 0
    /**
     *  将obj指针存入metatable，使用 OBJ_POINTER_FIELD 索引
     */
    lua_getglobal(m_pLuaState, name);
    lua_pushlightuserdata(m_pLuaState, this);
    lua_setfield(m_pLuaState, -2, OBJ_POINTER_FIELD);
    lua_pop(m_pLuaState, -1);
#endif
    
//    m_libId = name;

    LUA_TOP_LOG(CurLuaState, InitLib)
}

boolean ngLuaEngine::AppendCallBack(lua_CFunction f, NGCSTR cbName) {
    if (m_pCurLuaState == NULL) {
        return FALSE;
    }
    
    lua_pushcfunction(CurLuaState, f);
    lua_setglobal(CurLuaState, cbName);

    LUA_TOP_LOG(CurLuaState, AppendCallBack)
    
    return TRUE;
}

boolean ngLuaEngine::CallFunc(NGCSTR name, NGCSTR paramFormat, ...) {
    
    lua_getglobal(CurLuaState, name);
    
    va_list args;
    va_start(args, paramFormat);
    FuncArgs(paramFormat, args);
    va_end(args);

    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 1);
        NGASSERT(0);
        return FALSE;
    }
    
    LUA_TOP_LOG(CurLuaState, CallFunc)
    
    return TRUE;
}

/*
//deprecated in cocos2dx port
boolean ngLuaEngine::CallFunc(NGCSTR name, ngLuaTypeParser* parser, NGCSTR paramFormat, ...) {
    //    if (strlen(paramFormat) <= 0) {
    //        return FALSE;
    //    }
    
    lua_getglobal(CurLuaState, name);
    
    va_list args;
    va_start(args, paramFormat);
    FuncArgs(paramFormat, args);
    va_end(args);
    
    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 1);
        NGASSERT(0);
        return FALSE;
    }
    
    parser->Parse(CurLuaState, -1);

    lua_pop(CurLuaState, m_retsNum);

    LUA_TOP_LOG(CurLuaState, CallFunc)

    return TRUE;
}
 */

boolean ngLuaEngine::CallFunc(NGCSTR name, NGCSTR paramFormat, va_list argv) {
    lua_getglobal(CurLuaState, name);
    
    FuncArgs(paramFormat, argv);
    
    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 1);
        NGASSERT(0);
        return FALSE;
    }
    
    LUA_TOP_LOG(CurLuaState, CallFunc)
    
    return TRUE;
}

boolean ngLuaEngine::CallLuaFunc(int32 retNum) {
    return CallLuaFunc(0, retNum);
}

boolean ngLuaEngine::CallLuaFunc(int32 argNum, int32 retNum) {
    if (lua_pcall(CurLuaState, argNum, retNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 1);
        NGASSERT(0);
        return FALSE;
    }
    
    LUA_TOP_LOG(CurLuaState, CallLuaFunc)
    
    return TRUE;
}

#if ENABLE_LUA_VAR
ngLuaVar ngLuaEngine::CallFuncR(NGCSTR name, NGCSTR paramFormat, ...) {
    lua_getglobal(CurLuaState, name);
    
    va_list args;
    va_start(args, paramFormat);
    FuncArgs(paramFormat, args);
    va_end(args);
    
    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 1);
        NGASSERT(0);
        return ngLuaVar();
    }
    
    ngLuaVar ret(CurLuaState, -1);
    lua_pop(CurLuaState, 1);
    LUA_TOP_LOG(CurLuaState, CallLuaFuncR)
    return ret;
}

ngLuaVar ngLuaEngine::CallFuncR(NGCSTR name, NGCSTR paramFormat, va_list argv) {
    if (CallFunc(name, paramFormat, argv)) {
        ngLuaVar ret(CurLuaState, -1);
        lua_pop(CurLuaState, 1);
        LUA_TOP_LOG(CurLuaState, CallLuaFuncR)
        return ret;
    }
    
    return ngLuaVar();
}

ngLuaVar ngLuaEngine::CallLuaFuncR(int32 retNum) {
    NGASSERT(retNum <= 1);  /* 只支持一个返回值 */
    if (CallLuaFunc(retNum)) {
        ngLuaVar ret(CurLuaState, -1);
        lua_pop(CurLuaState, 1);
        LUA_TOP_LOG(CurLuaState, CallLuaFuncR)
        return ret;
    }
    
    return ngLuaVar();
}

ngLuaVar ngLuaEngine::CallModuleFuncR(NGCSTR module, NGCSTR name, NGCSTR paramFormat, ...) {
    //NG_DEBUG_LOG("[ngLuaEngine] call %s.%s", module, name);

    lua_getglobal(CurLuaState, module);
    NGASSERT(lua_type(CurLuaState, -1) == LUA_TTABLE && "module must a table");
    lua_pushstring(CurLuaState, name);
    lua_gettable(CurLuaState, -2);
    NGASSERT(lua_type(CurLuaState, -1) == LUA_TFUNCTION && "[module].[name] must point to a function.");

    va_list args;
    va_start(args, paramFormat);
    FuncArgs(paramFormat, args);
    va_end(args);

    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 2);
        NGASSERT(0);
        return ngLuaVar();
    }

    ngLuaVar ret(CurLuaState, -1);
    lua_pop(CurLuaState, 2);
    LUA_TOP_LOG(CurLuaState, CallLuaFuncR)

    return ret;
}

ngLuaVar ngLuaEngine::CallModuleFuncR(NGCSTR module, NGCSTR name, NGCSTR paramFormat, va_list argv) {
    NG_DEBUG_LOG("[ngLuaEngine] call %s.%s", module, name);

    lua_getglobal(CurLuaState, module);
    NGASSERT(lua_type(CurLuaState, -1) == LUA_TTABLE && "module must a table");
    lua_pushstring(CurLuaState, name);
    lua_gettable(CurLuaState, -2);
    NGASSERT(lua_type(CurLuaState, -1) == LUA_TFUNCTION && "[module].[name] must point to a function.");

    FuncArgs(paramFormat, argv);

    if (lua_pcall(CurLuaState, m_argsNum, m_retsNum, NULL) != 0) {
        L_ERROR(CurLuaState, "call function failed!~");
        lua_pop(CurLuaState, 2);
        NGASSERT(0);
        return ngLuaVar();
    }

    ngLuaVar ret(CurLuaState, -1);
    lua_pop(CurLuaState, 2);
    LUA_TOP_LOG(CurLuaState, CallLuaFuncR)

    return ret;
}

#endif

void ngLuaEngine::FuncArgs(NGCSTR format, va_list argv) {
    m_argsNum = 0;
    m_retsNum = 0;

    int32 i = 0;
    
    /*
     *  use last char to mark return num, [Optional].
     *  if format is empty, or not mark ret num, m_retsNum will be 0.
     */
    int32 paramNum = strlen(format);
    while (i < paramNum) {
        int32 inc = 1;
        switch (format[i]) {
            case LUA_FUNC_PARAM_TYPE_FLOAT:
                lua_pushnumber(CurLuaState, va_arg(argv, double));
                break;
            case LUA_FUNC_PARAM_TYPE_INTEGER:
                lua_pushinteger(CurLuaState, va_arg(argv, int32));
                break;
            case LUA_FUNC_PARAM_TYPE_STRING:
                lua_pushstring(CurLuaState, va_arg(argv, NGCSTR));
                break;
            case LUA_FUNC_PARAM_TYPE_POINTER:
                lua_pushlightuserdata(CurLuaState, va_arg(argv, void*));
                break;
            case LUA_FUNC_PARAM_TYPE_STACK_VALUE:
                lua_pushvalue(CurLuaState, va_arg(argv, int32));
                break;
            case LUA_FUNC_PARAM_TYPE_BOOLEAN:
                lua_pushboolean(CurLuaState, va_arg(argv, int /*boolean*/));
                break;
            case LUA_FUNC_PARAM_TYPE_GOLBAL_VALUE:
                lua_getglobal(CurLuaState, va_arg(argv, NGCSTR));
                break;
            default:
                m_retsNum = (int32)format[i] - (int32)'0';
                if (m_retsNum < 0 || m_retsNum > LUA_MAX_RETURN_NUM) {
                    NGDBGPRINTF("[lua] incurrect func format: %c", format[i]);
                    NGASSERT(0);
                }
                
                inc = m_retsNum;
                break;
        }
        i += inc;
    }
    
    m_argsNum = i - m_retsNum;
}

void* ngLuaEngine::GetPointerAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return NULL;
    }

    LUA_TOP_LOG(CurLuaState, GetPointerAt)

    return (void*)lua_topointer(CurLuaState, index);
}

int32 ngLuaEngine::GetIntAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return 0;
    }
    
    LUA_TOP_LOG(CurLuaState, GetIntAt)
    
    return lua_tointeger(CurLuaState, index);
}

float ngLuaEngine::GetFloatAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return 0;
    }
    
    LUA_TOP_LOG(CurLuaState, GetFloatAt)
    
    return lua_tonumber(CurLuaState, index);
}

NGCSTR ngLuaEngine::GetStringAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return "";
    }
    
    LUA_TOP_LOG(CurLuaState, GetStringAt)
    
    return lua_tostring(CurLuaState, index);
}

boolean ngLuaEngine::GetBooleanAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return FALSE;
    }
    
    LUA_TOP_LOG(CurLuaState, GetBooleanAt)
    
    return lua_toboolean(CurLuaState, index);
}

void ngLuaEngine::GetVauleAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return;
    }
    
    LUA_TOP_LOG(CurLuaState, GetVauleAt);
    
    lua_pushvalue(CurLuaState, index);
}

void ngLuaEngine::GetGlobal(NGCSTR name) const {
    if (m_pCurLuaState == NULL) {
        return;
    }
    
    lua_getglobal(CurLuaState, name);

    LUA_TOP_LOG(CurLuaState, GetGlobal)
    
}

boolean ngLuaEngine::IsNilAt(int32 index) const {
    if (m_pCurLuaState == NULL) {
        return TRUE;
    }
    
    return lua_isnil(CurLuaState, index);
}

int32 ngLuaEngine::_return_int32_() {
    int32 ret = get_int32_at(CurLuaState, -1);
    lua_pop(CurLuaState, 1);

    LUA_TOP_LOG(CurLuaState, _return_int32_)
    
    return ret;
}

uint32 ngLuaEngine::_return_uint32_() {
    uint32 ret = get_uint32_at(CurLuaState, -1);
    lua_pop(CurLuaState, 1);
    
    LUA_TOP_LOG(CurLuaState, _return_uint32_)
    
    return ret;
}

float ngLuaEngine::_return_float_() {
    float ret = get_float_at(CurLuaState, -1);
    lua_pop(CurLuaState, 1);
    
    LUA_TOP_LOG(CurLuaState, _return_float_)
    
    return ret;
}

boolean ngLuaEngine::_return_boolean_() {
    boolean ret = get_boolean_at(CurLuaState, -1);
    lua_pop(CurLuaState, 1);
    
    LUA_TOP_LOG(CurLuaState, _return_boolean_)
    
    return ret;
}

#if ENABLE_LUA_VAR
ngLuaVar ngLuaEngine::GetVarAt(int32 idx) const {
    return ngLuaVar(CurLuaState, idx);
}
#endif

void* ngLuaEngine::get_pointer_at(lua_State* l, int32 index) {
    LUA_TOP_LOG(l, get_pointer_at)
    return (void*)lua_topointer(l, index);
}

int32 ngLuaEngine::get_int32_at(lua_State* l, int32 index) {
    LUA_TOP_LOG(l, get_int32_at)
    return lua_tointeger(l, index);
}

uint32 ngLuaEngine::get_uint32_at(lua_State* l, int32 index) {
    LUA_TOP_LOG(l, get_uint32_at)
    return lua_tounsigned(l, index);
}

float ngLuaEngine::get_float_at(lua_State* l, int32 index) {
    LUA_TOP_LOG(l, get_float_at)
    return lua_tonumber(l, index);
}

boolean ngLuaEngine::get_boolean_at(lua_State* l, int32 index) {
    LUA_TOP_LOG(l, get_boolean_at)
    return lua_toboolean(l, index);
}

void ngLuaEngine::push_pointer(lua_State* l, void* p) {
    lua_pushlightuserdata(l, p);
    LUA_TOP_LOG(l, push_pointer)
}

void ngLuaEngine::push_int32_(lua_State* l, int32 n) {
    lua_pushinteger(l, n);
    LUA_TOP_LOG(l, push_int32_)
}

void ngLuaEngine::push_float_(lua_State* l, float f) {
    lua_pushnumber(l, f);
    LUA_TOP_LOG(l, push_float_)
}

void ngLuaEngine::push_boolean_(lua_State* l, boolean b) {
    lua_pushboolean(l, b);
    LUA_TOP_LOG(l, push_boolean_)
}

void ngLuaEngine::push_NGCSTR_(lua_State* l, NGCSTR str) {
    lua_pushstring(l, str);
    LUA_TOP_LOG(l, push_NGCSTR_);
}

#if ENABLE_LUA_VAR
ngLuaVar ngLuaEngine::get_var_at(lua_State* l, int32 idx) {
    LUA_TOP_LOG(l, get_var_at)
    return ngLuaVar(l, idx);
}

void ngLuaEngine::push_var(lua_State* l, const ngLuaVar& var) {
    var.Push(l);
    LUA_TOP_LOG(l, push_var)
}
#endif

void ngLuaEngine::InitLuaLibs() {
    for (int32 i = 0; i < m_libInitializerList.Size(); i++) {
        lua_lib_initializer initializer = (lua_lib_initializer)m_libInitializerList[i];
        initializer();
    }
}

void ngLuaEngine::NewTable() {
    lua_State* l = CurLuaState;

    /* lua stack size check. */
    NGASSERT(top() >= 0);

    lua_newtable(l);
    
    LUA_TOP_LOG(CurLuaState, NewTable)
}

void ngLuaEngine::AppendIntegerField(NGCSTR n, int32 nValue) {
    lua_State* l = CurLuaState;
    
    lua_pushstring(l, n);
    lua_pushinteger(l, nValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendIntegerField)
}

void ngLuaEngine::AppendNumberField(NGCSTR n, float fValue) {
    lua_State* l = CurLuaState;
    
    lua_pushstring(l, n);
    lua_pushnumber(l, fValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendNumberField)
}

void ngLuaEngine::AppendStringField(NGCSTR n, NGCSTR sValue) {
    lua_State* l = CurLuaState;
    
    lua_pushstring(l, n);
    lua_pushstring(l, sValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendStringField)
}

void ngLuaEngine::AppendTableField(NGCSTR n, NGCSTR tValueName) {
    lua_State* l = CurLuaState;
    
    lua_pushstring(l, n);
    lua_getglobal(l, tValueName);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendTableField)
}

void ngLuaEngine::AppendValueField(NGCSTR n, int32 idx) {
    lua_State* l = CurLuaState;
    
    lua_pushstring(l, n);
    lua_pushvalue(l, idx - 1);
    lua_settable(l, -4);
    LUA_TOP_LOG(CurLuaState, AppendValueField)
}

void ngLuaEngine::AppendIntegerField(int32 n, int32 nValue) {
    lua_State* l = CurLuaState;
    
    lua_pushinteger(l, n);
    lua_pushinteger(l, nValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendIntegerField)
}

void ngLuaEngine::AppendNumberField(int32 n, float fValue) {
    lua_State* l = CurLuaState;
    
    lua_pushinteger(l, n);
    lua_pushnumber(l, fValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendNumberField)
}

void ngLuaEngine::AppendStringField(int32 n, NGCSTR sValue) {
    lua_State* l = CurLuaState;
    
    lua_pushinteger(l, n);
    lua_pushstring(l, sValue);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendStringField)
}

void ngLuaEngine::AppendTableField(int32 n, NGCSTR tValueName) {
    lua_State* l = CurLuaState;
    
    lua_pushinteger(l, n);
    lua_getglobal(l, tValueName);
    lua_settable(l, -3);
    LUA_TOP_LOG(CurLuaState, AppendTableField)
}

void ngLuaEngine::AppendValueField(int32 n, int32 idx) {
    lua_State* l = CurLuaState;
    
    lua_pushinteger(l, n);
    lua_pushvalue(l, idx - 1);
    lua_settable(l, -4);
    LUA_TOP_LOG(CurLuaState, AppendValueField)
}

void ngLuaEngine::FinishNewTable(NGCSTR n) {
    lua_State* l = CurLuaState;
    lua_setglobal(l, n);
//    lua_pop(l, 1);
    LUA_TOP_LOG(CurLuaState, FinishNewTable)
}

void ngLuaEngine::GetTableField(NGCSTR field) {
    lua_State* l = CurLuaState;
    
    if (!lua_istable(l, -1)) {
        lua_pushnil(l);
        return;
    }
    
#if 0
    lua_pushstring(CurLuaState, field);
    lua_gettable(CurLuaState, -2); //or use lua_getfield()
#else
    
    int32 top = lua_gettop(l);
    
    lua_pushnil(l);
    while (lua_next(l, -2) != 0) {
        NGCSTR key = lua_tostring(l, -2);
        if (strcmp(key, field) == 0) {
            lua_insert(l, -2);
            lua_pop(l, 1);
            break;
        } else {
            lua_pop(l, 1);
        }
    }
    
    if (top == lua_gettop(l)) {
        lua_pushnil(l);
    }
#endif
    
    LUA_TOP_LOG(CurLuaState, GetTableField)
}

void ngLuaEngine::GetTableField(int32 index) {
    lua_State* l = CurLuaState;

    if (!lua_istable(l, -1)) {
        lua_pushnil(l);
        return;
    }
    
#if 1
    lua_pushinteger(CurLuaState, index);
    lua_gettable(CurLuaState, -2);
#else
    
    
    int32 top = lua_gettop(l);
    
    lua_pushnil(l);
    while (lua_next(l, -2) != 0) {
        int32 key = lua_tointeger(l, -2);
        if (key == index) {
            lua_insert(l, -2);
            lua_pop(l, 1);
            break;
        } else {
            lua_pop(l, 1);
        }
    }
    
    if (top == lua_gettop(l)) {
        lua_pushnil(l);
    }
#endif

    LUA_TOP_LOG(CurLuaState, GetTableField)
}

int32 ngLuaEngine::TableSize() {
    int32 len = 0;
    if (lua_istable(CurLuaState, -1)) {
        len = luaL_len(CurLuaState, -1);
    }
    LUA_TOP_LOG(CurLuaState, TableSize)
    return len;
}

void ngLuaEngine::Pop(int32 num) const {
    lua_pop(CurLuaState, num);
    LUA_TOP_LOG(CurLuaState, Pop)
    
    /* lua stack size check. */
    NGASSERT(top() >= 0);
}

int32 ngLuaEngine::top() const {
    return lua_gettop(CurLuaState);
}

void ngLuaEngine::Initialize() {
    m_pCurLuaState = NULL;
    
    m_pObjLib = NULL;
    m_pStateMap = DNEW(ngNOHashMap);
}

void ngLuaEngine::Clear() {
    ngHashMapIterator* itr = (ngHashMapIterator*)m_pStateMap->Iterator();
	while (itr->HasNext()) {
		ngHashMap::ngEntry *pEntry = itr->NextEntry();
		LuaStateData* st = (LuaStateData*)pEntry->GetValue();
        SAFE_CLOSE_STATE(st->m_st);
		SAFE_DELETE(st);
	}
	SAFE_DELETE(itr);
	
	SAFE_DELETE(m_pStateMap);
    
    m_pCurLuaState = NULL;
}

ngLuaEngine* ngLuaEngine::m_pThis = NULL;

ngLuaEngine* ngLuaEngine::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(ngLuaEngine);
    }
    return m_pThis;
}

void ngLuaEngine::ReleaseInstance(void *pObj) {
    ngLuaEngine* engine = static_cast<ngLuaEngine*>(pObj);
    DELETE_OBJECT(engine, ngLuaEngine);
}

ngLuaEngine::ngLuaEngine()
: ngSingleton(ReleaseInstance, this) {
    
    Initialize();
    
}

ngLuaEngine::~ngLuaEngine() {
    
    m_pThis = NULL; 

    Clear();
}

