/*
 *  asLuaObject.h
 *  NextGenEngine from SlamDunk
 *
 *  Created by Chen Zhen on 13-10-27.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#ifndef __asLuaObject_h__
#define __asLuaObject_h__

#include "ngLuaEngine.h"
//typedef struct lua_State lua_State;

/* 在类声明中定义一个静态方法用于绑定相应的metatable。 */
#define as_lua_metatable(name)                                  \
    public:                                                     \
        static int _as_lua_bind_metatable_##name(lua_State*);

/* 类标志，表示该类有metatable导出，参数为所有为所有同为导出类的父类。 */
#define as_lua_class
/* 父类标志，表示需要导出响应父类方法(父类必须由as_lua_class修饰)，参数为父类名。 */
#define as_lua_super_class(_cls) public _cls

/* 成员方法标志（无参数返回值调用），表示该方法可在lua环境中调用(绑定在所属类的metatable中)。 */
#define as_lua_export(name)

/* 成员方法标志（getter，无参数，有返回值），表示该方法可在lua环境中调用(绑定在所属类的metatable中)。 */
#define as_lua_export_g_number
#define as_lua_export_g_integer
#define as_lua_export_g_boolean
#define as_lua_export_g_string
#define as_lua_export_g_pointer
#define as_lua_export_g(_userdata)

/* 成员方法标志（getter，单个参数，无返回值），表示该方法可在lua环境中调用(绑定在所属类的metatable中)。 */
#define as_lua_export_s_number
#define as_lua_export_s_integer
#define as_lua_export_s_boolean
#define as_lua_export_s_string
#define as_lua_export_s_pointer

/*
 成员方法标志（call，单个参数，无返回值），表示该方法可在lua环境中调用(绑定在所属类的metatable中)。
 
 TIP: 在Lua环境中可以使用:AsPointer()调用获取到C++指针，(该方法绑定在当前对象的basic metatable中)。
 */
#define as_lua_export_c(ret, ...)

/* 枚举类型绑定，支持递增类型枚举的绑定 */
#define as_lua_enum(_name)

#pragma mark - lua macro for bind export
/* 保存c++对象指针和类型转换信息(继承关系) */
typedef struct __AS_T_LUA_C_USERDATA__ {
    void* origin;   /* 原始指针 */
    void* cast;     /* 类型转换指针，默认为原始指针，在必要时候转换为父类指针(__index) */
} as_lua_c_userdata;

/* 创建一个新的userdata并保存c++对像指针 */
#define __as_lua_bind_new_block(_p)                                             \
do {                                                                            \
    as_lua_c_userdata* block =                                                  \
        (as_lua_c_userdata*)lua_newuserdata(_ls, sizeof(as_lua_c_userdata));    \
    block->origin = _p;                                                         \
    block->cast = _p;                                                           \
} while(0);

/* 将userdata中保存的c++对象指针进行类型转换 */
#define __as_lua_cast_block(_from, _to)                     \
    do {                                                    \
        if (LUA_TNIL != lua_type(l, -1)) {                  \
            as_lua_c_userdata* block =                      \
                (as_lua_c_userdata*)lua_touserdata(l, 1);   \
            block->cast =                                   \
                (_to*)(_from*)block->origin;                \
        }                                                   \
    } while(0);

/* 绑定接口 */
#define BIND_LUA_CLASS_METHOD(cls, mn)                  \
do {                                                    \
    lua_pushstring(_ls, #mn);                           \
    lua_pushcfunction(_ls, __l##cls##_method_##mn);     \
    lua_settable(_ls, -3);                              \
} while(0);

/* 绑定的接口的实现，开始行，包含c++指针的获取操作 */
#define __as_lua_bind_method_start(_cls_tag, _cls_name, _metatable_method)          \
    static int __l##_cls_name##_method_##_metatable_method(lua_State* l) {          \
        _cls_tag* _##_cls_name =                                                    \
            (_cls_tag*)((as_lua_c_userdata*)lua_touserdata(l, 1))->cast;

/* 绑定的接口的实现，结束行，包含返回值 */
#define __as_lua_bind_method_end(_ret)              \
        return _ret;                                \
    }


#pragma mark - misc
/* other lua interface proxy here. */

#define BindLuaMetatable(_ls, _obj, _metatable) \
    _as_lua_bind_metatable((_ls), (_obj), (_metatable))
#define CallLuaMetatableFunc(_ls, _obj, _method, _var_list) \
    Invoke((_ls), (_method), (_var_list))
#define CallLuaMetatableFuncForResult(_ls, _obj, _method, _var_list)  \
    InvokeForResult((_ls), (_method), (_var_list))

/*!
 @function _as_lua_bind_metatable
 @param _ls lua环境指针
 @param _obj C++对象
 @param _metatable lua元表名
 @abstract 为一个C++对象创建userdata并绑定metatable，并且在lua环境中注册全局副本
 @discussion
 */
void _as_lua_bind_metatable(lua_State* _ls, void* _obj, const char* _metatable);

/*!
 @function _as_lua_metatable_call
 @param _ls lua环境指针
 @param _obj C++对象
 @param _metatable lua元表名
 @param _var_list 参数列表
 @abstract 为已绑定lua环境的C++对象调用相关metatable方法
 @discussion 用于无返回值方法
 */
void _as_lua_metatable_call(lua_State* _ls, void* _obj, const char* _method, const ngLuaVarList& _var_list);

/*!
 @function _as_lua_metatable_call_for_result
 @param _ls lua环境指针
 @param _obj C++对象
 @param _metatable lua元表名
 @param _var_list 参数列表
 @result lua方法返回值，如果出错或没有返回值，返回无效ngLuaVar对象。
 @abstract 为已绑定lua环境的C++对象调用相关metatable方法
 @discussion
 */
ngLuaVar _as_lua_metatable_call_for_result(lua_State* _ls, void* _obj, const char* _method, const ngLuaVarList& _var_list);

#pragma mark - lua object interface

/* a simple code to inherit asLuaObject as public. */
#define AS_LUAOBJECT \
    public asLuaObject

#define AS_LUA_METATABLE(_ls, _table_name)  \
SetMetatable((_ls), (_table_name))

/* default means: _<_table_name>_metatable */
#define AS_LUA_DEFAULT_METATABLE(_ls, _table_name)  \
SetDefaultMetatable((_ls), (_table_name))

/* 转换userdata指针(void*)到特定的asLuaObject对象指针。 */
#define AS_USERDATA_CAST(_usr_data, _asLuaObjType) ((_asLuaObjType*)(asLuaObject*)((as_lua_c_userdata*)_usr_data)->origin)

class ngStringV2;
typedef ngStringV2 ngString;

/*!
 @class asLuaObject
 @abstract lua-c绑定类的基类，提供lua和c相互调用的接口封装
 @discussion 相比旧版本(ngLuaObject)，提供更灵活易用的接口
 
    注意：asLuaObject实例的销毁必须在其初始化时传入的lua环境(lua_State)关闭前进行。

 */

class asLuaObject {
public:
    explicit asLuaObject(lua_State* _ls);
    explicit asLuaObject(lua_State* _ls, boolean noBind);
    virtual ~asLuaObject();

private:
    /*!
     @function Bind
     @param _ls lua环境参数
     @abstract 绑定当前实例到lua环境
     @discussion 在基类asLuaObject构造函数中调用
     */
    void Bind(lua_State* _ls);

    /*!
     @function Release
     @param _ls lua环境参数
     @abstract 解除当前实例在lua环境中的引用的绑定，并释放相关资源
     @discussion 在基类asLuaObject析构函数中调用。

        注意：由于lua使用gc，所以lua引用可能依然存在，需要注意调用该方法之后避免继续使用改实例在lua中的引用
     
     */
    void Release(lua_State* _ls);

public:
    /*!
     @function Invoke
     @param _ls lua环境参数
     @param func lua方法名
     @param args 参数列表
     @abstract 调用当前实例的lua方法，通常在metatable中定义。（用于没有返回值的lua方法调用）
     @discussion 被调用的lua方法可以使用self
     */
    void Invoke(lua_State* _ls, NGCSTR func, const ngLuaVarList& args) const;

    /*!
     @function InvokeForResult
     @param _ls lua环境参数
     @param func lua方法名
     @param args 参数列表
     @result lua方法的返回值，只支持单个返回值
     @abstract 调用当前实例的lua方法，通常在metatable中定义
     @discussion 被调用的lua方法可以使用self
     */
    ngLuaVar InvokeForResult(lua_State* _ls, NGCSTR func, const ngLuaVarList& args) const;

    /*!
     @function PushLuaVal
     @param _ls lua环境参数
     @abstract push当前实例绑定的lua引用到当前lua栈顶
     @discussion
     */
    void PushLuaVal(lua_State* _ls);

    /*! 获取lua全局引用名。(用于从C++传入lua环境) */
    void GetRefName(ngString& strRefName) const;

protected:
    /*!
     @function SetMetatable
     @param _ls lua环境参数
     @param table_name 要设置的已存在的元表名
     @abstract 为当前实例的lua引用设置元表(metatable)
     @discussion 在派生类的构造函数中调用。建议使用宏AS_LUA_METATABLE(_ls, _table_name)调用。
     */
    void SetMetatable(lua_State* _ls, NGCSTR table_name);

    /*!
     @function SetDefaultMetatable
     @param _ls lua环境参数
     @param table_name 要设置的已存在的元表名，本方法会自动生成并使用格式为“_<table_name>_metatable”的最终元表名
     @abstract 为当前实例的lua引用设置元表(metatable)
     @discussion 在派生类的构造函数中调用。建议使用宏AS_LUA_DEFAULT_METATABLE(_ls, _table_name)调用。
     */
    void SetDefaultMetatable(lua_State* _ls, NGCSTR table_name);

private:
    /* 当前实例对应的userdata的block指针 */
    as_lua_c_userdata* m_pUserData;

    /* 当前实例所在的lua环境 */
    lua_State* m_pHostLuaState;
};

#endif	//__asLuaObject_h__
