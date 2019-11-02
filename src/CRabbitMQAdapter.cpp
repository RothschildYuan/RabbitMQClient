#include "CRabbitMQAdapter.h"
#include "amqp_tcp_socket.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

CRabbitMQAdapter::CRabbitMQAdapter(CRabbitMQ* rabbitMQ, const int channel)
{
    m_Sendchannel   = channel; //默认用1号通道
    m_Revcchannel   = m_Sendchannel+1;

    m_sock          = nullptr;
    m_conn          = nullptr;

    m_exchange      = nullptr;  // new CExchange("cfdefault.direct",0,"direct");
    m_queue         = nullptr;  // new CQueue("cfdefaultqueue",0);
    m_rabbitMQ      = rabbitMQ;
}

CRabbitMQAdapter::~CRabbitMQAdapter()
{
    m_port = 0;

    if(nullptr!=m_conn) {
        disconnect();
        m_conn = nullptr;
	}

    if(m_exchange != nullptr) {
		delete m_exchange;
        m_exchange = nullptr;
	}

    if(m_queue != nullptr) {
		delete m_queue;
        m_queue = nullptr;
	}
}

int CRabbitMQAdapter::connect(string& ErrorReturn)
{
    m_conn = amqp_new_connection();
    if(nullptr==m_conn) {
        ErrorReturn = "无法获得连接";
        return -1;
    }

    m_sock =  amqp_tcp_socket_new(m_conn);
    if(nullptr==m_sock) {
        ErrorReturn = "无法获得套接字";
        return -2;
    }

    if(amqp_socket_open(m_sock,m_hostIp.c_str(),m_port) < 0) {
        ErrorReturn = "无法连接目标主机";
        return -3;
    }

    if(assertError(amqp_login(m_conn, m_vHostName.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, m_user.c_str(), m_pswd.c_str()),"Loging in",ErrorReturn) != 1) {
        return -4;
    }
    return 0;
}

int CRabbitMQAdapter::disconnect(string& ErrorReturn)
{
    if(m_conn == nullptr) {
        return 0;
    }
    if(1!=assertError(amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS), "Closing connection",ErrorReturn))  return -1;
    if(amqp_destroy_connection(m_conn)<0)    return -2;
    m_conn = nullptr;
    return 0;
}

int CRabbitMQAdapter::exchangeDeclare(const CExchange &exchange,
                                       string &ErrorReturn)
{
    return exchangeDeclare(exchange, m_Sendchannel, ErrorReturn);
}

int CRabbitMQAdapter::exchangeDeclare(const CExchange &exchange,
                                       const int nChannel,
                                       string &ErrorReturn)
{
	 amqp_channel_open(m_conn, nChannel);
	 amqp_bytes_t _exchange = amqp_cstring_bytes(exchange.m_name.c_str());
	 amqp_bytes_t _type = amqp_cstring_bytes(exchange.m_type.c_str());
     int  _passive = exchange.m_passive;
     int  _durable = exchange.m_durable;
	 amqp_exchange_declare(m_conn, nChannel, _exchange, _type, _passive, _durable, 0,0,amqp_empty_table);

     if (1 != assertError(amqp_get_rpc_reply(m_conn), "exchange_declare", ErrorReturn)) {
		 amqp_channel_close(m_conn, nChannel, AMQP_REPLY_SUCCESS);
		 return -1;
	 }

	 amqp_channel_close(m_conn, nChannel, AMQP_REPLY_SUCCESS);
     if(m_exchange) {
         delete m_exchange;
         m_exchange = new CExchange(exchange);
     } else {
         m_exchange = new CExchange(exchange);
     }
     return 0;
}

int CRabbitMQAdapter::queueDeclare(const CQueue &queue,
                                    string &ErrorReturn)
{
	amqp_channel_open(m_conn, m_Sendchannel);
	amqp_bytes_t _queue =amqp_cstring_bytes(queue.m_name.c_str());
    int _passive = queue.m_passive;
    int _durable = queue.m_durable;
    int _exclusive = queue.m_exclusive;
    int _auto_delete =queue.m_auto_delete;
    amqp_queue_declare(m_conn,m_Sendchannel,_queue,_passive,_durable,_exclusive,_auto_delete,amqp_empty_table);

    if(1!= assertError(amqp_get_rpc_reply(m_conn),"queue_declare",ErrorReturn)) {
		amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
		return -1;
	}
    if(m_queue) {
        delete m_queue;
        m_queue = new CQueue(queue);
    } else {
        m_queue = new CQueue(queue);
    }
    amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
    return 0;
}

int CRabbitMQAdapter::queueBind(const CQueue &queue,
                                 const CExchange &exchange,
                                 const string &bind_key,
                                 string &ErrorReturn)
{
	 amqp_channel_open(m_conn, m_Sendchannel);
	 amqp_bytes_t _queue    = amqp_cstring_bytes(queue.m_name.c_str());
	 amqp_bytes_t _exchange = amqp_cstring_bytes(exchange.m_name.c_str());
	 amqp_bytes_t _routkey  = amqp_cstring_bytes(bind_key.c_str());
	 amqp_queue_bind(m_conn, m_Sendchannel,_queue,_exchange,_routkey,amqp_empty_table);

     if(1 != assertError(amqp_get_rpc_reply(m_conn),"queue_bind",ErrorReturn)) {
		 amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
		 return -1;
	 }
	 amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
     return 0;
}

int CRabbitMQAdapter::queueUnbind(const CQueue &queue,
                                   const CExchange &exchange,
                                   const string &bind_key,
                                   string &ErrorReturn)
{
	 amqp_channel_open(m_conn, m_Sendchannel);
	 amqp_bytes_t _queue    = amqp_cstring_bytes(queue.m_name.c_str());
	 amqp_bytes_t _exchange = amqp_cstring_bytes(exchange.m_name.c_str());
	 amqp_bytes_t _routkey  = amqp_cstring_bytes(bind_key.c_str());
	 amqp_queue_unbind(m_conn, m_Sendchannel,_queue,_exchange,_routkey,amqp_empty_table);

     if(1!= assertError(amqp_get_rpc_reply(m_conn),"queue_declare",ErrorReturn)) {
         amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
         return -1;
     }
	 amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
     return 0;
}

int CRabbitMQAdapter::publish(vector<CMessage> &message,
                               const string &exchange,
                               const string &routkey,
                               string &ErrorReturn)
{
    if(nullptr ==m_conn) {
        std::cout << "conn error";
		return -1;
    }
    amqp_channel_open(m_conn, m_Sendchannel);
    if(1!= assertError(amqp_get_rpc_reply(m_conn),"open channel",ErrorReturn)) {
		amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
        std::cout << "conn error open";
		return -1;
    }

   amqp_basic_properties_t props;
   for(vector<CMessage>::iterator it=message.begin(); it!=message.end(); ++it)
   {
	  amqp_bytes_t message_bytes;
	  message_bytes.len =(*it).m_data.length();
	  message_bytes.bytes =(void *)((*it).m_data.c_str());
	  props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	  props.content_type = amqp_cstring_bytes((*it).m_type.c_str());
      props.delivery_mode = (*it).m_durable;

      //这里修改目标 exchange
	  amqp_bytes_t _exchange = amqp_cstring_bytes(exchange.c_str());
      amqp_bytes_t _rout_key = amqp_cstring_bytes(routkey.c_str());

      if(amqp_basic_publish(m_conn, m_Sendchannel,_exchange,_rout_key,0,0,&props,message_bytes)!=0)
      {
         std::cout << "CRabbitMQAdapter::publish   publish  ERROR 发送消息失败" <<std::endl;
          if(1!= assertError(amqp_get_rpc_reply(m_conn),"open amqp_basic_publish",ErrorReturn)) {
              amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
              std::cout << "conn error open";
              return -1;
          }
      }
   }
   amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
   return 0;
}

int CRabbitMQAdapter::publish(const CMessage &message,
                               const string &exchange,
                               const string &routkey,
                               string &ErrorReturn)
{
    vector<CMessage> msg;
	msg.push_back(message);
    return publish(msg, exchange,routkey,ErrorReturn);
}

int CRabbitMQAdapter::consumer(const CQueue &queue,
                                vector<CMessage> &message,
                                int GetNum,
                                timeval *timeout,
                                string &ErrorReturn)
{
    if(nullptr ==m_conn) {
		ErrorReturn = "还未创建连接";
		return -1;
	}
	amqp_channel_open(m_conn, m_Revcchannel);
    if(1!=assertError(amqp_get_rpc_reply(m_conn),"open channel",ErrorReturn) ) {
		amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
		return -1;
	}
	amqp_bytes_t queuename= amqp_cstring_bytes(queue.m_name.c_str());
	amqp_queue_declare(m_conn, m_Revcchannel,queuename,0,queue.m_durable,0,0,amqp_empty_table);
	amqp_basic_qos(m_conn, m_Revcchannel,0,GetNum,0);
	int ack = 0; // no_ack    是否需要确认消息后再从队列中删除消息
	amqp_basic_consume(m_conn, m_Revcchannel,queuename,amqp_empty_bytes,0,ack,0,amqp_empty_table);

    if(1 != assertError(amqp_get_rpc_reply(m_conn),"Consuming",ErrorReturn)) {
		amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
		return -1;
	}

	CMessage tmp("tmp");
	amqp_rpc_reply_t res;
	amqp_envelope_t envelope;
	int hasget = 0;
	while(GetNum>0)
	{
		amqp_maybe_release_buffers(m_conn);
		res = amqp_consume_message(m_conn, &envelope,timeout, 0);
		if (AMQP_RESPONSE_NORMAL != res.reply_type)
		{
			amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
			ErrorReturn = "无法取得消息\n";
			if(0==hasget)
				return -res.reply_type;
			else
				return hasget;
		}

        string str((char *)envelope.message.body.bytes, (char *)envelope.message.body.bytes+envelope.message.body.len);
		tmp.m_data = str;
		tmp.m_data = tmp.m_data.substr(0,(int)envelope.message.body.len);
        tmp.m_routkey = (char *) envelope.routing_key.bytes;
        tmp.m_routkey = tmp.m_routkey.substr(0,(int)envelope.routing_key.len);
        message.push_back(tmp);
        amqp_destroy_envelope(&envelope);
        if(amqp_basic_ack(m_conn, m_Revcchannel,envelope.delivery_tag,1) != 0) {
			amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
			return -1;
		}
		GetNum--;
		hasget++;
		 __sleep(1);
	}
    return hasget;
}

int CRabbitMQAdapter::consumer(const string &queue_name,
                                vector<string> &message_array,
                                int GetNum,
                                timeval *timeout,
                                string &ErrorReturn)
{
    vector<CMessage> message;
    CQueue queue(queue_name);
    int hasget = CRabbitMQAdapter::consumer(queue, message, GetNum, timeout, ErrorReturn);
    for(vector<CMessage>::iterator it = message.begin();it != message.end(); ++it) {
        message_array.push_back((*it).m_data);
    }
    return hasget;
}

int CRabbitMQAdapter::consumer(const string &QueueName,
                                Func recvMsgFunc,
                                int GetNum,
                                timeval *timeout,
                                string &ErrorReturn)
{
    std::cout << "CRabbitMQAdapter::consumer "<<std::endl;
    __sleep(2000000);
    if(nullptr ==m_conn) {
        ErrorReturn = "还未创建连接";
        return -1;
    }
    amqp_channel_open(m_conn, m_Revcchannel);
    if(assertError(amqp_get_rpc_reply(m_conn),"open channel",ErrorReturn) != 1)    return -1;
    amqp_queue_declare(m_conn, m_Revcchannel,amqp_cstring_bytes(QueueName.c_str()),0, 0,0,0,amqp_empty_table);
    amqp_basic_qos(m_conn, m_Revcchannel,0,GetNum,0);
    int ack = 0; // no_ack    是否需要确认消息后再从队列中删除消息
    amqp_basic_consume(m_conn, m_Revcchannel,amqp_cstring_bytes(QueueName.c_str()),amqp_empty_bytes,0,ack,0,amqp_empty_table);
    if(1!=assertError(amqp_get_rpc_reply(m_conn),"Consuming",ErrorReturn) )     return -1;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    while(true) {
        amqp_maybe_release_buffers(m_conn);
        res = amqp_consume_message(m_conn, &envelope,timeout, 0);
        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            std::cout << " this is consumer errot "<<std::endl;
            ErrorReturn = "无法取得消息\n";
            return -res.reply_type;
        }
        CMessage tmp("tmp");
        string str((char *)envelope.message.body.bytes, (char *)envelope.message.body.bytes + envelope.message.body.len);
        tmp.m_data = str;
        tmp.m_data = tmp.m_data.substr(0, (int)envelope.message.body.len);
        tmp.m_routkey = (char *)envelope.routing_key.bytes;
        tmp.m_routkey = tmp.m_routkey.substr(0, (int)envelope.routing_key.len);
        recvMsgFunc(tmp, m_rabbitMQ);
        amqp_destroy_envelope(&envelope);
        amqp_basic_ack(m_conn, m_Revcchannel,envelope.delivery_tag,0);
         __sleep(1);
    }
    return 0;
}

int CRabbitMQAdapter::queueDelete(const string &queuename,
                                   const int if_unused,
                                   string &ErrorReturn)
{
    if(nullptr == m_conn) {
        ErrorReturn = "还未创建连接";
        return -1;
    }
    amqp_channel_open(m_conn, m_Sendchannel);
    if(1!=assertError(amqp_get_rpc_reply(m_conn),"open channel",ErrorReturn) )
    {
        amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
        return -1;
    }

    amqp_queue_delete(m_conn, m_Sendchannel,amqp_cstring_bytes(queuename.c_str()),if_unused,0);
    if(1!=assertError(amqp_get_rpc_reply(m_conn),"delete queue",ErrorReturn) )
    {
        amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
        return -1;
    }
    amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
    return 0;
}

int CRabbitMQAdapter::getMessageCount(const CQueue &queue,
                                       string& ErrorReturn)
{
    int TotalMessage = -1 ;
    if(nullptr == m_conn) {
        ErrorReturn = "还未创建连接";
        return -1;
    }
    amqp_channel_open(m_conn, m_Revcchannel);
    if(1!= assertError(amqp_get_rpc_reply(m_conn),"open channel",ErrorReturn)) {
        amqp_channel_close(m_conn, m_Sendchannel, AMQP_REPLY_SUCCESS);
        std::cout << "conn error open";
        return -1;
    }

    amqp_bytes_t _queue =amqp_cstring_bytes(queue.m_name.c_str());
    int _passive = queue.m_passive;
    int _durable = queue.m_durable;
    int _exclusive = queue.m_exclusive;
    int _auto_delete =queue.m_auto_delete;
    amqp_queue_declare_ok_t *p = amqp_queue_declare(m_conn, m_Revcchannel,_queue,_passive,_durable,_exclusive,_auto_delete,amqp_empty_table);
    if(1!=assertError(amqp_get_rpc_reply(m_conn),"Get Message count",ErrorReturn) ) {
        amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
        return -1;
    }
    amqp_channel_close(m_conn, m_Revcchannel, AMQP_REPLY_SUCCESS);
    TotalMessage = p->message_count;
    return TotalMessage;
}


int CRabbitMQAdapter::getMessageCount(const string &queuename,
                                       string& ErrorReturn)
{
    CQueue queue(queuename,1);
    return getMessageCount(queue, ErrorReturn);
}

void CRabbitMQAdapter::setHostIp(const string &hostIp)
{
    m_hostIp = hostIp;
}

void CRabbitMQAdapter::setVHostName(const string &vHostName)
{
    m_vHostName = vHostName;
}

void CRabbitMQAdapter::setPort(int port)
{
    m_port = port;
}

void CRabbitMQAdapter::setUser(const string &user)
{
    m_user = user;
}

void CRabbitMQAdapter::setPswd(const string &pswd)
{
    m_pswd = pswd;
}

void CRabbitMQAdapter::__sleep(int millsecond)
{
	#if defined (__linux)
		usleep(millsecond);
	#elif defined (WIN32)
	  Sleep(millsecond);
	#endif
}

//返回1代表正常 其他都是错
int CRabbitMQAdapter::assertError(amqp_rpc_reply_t x,
                                   const string& context,
                                   string& ErrorReturn)
{
    char rtnmsg[1024];
	switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
			return 1;
		case AMQP_RESPONSE_NONE:
            sprintf(rtnmsg, "%s: missing RPC reply type!\n", context.c_str());
			break;
        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
			sprintf(rtnmsg, "%s: %s\n", context.c_str(), amqp_error_string2(x.library_error));
			break;
		case AMQP_RESPONSE_SERVER_EXCEPTION:
			switch (x.reply.id) {
                case AMQP_CONNECTION_CLOSE_METHOD:
                {
                    amqp_connection_close_t *m1 = (amqp_connection_close_t *) x.reply.decoded;
                    sprintf(rtnmsg, "%s: server connection error %d, message: %.*s\n", context.c_str(), m1->reply_code, (int) m1->reply_text.len, (char *) m1->reply_text.bytes);
                }
                    break;
                case AMQP_CHANNEL_CLOSE_METHOD:
                {
                    amqp_channel_close_t *m2 = (amqp_channel_close_t *) x.reply.decoded;
                    sprintf(rtnmsg, "%s: server channel error %d, message: %.*s\n",context.c_str(), m2->reply_code, (int) m2->reply_text.len, (char *) m2->reply_text.bytes);
                }
                    break;
                default:
                    sprintf(rtnmsg, "%s: unknown server error, method id 0x%x\n", context.c_str(), x.reply.id);
					break;
            }
		break;
	}
	ErrorReturn = rtnmsg;
    std::cout<<"rtnmsg: "<<rtnmsg<<std::endl;
	return -1;
}

string CRabbitMQAdapter::m_ErrorData;
