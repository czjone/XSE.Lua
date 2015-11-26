#ifndef __xsegame__Socket__
#define __xsegame__Socket__ 1

#include <vector>
#include "cocos2d.h"
#ifndef WIN32
#include "CCDirector.h"
#endif
#include <assert.h>

#define REV_BUF_LEN 1500    //帧长度
#define MAX_PACK_LEN 1024  //
#define head 4			//64位也用四个字节

extern "C" {
	extern int call_data_proccess(const char*, const char*, int len);
}

class tcp : public cocos2d::Director::IUpdate{
    
public:
    enum state{
		CONNECT_ERROR		=	-1,
        NEW_CREATE          =   2,
        CONNECTTING         =   1,
        CONNECTED           =   0,
        DISCONNECTED        =   -2,
        SHUTDOWN            =   -3,
    };
     
public:
    
    //data packe
    class packe{
    public:
        int capacity; //当前 开辟的容量，要多少开多少
        int length;    //当前已填充的长度
        unsigned char* buf;
    };
    
    //data packe handler
    class handler{
    public:
        virtual ~handler(){};
        virtual void invork(packe& data) = 0;
    };
    
    //error handler
    class errorHandler{
        
        public :
        enum ERROR_CODE{
            TCP_ERROR_UNKNOW        =   0x01,
            TCP_ERROR_CONNECT       =   0x02,
            TCP_ERROR_SEND          =   0x03,
            TCP_ERROR_BINDADRESS    =   0x04,
            TCP_ERROR_PROTOCAL      =   0x05,
            TCP_ERROR_SERVER_ERROR  =   0x08,
        };
        
    public:
        //error handler args.
        class ErrorArgs{
            
            friend class tcp;
            
        private:
            int code;
            std::string m_sdata;
        private:
            inline void setCode(int code){this->code  = code;};
        public:
            inline int getCode(){return code;};
            inline void setRequestData(std::string dt){m_sdata = dt;};
            inline std::string getRequestData(void){return m_sdata;};
        };
    public:
        virtual ~errorHandler(){};
        virtual void invork(ErrorArgs& args) = 0;
    private:
        ErrorArgs args;
    };
    
    typedef void (tcp::*err)(int);
    //tcp processer.
public:
    tcp(const char* host,unsigned int port);
    ~tcp(void);
    
    void connect(void);
    void shutdown(int);
    void send(packe& data);
    void bind(handler *handler);
    void removeHandler(handler* handler);
    void addErrorHandler(errorHandler*);
    void removeErrorHandler(errorHandler*);
    void ondisconnect(std::string&);
    bool isConnected();
    
private:
    inline void errorTriger(errorHandler::ErrorArgs &args){
        std::vector<tcp::errorHandler*>::iterator itr = errorHandlers.begin();
        while (itr != errorHandlers.end())(*itr)->invork(args);
    };
    
	inline void onerror(int code){ errorHandler::ErrorArgs args; args.setCode(code); this->errorTriger(args); };
    
private:
    std::string host;
    unsigned int port;
    int session;
    state m_tcp_state;
    packe *m_data;
	packe *m_buffer;
    char m_bandwidth;
    std::vector<tcp::handler*> handlers;
    std::vector<tcp::errorHandler*> errorHandlers;
    
protected:
    //loop recive looper
    virtual void Update();
};

//===================================================
//lua tcp

#include "tolua++.h"
#include <vector>
#include <string>

using namespace std;
//lua apis
class lua_tcp{
private:
    class handler:public tcp::handler{
    private:
        vector<string> calls;
    public:
        virtual void invork(tcp::packe& data){
            if(data.length<=0) return;
			//extern int call_data_proccess(const char*, const char*, int len);
            for(vector<string>::iterator iter = calls.begin();iter!=calls.end();iter++){
				call_data_proccess(iter->c_str(), (const char*)data.buf, data.length);
            }
        };
        
        inline void setCall(const char* name){
            if(strlen(name)>0 && find(calls.begin(),calls.end(),std::string(name)) == calls.end())
                calls.push_back(name);
        };
        
        
        inline void revmoveCall(const char* name){
            if(strlen(name)>0){
                string str(name);
                for(vector<string>::iterator iter = calls.begin();iter!=calls.end();iter++){
                    if(str.compare(*iter)){
                        calls.erase(iter);
                    }
                }
                
            }
        };
    };
    
    class errorHandler:public tcp::errorHandler{
    private:
        vector<string> calls;
    public:
        virtual void invork(ErrorArgs& args){
			//TODO::应该调用 call_error_proccess
			
            for(vector<string>::iterator iter = calls.begin();iter!=calls.end();iter++){
				call_data_proccess(iter->c_str(), (const char*)(args.getRequestData().c_str()), (int)args.getRequestData().length());
            }
        };
        
        inline void setCall(const char* name){
            if(strlen(name)>0 && find(calls.begin(),calls.end(),std::string(name)) == calls.end())
                calls.push_back(name);
        };
        
        
        inline void revmoveCall(const char* name){
            if(strlen(name)>0){
                string str(name);
                for(vector<string>::iterator iter = calls.begin();iter!=calls.end();iter++){
                    if(str.compare(*iter)){
                        calls.erase(iter);
                    }
                }
                
            }
        };
        
    };
    
public:
    lua_tcp(void):m_tcp(nullptr){
    };
    
    ~lua_tcp(void){
        if(m_tcp){
            m_tcp->removeHandler(&m_handler);
            delete m_tcp;
            m_tcp=nullptr;
        }
    };
    inline void connect(const char* host,int port){
        if(!m_tcp){
            m_tcp = new tcp(host,port);
            m_tcp->bind(&m_handler);
            m_tcp->addErrorHandler(&m_error_handler);
        }
        
        m_tcp->connect();
    };
    inline void shutdown(){
        if(m_tcp){
            m_tcp->shutdown(2);
        }
    };
    inline void send(const char* d,const char* callbackFuc){
        m_handler.setCall(callbackFuc);
        
        tcp::packe _p;
        _p.buf = (unsigned char*)d;
        _p.length =strlen(d);
        m_tcp->send(_p);
    };
    
    inline void remove(const char* callbackFuc){
        m_handler.revmoveCall(callbackFuc);
    };
    
    inline void binderr(const char* callbackFuc){
        m_error_handler.setCall(callbackFuc);
    };
    
private:
    handler m_handler;
    errorHandler m_error_handler;
    tcp *m_tcp;
};


#endif /* defined(__xsegame__Socket__) */
