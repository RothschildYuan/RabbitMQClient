#ifndef RABBITMQ_H
#define RABBITMQ_H
#include <string>
#include "stdint.h"
#include <vector>
#include "MessageBody.h"

#include <fstream>

class CRabbitMQAdapter;

/** 
*   @brief 消息队列工具类
* 
*   class CRabbitMQ in "RabbitMQ.h"
**/
class WIN32_API CRabbitMQ
{
public:
    CRabbitMQ(const int channel = 1);
    ~CRabbitMQ();
	
    int init(const CQueue& recv_queue,
             const CExchange& recv_exchange,
             const string& router_key,
             Func recvMsgFunc,
             const CExchange& dst_exchange);

    int init(const CQueue& recv_queue,
             const CExchange& recv_exchange,
             const string& router_key,
             Func recvMsgFunc,
             const std::vector<CExchange>& dst_exchange);

    int bind(const CQueue& queue,
             const CExchange& exchange,
             const string& bind_key);

    int publish(const CMessage& message,
                const string& exchange,
                const string& routkey);

    int consumer(const string &QueueName,
                 Func recvMsgFunc);


    static void* mq_recv(void* param);

    void setHostIp(const string &hostIp);
    void setVHostName(const string &vHostName);
    void setPort(int port);
    void setUser(const string &user);
    void setPswd(const string &pswd);

public:
    CRabbitMQAdapter*  m_adapter;
    CQueue              m_recvQueue;
    Func                m_recvMsgFunc;


};

#endif
