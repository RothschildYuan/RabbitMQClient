#ifndef MESSAGE_BODY_H
#define MESSAGE_BODY_H

#include <stdint.h>
#include <string>

using std::string;

#ifndef __linux__
#define	STDCALL					__stdcall

#define IMEXPORTS

#ifdef IMEXPORTS
#define  WIN32_API __declspec(dllexport)
#else
#define  WIN32_API __declspec(dllimport)
#endif
#else
#define  WIN32_API
#endif


class WIN32_API CMessage
{
public:

    CMessage(string data,int32_t durable=1):
        m_data(data),
        m_durable(durable)
    {
        m_type = "text/plain";
    }

    CMessage(string data,string routkey, int32_t durable=1):
        m_data(data),
        m_routkey(routkey),
        m_durable(durable)
    {
        m_type = "text/plain";
    }

    //拷贝构造函数
    CMessage(const CMessage &other)
    {
        m_data      = other.m_data;
        m_durable   = other.m_durable;
        m_type      = other.m_type;
        m_routkey  =other.m_routkey;
    }

    //重载 赋值= 运算符
    CMessage& operator=(const CMessage &other)
    {
        if(this==&other) {
            return *this;
        }
        m_data      = other.m_data;
        m_durable   = other.m_durable;
        m_type      = other.m_type;
        m_routkey  =other.m_routkey;
        return *this;
    }

    string  m_data;
    string  m_routkey;
    int32_t      m_durable;
    string  m_type;
};

typedef void(*Func)(CMessage &msg, void* arg);

class WIN32_API CQueue
{
public:

    CQueue(string name = "moren",int32_t durable=0):
        m_name(name),
        m_durable(durable)
    {
        m_auto_delete   = 0;
        m_exclusive     = 0;
        m_passive       = 0;
    }

    //拷贝构造函数
    CQueue(const CQueue &other)
    {
        m_name          = other.m_name;
        m_durable		= other.m_durable;
        m_auto_delete   = other.m_auto_delete;
        m_exclusive     = other.m_exclusive;
        m_passive       = other.m_passive;
    }

    //重载 赋值= 运算符
    CQueue& operator=(const CQueue &other)
    {
        if(this==&other) {
            return *this;
        }
        m_name		    = other.m_name;
        m_durable       = other.m_durable;
        m_auto_delete   = other.m_auto_delete;
        m_exclusive     = other.m_exclusive  ;
        m_passive       = other.m_passive    ;
        return *this;
    }

    string  m_name;         //消息队列名称
    int     m_durable;      //消息队列是否持久化
    int     m_exclusive;    //当前连接不在时，队列是否自动删除
    int     m_auto_delete;  //没有consumer时，队列是否自动删除
    int     m_passive;

};

/**
*   @brief 消息队列的队列实体
*
*   class Message in "RabbitMQ.h"
**/
class WIN32_API CExchange
{
public:

    CExchange(string name,int32_t durable=0,string type="direct"):
        m_name(name),m_durable(durable),m_type(type)
    {
        m_passive   = 0;
    }

    //拷贝构造函数
    CExchange(const CExchange &other)
    {
        m_name      = other.m_name;
        m_durable	= other.m_durable;
        m_type      = other.m_type;
        m_passive   = other.m_passive;
    }

    //重载 赋值= 运算符
    CExchange& operator=(const CExchange &other)
    {
        if(this==&other) {
            return *this;
        }
        m_name      = other.m_name;
        m_durable   = other.m_durable;
        m_type      = other.m_type;
        m_passive   = other.m_passive;
        return *this;
    }

public:
    string m_name;         //交换机名称
    int    m_durable;      //交换机是否持久化
    string m_type;         //交换机类型  "fanout"  "direct" "topic"三选一
    int    m_passive;
};

#endif
