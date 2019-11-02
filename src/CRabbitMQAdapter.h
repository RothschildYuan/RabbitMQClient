#ifndef RABBITMQ_ADAPTER_h
#define RABBITMQ_ADAPTER_h

#include <vector>

#include "amqp.h"
#include "MessageBody.h"

using std::vector;

class CRabbitMQ;
class CRabbitMQAdapter
{
public:
    CRabbitMQAdapter(CRabbitMQ* rabbitMQ, const int channel = 1);

    // 析构函数
    virtual ~CRabbitMQAdapter();

    // 连接消息队列服务器
    int connect(string& ErrorReturn = m_ErrorData);

    // 与消息队列服务器断开连接
    int disconnect(string& ErrorReturn = m_ErrorData);

    // 声明exchange
    int exchangeDeclare(const CExchange &exchange,
                        string& ErrorReturn = m_ErrorData);

    int exchangeDeclare(const CExchange &exchange,
                        const int nChannel,
                        string& ErrorReturn = m_ErrorData);

    // 声明消息队列
    int queueDeclare(const CQueue &queue,
                     string& ErrorReturn = m_ErrorData);

    //将队列，交换机和绑定规则绑定起来形成一个路由表
    int queueBind(const CQueue &queue,
                  const CExchange &exchange,
                  const string& bind_key,
                  string& ErrorReturn = m_ErrorData);

    //将队列，交换机和绑定规则绑定解除
    int queueUnbind(const CQueue &queue,
                    const CExchange &exchange,
                    const string& bind_key,
                    string& ErrorReturn = m_ErrorData);

    //发布消息
    int publish(vector<CMessage> &message,
                const string& exchange,
                const string& routkey,
                string& ErrorReturn = m_ErrorData);

    int publish(const CMessage &message,
                const string& exchange,
                const string& routkey,
                string& ErrorReturn = m_ErrorData);

    //消费消息
    int consumer(const CQueue &queue,
                 vector<CMessage> &message,
                 int GetNum = 1,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    int consumer(const string& queue_name,
                 vector<string> &message_array,
                 int GetNum = 1000,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    int consumer(const string& QueueName,
                 Func recvMsgFunc,
                 int GetNum = 1,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    //删除消息队列。
    int queueDelete(const string& queuename,
                    const int if_unused = 0,
                    string& ErrorReturn = m_ErrorData);

    //获得队列消息个数
    int getMessageCount(const CQueue &queue,
                        string& ErrorReturn = m_ErrorData);

    int getMessageCount(const string& queuename,
                        string& ErrorReturn = m_ErrorData);

    void setHostIp(const string &hostIp);
    void setVHostName(const string &vHostName);
    void setPort(int port);
    void setUser(const string &user);
    void setPswd(const string &pswd);
private:
    void __sleep(int millsecond);
	//返回1成功，其他是错误
    int assertError(amqp_rpc_reply_t x,
                    const string& context,
                    string& ErrorReturn = m_ErrorData);

public:
    string  m_hostIp;
    string  m_vHostName;
    int     m_port;
    string  m_user;
    string  m_pswd;

    int     m_Sendchannel;
    int     m_Revcchannel;

    amqp_socket_t*          m_sock;
    amqp_connection_state_t m_conn;

    string                  m_routkey;
    CExchange*              m_exchange;
    CQueue*                 m_queue;
    CRabbitMQ*              m_rabbitMQ;

    static string m_ErrorData;
};

#endif
