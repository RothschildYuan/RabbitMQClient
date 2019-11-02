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
     *
     */
    printf("wwwww    %s\n", msg.m_data.c_str());
    mq->publish(msg, AAA_send_EXCHANGE, AAA_send_KEY);
}

int main()
{
    CQueue      queue(CDE_recv_QUEUE);
    CExchange   exchange(CDE_recv_EXCHANGE);
    CExchange   sexchange(CDE_send_EXCHANGE);

    CMessage msg("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    CRabbitMQ mq(9);
    mq.setHostIp("192.168.2.37");
    mq.setVHostName("/");
    mq.setPort(5672);
    mq.setUser("admin");
    mq.setPswd("123456");
    mq.init(queue, exchange, CDE_recv_KEY, callback, sexchange);
    mq.bind(CQueue(CDE_send_QUEUE), CExchange(AAA_send_EXCHANGE), AAA_send_KEY);
    mq.publish(msg, CDE_recv_EXCHANGE, CDE_recv_KEY);
    while(1) {
        sleep(1000);
    }
    cout << "Hello World!" << endl;
    return 0;
}
