#ifndef RABBITMQ_H
#define RABBITMQ_H
#include <string>
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
	
    /**
     * @brief init			初始化 RabbitMQ 句柄
     * @param recv_queue	默认接收队列对象
     * @param recv_exchange	默认接收数据的交换机对象
     * @param router_key	队列绑定交换机的路由Key
     * @param recvMsgFunc	处理接收消息的回调函数
     * @param dst_exchange	接收数据的交换机集
     * @return				0 成功，其他失败
     */
    int init(const CQueue& recv_queue,
             const CExchange& recv_exchange,
             const string& router_key,
             Func recvMsgFunc,
             const CExchange& dst_exchange);

    /**
     * @brief init          初始化 RabbitMQ 句柄
     * @param recv_queue    默认接收队列对象
     * @param recv_exchange 默认接收数据的交换机对象
     * @param router_key    队列绑定交换机的路由Key
     * @param recvMsgFunc   处理接收消息的回调函数
     * @param dst_exchange	接收数据的交换机集
     * @return				0 成功，其他失败
     */
    int init(const CQueue& recv_queue,
             const CExchange& recv_exchange,
             const string& router_key,
             Func recvMsgFunc,
             const std::vector<CExchange>& dst_exchange);

    /**
     * @brief bind			对队列名称，交换机名称，路由Key进行绑定
     * @param queueName     队列名称
     * @param exchangeName  交换机名称
     * @param bind_key      路由Key
     * @return				0 成功，其他失败
     */
    int bind(const string &queueName,
             const string &exchangeName,
             const string &bind_key);

    /**
     * @brief bind			对队列，交换机，路由Key进行绑定
     * @param queue			队列
     * @param exchange		交换机
     * @param bind_key		路由Key
     * @return				0 成功，其他失败
     */
    int bind(const CQueue& queue,
             const CExchange& exchange,
             const string& bind_key);

    /**
     * @brief publish		发布消息
     * @param msg			消息内容
     * @param exchange		交换机名称
     * @param routkey		路由Key
     * @return				0 成功，其他失败
     */
    int publish(const string &msg,
                const string &exchange,
                const string &routkey);

    /**
     * @brief publish		发布消息
     * @param message       消息内容对象
     * @param exchange      交换机名称
     * @param routkey       路由Key
     * @return              0 成功，其他失败
     */
    int publish(const CMessage& message,
                const string& exchange,
                const string& routkey);

    /**
     * @brief consumer		消费消息
     * @param QueueName		队列名称
     * @param recvMsgFunc	处理接收消息的回调函数
     * @return				0 成功，其他失败
     */
    int consumer(const string &QueueName,
                 Func recvMsgFunc);

    /**
     * @brief setHostIp		设置 RabbitMQ 句柄的主机ip
     * @param hostIp		主机ip
     */
    void setHostIp(const string &hostIp);

    /**
     * @brief setVHostName	设置 RabbitMQ 句柄的虚拟主机地址
     * @param vHostName		虚拟主机地址
     */
    void setVHostName(const string &vHostName);

    /**
     * @brief setPort		设置 RabbitMQ 句柄的端口号
     * @param port			端口号
     */
    void setPort(int port);

    /**
     * @brief setUser		设置 RabbitMQ 句柄的用户名
     * @param user			用户名
     */
    void setUser(const string &user);

    /**
     * @brief setPswd		设置 RabbitMQ 句柄的密码
     * @param pswd			密码
     */
    void setPswd(const string &pswd);

private:
    static void* mq_recv(void* param);

public:
    CRabbitMQAdapter*  	m_adapter;		// 适配器模式转接成符合 CRabbitMQ 的接口
    CQueue              m_recvQueue;
    Func                m_recvMsgFunc;	// 处理接收消息的回调函数
};

#endif
