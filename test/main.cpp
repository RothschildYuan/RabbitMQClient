#include <iostream>
#include <string>
#include <unistd.h>
#include "CRabbitMQ.h"

using namespace std;

#define AAA_send_EXCHANGE   "AAA_send_EXCHANGE"
#define AAA_send_QUEUE      "AAA_send_QUEUE"
#define AAA_send_KEY        "AAA_send_KEY"
#define AAA_recv_EXCHANGE   "AAA_recv_EXCHANGE"
#define AAA_recv_QUEUE      "AAA_recv_QUEUE"
#define AAA_recv_KEY        "AAA_recv_KEY"

#define CDE_recv_EXCHANGE   "CDE_EXCHANGE"
#define CDE_recv_QUEUE      "CDE_QUEUE"
#define CDE_recv_KEY        "CDE_KEY"

#define CDE_send_EXCHANGE   "CDE_send_EXCHANGE"
#define CDE_send_QUEUE      "CDE_send_QUEUE"
#define CDE_send_KEY        "CDE_send_KEY"

void callback(CMessage &msg, void* arg) {
    CRabbitMQ* mq = (CRabbitMQ*)arg;

    /*
     * 业务逻辑处理
     */
    printf("wwwww    %s\n", msg.m_data.c_str());
	// 根据交换机名称和路由键Key名称发送高级消息队列消息
    mq->publish(msg.m_data, AAA_send_EXCHANGE, AAA_send_KEY);
}

int main()
{
	// 队列对象
    CQueue      queue(CDE_recv_QUEUE);
	// 接收交换机对象
    CExchange   exchange(CDE_recv_EXCHANGE);
	// 发送交换机对象
    CExchange   sexchange(CDE_send_EXCHANGE);

    // MQ 连接句柄对象
    CRabbitMQ mq;
	// 设置 ip，端口，账号等
    mq.setHostIp("192.168.2.37");
    mq.setVHostName("/");
    mq.setPort(5672);
    mq.setUser("admin");
    mq.setPswd("123456");
	
	// 定义默认队列，默认接收交换机，绑定队列，设置发送交换机对象
    mq.init(queue, exchange, CDE_recv_KEY, callback, sexchange);
	// 绑定队列，交换机与路由Key
    mq.bind(CQueue(CDE_send_QUEUE), CExchange(AAA_send_EXCHANGE), AAA_send_KEY);
    CMessage msg("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	// 发送消息到指定交换机和路由Key中
    mq.publish(msg, CDE_recv_EXCHANGE, CDE_recv_KEY);
    while(1) {
        sleep(1000);
    }
    cout << "Hello World!" << endl;
    return 0;
}
