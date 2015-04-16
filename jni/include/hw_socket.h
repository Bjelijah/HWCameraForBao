#ifndef hw_socket_include_h
#define hw_socket_include_h
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

class hw_tcp_socket
	:public boost::noncopyable
{
public:
	hw_tcp_socket();

	virtual ~hw_tcp_socket();

	bool connect(const char* ip,short port,int timeout = 5000);

	void disconnect();

	bool send_packet(const char* buf,int len,int timeout = 1000);

	int s();
protected:
	virtual void on_recv_data(const char* buf,int len);
	int m_s;
	bool m_close;
	bool m_connectted;
};

typedef boost::shared_ptr<hw_tcp_socket> hw_tcp_socket_ptr;

#endif
