#ifndef stream_observer_include_h
#define stream_observer_include_h

#include <list>
#include <boost/thread.hpp>

class stream_observer
{
public:
	virtual void on_stream_come(const char* buf,int len) = 0;
	virtual void on_stream_error(int errno) = 0;
};

class stream_post
{
public:
	stream_post()
	{

	}

	~stream_post()
	{

	}

	void register_stream_observer(stream_observer* observer)
	{
		boost::unique_lock<boost::mutex> lock(m_stream_observers_mu);

		std::list<stream_observer*>::iterator it;
		for(it = m_stream_observers.begin(); it != m_stream_observers.end(); it++)
		{
			if(*it == observer)
			{				
				return;
			}
		}

		m_stream_observers.push_back(observer);
	}

	void unregister_stream_observer(stream_observer* observer)
	{
		boost::unique_lock<boost::mutex> lock(m_stream_observers_mu);

		m_stream_observers.remove(observer);
	}

	void clear_observers()
	{
		boost::unique_lock<boost::mutex> lock(m_stream_observers_mu);
		m_stream_observers.clear();
	}

	int get_observer_count()
	{
		boost::unique_lock<boost::mutex> lock(m_stream_observers_mu);
		return m_stream_observers.size();
	}

protected:
	void post_stream_to_observer(const char* buf,int len)
	{
		boost::unique_lock<boost::mutex> lock(m_stream_observers_mu);
		std::list<stream_observer*>::iterator it;
		for(it = m_stream_observers.begin(); it != m_stream_observers.end(); it++)
		{
			if(*it)
			{
				(*it)->on_stream_come(buf,len);					
			}
		}
	}	

protected:
	std::list<stream_observer*> m_stream_observers;
	boost::mutex m_stream_observers_mu;
};

#endif

