#include "CRabbitMQ.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>

#include "CRabbitMQAdapter.h"

CRabbitMQ::CRabbitMQ(const int channel)
{
    m_adapter = new CRabbitMQAdapter(this, channel);
    if(m_adapter == nullptr) {
        perror("m_adapter == nullptr");
    }
    m_recvMsgFunc = nullptr;
}

CRabbitMQ::~CRabbitMQ()
{
    if(m_adapter != nullptr) {
        m_adapter->disconnect();
        delete m_adapter;
        m_adapter = nullptr;
    }
}

int CRabbitMQ::init(const CQueue &recv_queue,
                    const CExchange &recv_exchange,
                    const string &router_key,
                    Func recvMsgFunc,
                    const CExchange &dst_exchange)
{
    vector<CExchange> exchanges;
    exchanges.push_back(dst_exchange);
    return init(recv_queue, recv_exchange, router_key, recvMsgFunc, exchanges);
}

int CRabbitMQ::init(const CQueue &recv_queue,
                    const CExchange &recv_exchange,
                    const string &router_key,
                    Func recvMsgFunc,
                    const vector<CExchange> &dst_exchange)
{
    m_recvMsgFunc = recvMsgFunc;
    if (m_adapter->connect() < 0) {
        std::cout << "connect err\n";
        return -2;
    }

    if (m_adapter->exchangeDeclare(recv_exchange) < 0) {
        std::cout << "exchange_declare err\n";
        return -3;
    }

    for (size_t i=10;i<dst_exchange.size()+10;i++) {
        if (m_adapter->exchangeDeclare(dst_exchange[i-10],5) < 0) {
            std::cout << "exchange_declare err"<<dst_exchange[i-10].m_name.c_str()<<std::endl;
            return -4;
		}
	}
    m_recvQueue = recv_queue;
    if ((m_adapter->queueDeclare(recv_queue) < 0)) {
        std::cout << "queue_declare err  " << "queue name  " << recv_queue.m_name.c_str() << std::endl;
        return -5;
    }

    if ((m_adapter->queueBind(recv_queue, recv_exchange, router_key) < 0))  {
        std::cout << "queue_bind err\n";
        return -6;
    }
    pthread_t ntid;
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&ntid, &attr, mq_recv, this) != 0) {
        printf("can't create thread: \n");
        return -7;
    }
    return 0;
}

int CRabbitMQ::bind(const string &queueName,
                    const string &exchangeName,
                    const string &bind_key)
{
    return bind(CQueue(queueName), CExchange(exchangeName), bind_key);
}

int CRabbitMQ::bind(const CQueue &queue,
                    const CExchange &exchange,
                    const string &bind_key)
{
    if ((m_adapter->queueBind(queue, exchange, bind_key) < 0))  {
        std::cout << "queue_bind err\n";
        return -6;
    }
    return 0;
}

int CRabbitMQ::publish(const string &msg,
                       const string &exchange,
                       const string &routkey)
{
    return m_adapter->publish(CMessage(msg), exchange, routkey);
}

int CRabbitMQ::publish(const CMessage &message,
                       const string &exchange,
                       const string &routkey)
{
    return m_adapter->publish(message, exchange, routkey);
}

int CRabbitMQ::consumer(const string &QueueName,
                        Func recvMsgFunc)
{
    return m_adapter->consumer(QueueName, recvMsgFunc);
}

void CRabbitMQ::setHostIp(const string &hostIp)
{
    m_adapter->setHostIp(hostIp);
}

void CRabbitMQ::setVHostName(const string &vHostName)
{
    m_adapter->setVHostName(vHostName);
}

void CRabbitMQ::setPort(int port)
{
    m_adapter->setPort(port);
}

void CRabbitMQ::setUser(const string &user)
{
    m_adapter->setUser(user);
}

void CRabbitMQ::setPswd(const string &pswd)
{
    m_adapter->setPswd(pswd);
}

void* CRabbitMQ::mq_recv(void* param)
{
    CRabbitMQ* mq = (CRabbitMQ*)param;
    string queue_name = mq->m_recvQueue.m_name;
    if (mq->consumer(queue_name, mq->m_recvMsgFunc)) {
        std::cout << " mq_recv consumer err\n";
        return nullptr;
    }
    return nullptr;
}
