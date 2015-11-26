#	include <stdio.h>
#	include "cocos2d.h"
#	include "net.h"

//*nux
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || \
(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)|| \
(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID))
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <signal.h>
#	include <sys/ioctl.h>
//#include <netdb.h>
#	define XNUX 1
#else
#	define MSWIN 1
#endif

#	include <string.h>
#	include <stdlib.h>
#	include <thread>
#	include <math.h>


#ifdef WIN32
#	include <time.h>
#	include <string>
#	include <WinSock2.h>
#	include <stdlib.h>
#	include <WS2tcpip.h>
#	define _usleep(s) Sleep(s)
#else
#	include <sys/time.h>
#	include <unistd.h>
#	define _usleep(s) usleep((s) * 1000)
#endif

#define  DEL_ARRAY(exp) \
if((exp)!= nullptr)		\
{						\
	delete[] (exp);		\
	(exp)=nullptr;		\
}

#define DEL_PTR(exp)	\
if((exp)!=nullptr)		\
{						\
	delete (exp);		\
	(exp) = nullptr;	\
}

#define ERROR_FUN(_c_)								\
CCLOG("ERROR CONNECTING TO SERVER,error:%d",_c_);	\
errorHandler::ErrorArgs arg;						\
arg.setCode(errorHandler::TCP_ERROR_CONNECT);		\
this->errorTriger(arg);								\
session = 0;										\
return;

#define BANDWIDTH_DEFAULT_VALUE  0x00

void connectSert(tcp::state* state, int sockfd, sockaddr_in server_addr){
	int confres = (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)));
	*state = (tcp::state) confres;
}

static std::vector<int> errorSignal;


void handerfun(int id){
	errorSignal.push_back(id);
}

tcp::tcp(const char* host, unsigned int port) : m_data(nullptr), port(port), m_tcp_state(tcp::state::NEW_CREATE),
m_bandwidth(BANDWIDTH_DEFAULT_VALUE)
{
	CCASSERT(host, "must set host and prort.");
	tcp::host.append(host);
	this->m_buffer = new packe();
	this->m_buffer->capacity = REV_BUF_LEN;
	this->m_buffer->buf = new unsigned char[this->m_buffer->capacity];
}
tcp::~tcp(void){
	DEL_ARRAY(m_data->buf);
	DEL_PTR(m_data);

	DEL_ARRAY(m_buffer->buf);
	DEL_PTR(m_buffer);
}

void tcp::connect(void){
	if (this->m_tcp_state == tcp::state::CONNECTTING) return;
	struct sockaddr_in sock_add;
	struct sockaddr_in server_addr;
	const char* addr = (const char*)tcp::host.c_str();
	int port = tcp::port;
	CCLOG("connect server host:%s,port %d", addr, port);
	this->session = socket(AF_INET, SOCK_STREAM, 0);    //ipv4,TCP数据连接
	if (this->session < 0) {
		CCLOG("connect error");
		this->m_tcp_state = tcp::state::DISCONNECTED;
		std::string str("-1");
		this->ondisconnect(str);
	}

#ifndef WIN32
	bzero(&server_addr, sizeof(server_addr));
#endif

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//::ioctl(sockfd, FIONBIO,0);
	if (inet_pton(AF_INET, addr, &server_addr.sin_addr) < 0){    //set ip address
		CCLOG("address error");
		this->m_tcp_state = tcp::state::DISCONNECTED;
		std::string str("-2");
		this->ondisconnect(str);
	}

	//submit socket connect request.
	socklen_t server_addr_length = sizeof(server_addr);
	this->m_tcp_state = tcp::state::CONNECTED;
	std::thread thread(&connectSert, &(this->m_tcp_state), this->session, server_addr);
	thread.detach();
	_usleep(1000);
}

bool tcp::isConnected(){
	return this->m_tcp_state == tcp::state::CONNECTED;
}

void tcp::shutdown(int a){
	if (session) {
#if XNUX
		::shutdown(session, a);
#elif WIN32
		::shutdown(this->session, SD_BOTH);	
#endif
		this->session = 0;
		cocos2d::CCDirector::removeLoop(this);
        m_bandwidth = BANDWIDTH_DEFAULT_VALUE;
	}
}

void tcp::send(packe& data){
	int i = 0; //vc下的特殊写法

	if (!this->isConnected())
	{
		std::string dt;
		dt.append((const char*)data.buf, data.length);
		this->ondisconnect(dt);
		CCLOG("send tcp data error!");
		return;
	}

	if (data.length > 0 && data.buf != NULL) {
		int len = data.length + head;
		int u = data.length;
		long res = 0;
		char *str = new char[len];
		char b[head];
		memset(str, 0, len);
		memset(b, 0, head);
		CCASSERT(str && b, "memory error");

		//分析头
		for (; i < head; i++)
			b[head - i - 1] = (char)(u >> (i * 8));
		
		memcpy(str, b, head);
		memcpy(str + head, data.buf, data.length);

#if XNUX
		//处理错误异常，不导致应用闪退。
		struct sigaction act, oact;
		act.sa_handler = handerfun;
		sigemptyset(&act. sa_mask);
		act.sa_flags = 0;
		sigaction(SIGPIPE, &act, &oact);

		if(session){
			res = ::send(session, str, data.length+ head, MSG_DONTWAIT);
		}
		CCLOG("send data:%s", data.buf);
#else
		//send data with the ms windows
		res = ::send(this->session, str, data.length + head, 0);
		CCLOG("send data:%s", data.buf);
#endif
		if (res < 0)
		{
			std::string dt;
			dt.append((const char*)data.buf, data.length);
			this -> ondisconnect(dt);
			CCLOG("send tcp data error!");
		}

		DEL_ARRAY(str);
	}
};

void tcp::ondisconnect(std::string&dt){
	errorHandler::ErrorArgs arg;
#if WIN32
	this->shutdown(SD_BOTH);
#else
	//TODO
	this->shutdown(SHUT_RDWR);
#endif
	arg.setRequestData(dt);
	arg.setCode(errorHandler::TCP_ERROR_SEND);
	this->m_tcp_state = tcp::state::DISCONNECTED;
	std::vector<tcp::errorHandler*> ::iterator itr = errorHandlers.begin();
	while (itr != errorHandlers.end())
	{
		(*itr)->invork(arg);
		++itr;
	}
}

void tcp::bind(handler *handler){
	if (handler){
		handlers.push_back(handler);
	}
}

void tcp::removeHandler(handler* handler){
	if (handler){
		std::vector<tcp::handler*>::iterator itr = handlers.begin();
		while (itr != handlers.end())
		{
			if (*itr == handler)handlers.erase(itr);
			++itr;
		}
	}
}

void tcp::Update(){

	//开始处理数据
	if (!this->isConnected())return;

	//tprocess signals 
	if (errorSignal.begin() != errorSignal.end()){
		std::string str;
		this->ondisconnect(str);
		errorSignal.clear();
	}
#if XNUX
    bzero(m_buffer->buf,REV_BUF_LEN );
#endif
	int rev_len =
#if XNUX
	 ::recv(session, (char*)(m_buffer->buf + m_buffer->length), REV_BUF_LEN, MSG_DONTWAIT);
#elif WIN32
	 ::recv(session, (char*)(m_buffer->buf + m_buffer->length), REV_BUF_LEN - m_buffer ->length, 0);
#endif

	if (rev_len <= 0) return;
	m_buffer->length += rev_len;
	
	//没有达到处理条件
	if (m_data == nullptr && m_buffer->length < head) return;
	int _offset = 0;
	while (_offset < m_buffer->length)
	{
		if (m_data == nullptr){
            
//            if(m_bandwidth == 0x00){
//                m_bandwidth = *m_buffer->buf;
//                _offset++;
//            }
//            
			m_data = new packe();
			m_data->capacity = 0;
			m_data->length = 0;
            
            //64们服务器做的特殊处理
//            if(m_bandwidth == 0x40){
//                m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
//                m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
//                m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
//                m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
//            }
            
            //32位保留
            m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
            m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
            m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++); m_data->capacity <<= 8;
			m_data->capacity |= (unsigned char)*(m_buffer->buf + _offset++);

			m_data->length = 0;
			m_data->buf = new unsigned char[m_data->capacity];
			memset(m_data->buf, 0, m_buffer->length);
		}
        

		//copy data to tag;
		int suf_len = m_buffer->length - _offset;
		int m_data_suf_len = m_data->capacity - m_data->length;
		int processLen = suf_len <= m_data_suf_len ? suf_len : m_data_suf_len;
        
        if (processLen == 0) break;

		memcpy(m_data->buf + m_data->length,m_buffer->buf + _offset,processLen);
		_offset += processLen;
        m_data->length += processLen;
		if (processLen == m_data_suf_len){
			for (int i = 0; i < handlers.size();i++)
			{
				CCLOG("recive remove data len: .. %d", m_data->length);
				CCLOG("recive remove data: %s", m_data->buf);
				handlers[i]->invork(*m_data);
			}

			DEL_ARRAY(m_data->buf);
			DEL_PTR(m_data);
		}
	}
    m_buffer->length = 0;
};

void tcp::addErrorHandler(tcp::errorHandler * handler){
	if (handler){
		errorHandlers.push_back(handler);
	}
};

void tcp::removeErrorHandler(tcp::errorHandler* handler){
	if (handler){
		std::vector<tcp::errorHandler*>::iterator itr = errorHandlers.begin();
		while (itr != errorHandlers.end())
		{
			if (*itr == handler)errorHandlers.erase(itr);
			++itr;
		}
	}
};