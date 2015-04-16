#ifndef hw_voice_include_h
#define hw_voice_include_h

#include "hw_net_base.h"
#include "stream_observer.h"
#include "stream_buf.h"
#include "hw_server.h"

class net_voice
       :public hw_net_base
        ,public stream_post
{
public:
    typedef enum
    {
        VOICE_SEND_ONLY = 0,
        VOICE_SEND_RECV = 1,
    }VOICE_SEND_TYPE_E;

    net_voice(server_ref server);

    virtual ~net_voice();

    bool start(int slot,VOICE_SEND_TYPE_E type);

    void stop();

    bool is_start();

    bool input(const char* pcm,int len);

protected:
    virtual bool on_protocol_come(hw_msg& msg);

protected:
    server_ref m_server;
    dybuf m_dbuf_decode;
    dybuf m_dbuf_encode;
};

typedef boost::shared_ptr<net_voice> net_voice_ref;

#endif

