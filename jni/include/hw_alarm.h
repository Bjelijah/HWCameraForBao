#ifndef hw_alarm_include_h
#define hw_alarm_include_h

#include "hw_net_base.h"
#include "stream_observer.h"
#include "stream_buf.h"
#include "hw_server.h"

class net_alarm
	:public stream_post
	 ,public hw_net_base
{
public:
		net_alarm(server_ref server);

		virtual ~net_alarm();

		bool start();

		void stop();

		bool is_start();

protected:
		virtual bool on_protocol_come(hw_msg& msg);
	
protected:
		server_ref m_server;
		dybuf m_dbuf;
};

typedef boost::shared_ptr<net_alarm> net_alarm_ref;

#endif

