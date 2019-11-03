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

    /**
     * @brief connect           连接消息队列服务器
     * @param ErrorReturn		错误原因（传出参数）
     * @return					0 成功，其他失败
     */
    int connect(string& ErrorReturn = m_ErrorData);

    /**
     * @brief disconnect        与消息队列服务器断开连接
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int disconnect(string& ErrorReturn = m_ErrorData);

    /**
     * @brief exchangeDeclare   声明exchange
     * @param exchange			交换机对象
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int exchangeDeclare(const CExchange &exchange,
                        string& ErrorReturn = m_ErrorData);

    /**
     * @brief exchangeDeclare   声明exchange
     * @param exchange			交换机对象
     * @param nChannel			通道编号
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int exchangeDeclare(const CExchange &exchange,
                        const int nChannel,
                        string& ErrorReturn = m_ErrorData);

    //
    /**
     * @brief queueDeclare      声明消息队列
     * @param queue				交换机对象
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int queueDeclare(const CQueue &queue,
                     string& ErrorReturn = m_ErrorData);

    /**
     * @brief queueBind         将队列，交换机和绑定规则绑定起来形成一个路由表
     * @param queue				队列对象
     * @param exchange			交换机对象
     * @param bind_key			路由Key
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int queueBind(const CQueue &queue,
                  const CExchange &exchange,
                  const string& bind_key,
                  string& ErrorReturn = m_ErrorData);

    /**
     * @brief queueUnbind		将队列，交换机和绑定规则绑定解除
     * @param queue				队列对象
     * @param exchange          交换机对象
     * @param bind_key          路由Key
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int queueUnbind(const CQueue &queue,
                    const CExchange &exchange,
                    const string& bind_key,
                    string& ErrorReturn = m_ErrorData);

    /**
     * @brief publish           发布消息
     * @param message			消息对象集
     * @param exchange          交换机对象
     * @param routkey           路由Key
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int publish(vector<CMessage> &message,
                const string& exchange,
                const string& routkey,
                string& ErrorReturn = m_ErrorData);

    /**
     * @brief publish           发布消息
     * @param message			消息对象
     * @param exchange          交换机对象
     * @param routkey           路由Key
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int publish(const CMessage &message,
                const string& exchange,
                const string& routkey,
                string& ErrorReturn = m_ErrorData);

    //
    /**
     * @brief consumer          消费消息
     * @param queue				队列对象
     * @param message			消息对象集
     * @param GetNum			预取次数
     * @param timeout			超时
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int consumer(const CQueue &queue,
                 vector<CMessage> &message,
                 int GetNum = 1,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    /**
     * @brief consumer          消费消息
     * @param queue_name		队列名称
     * @param message_array     消息集
     * @param GetNum            预取次数
     * @param timeout           超时
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int consumer(const string& queue_name,
                 vector<string> &message_array,
                 int GetNum = 1000,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    /**
     * @brief consumer			消费消息
     * @param QueueName			队列名称
     * @param recvMsgFunc		处理接收消息的回调函数
     * @param GetNum			预取次数
     * @param timeout			超时
     * @param ErrorReturn       错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int consumer(const string& QueueName,
                 Func recvMsgFunc,
                 int GetNum = 1,
                 struct timeval *timeout = nullptr,
                 string& ErrorReturn = m_ErrorData);

    /**
     * @brief queueDelete       删除消息队列。
     * @param queuename			队列名称
     * @param if_unused			是否不用了
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int queueDelete(const string& queuename,
                    const int if_unused = 0,
                    string& ErrorReturn = m_ErrorData);

    /**
     * @brief getMessageCount   获得队列消息个数
     * @param queue				队列对象
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int getMessageCount(const CQueue &queue,
                        string& ErrorReturn = m_ErrorData);

    /**
     * @brief getMessageCount	获得队列消息个数
     * @param queuename			队列名称
     * @param ErrorReturn		错误原因（传出参数）
     * @return                  0 成功，其他失败
     */
    int getMessageCount(const string& queuename,
                        string& ErrorReturn = m_ErrorData);

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
    void __sleep(int millsecond);
	//返回1成功，其他是错误
    int assertError(amqp_rpc_reply_t x,
                    const string& context,
                    string& ErrorReturn = m_ErrorData);

public:
    string  m_hostIp;       // 主机ip
    string  m_vHostName;    // 虚拟主机地址
    int     m_port;         // 端口号
    string  m_user;         // 用户名
    string  m_pswd;         // 密码

    int     m_Sendchannel;  // 发送通道编号
    int     m_Revcchannel;  // 接收通道编号

    amqp_socket_t*          m_sock;
    amqp_connection_state_t m_conn;

    string                  m_routkey;
    CExchange*              m_exchange;
    CQueue*                 m_queue;
    CRabbitMQ*              m_rabbitMQ;

    static string m_ErrorData;
};

#endif
