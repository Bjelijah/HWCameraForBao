#ifndef hw_server_include_h
#define hw_server_include_h

#include "hw_net_base.h"
#include "boost/enable_shared_from_this.hpp"

class net_live_stream;
class net_file_stream;
class hw_files;

class server
	 :public hw_net_base
	  ,public boost::enable_shared_from_this<server>
{
public:
	server(const char* server_ip,
			short port,
			const char* name,
			const char* log_name,
			const char* log_psw);

	virtual ~server();

	bool is_login()
	{
		return m_log_ok;
	}	

	unsigned int slot_count()
	{
		if(!is_login()) 
		{
			return 0;
		}

		return m_svrinfo.slot_count;
	}

	long version()
	{
		if(!is_login()) 
		{
			return 0;
		}

		return m_svrinfo.server_version;
	}

	long chn_version(int slot);

	enum{
		TCP_MODE = 0,
		UDP_MODE = 1,
	};

	net_live_stream* get_live_stream(int slot,bool is_sub = false,int mode = TCP_MODE);

	net_file_stream* get_file_stream(int slot,SYSTEMTIME beg,SYSTEMTIME end);

	hw_files* get_file_list(int slot,SYSTEMTIME beg,SYSTEMTIME end,int file_type);

	hw_files* get_smart_file_list(int slot,SYSTEMTIME beg,SYSTEMTIME end,RECT search_rt);

	bool get_stream_head(int slot,int is_sub,HW_MEDIAINFO* media);	

	bool login();

	void logout();

	bool reboot();

	bool shutdown();

	bool get_device_config(device_info_t* dev_config);

	bool get_alarm_state(alarm_state_t* alarm_state);
	
	bool get_channel_type(channel_type_t* chn_type);

	bool force_i_frame(int slot);

	bool get_recfile_format(rec_file_format_t* file_type);

	bool get_video_color(video_color_t* color);

	bool set_video_color(video_color_t* color);

	bool get_net_cfg(net_cfg_t* net);

	bool set_net_cfg(net_cfg_t* net);

	bool get_slot_cfg(slot_cfg_t* slot_cfg);

	bool set_slot_cfg(slot_cfg_t* slot_cfg);

	bool get_video_quality(video_quality_t* quality);

	bool set_video_quality(video_quality_t* quality);

	bool get_sub_video_quality(sub_video_quality_t* quality);

	bool set_sub_video_quality(sub_video_quality_t* quality);

	bool get_channel_count(channel_count_t* channel_count);

	bool set_channel_count(channel_count_t* channel_count);

	bool get_osd_date(osd_date_t* osd_date);

	bool set_osd_date(osd_date_t* osd_date);

	bool get_osd_name(osd_name_t* osd_name);

	bool set_osd_name(osd_name_t* osd_name);

	bool get_ipc_feature(ipc_feature_t* ipc_feature);

	bool set_ipc_feature(ipc_feature_t* ipc_feature);

	bool get_stream_type(stream_type_t* stream_type);

	bool set_stream_type(stream_type_t* stream_type);

	bool get_motion_cfg(motion_cfg_t* motion_cfg);

	bool set_motion_cfg(motion_cfg_t* motion_cfg);

	bool get_systime(SYSTEMTIME* systime);

	bool set_systime(SYSTEMTIME* systime);

	bool restore_default();

	bool get_rs232_cfg(rs232_cfg_t* rs232_cfg);

	bool set_rs232_cfg(rs232_cfg_t* rs232_cfg);

	bool get_ptz_cfg(ptz_cfg_t* ptz_cfg);

	bool set_ptz_cfg(ptz_cfg_t* ptz_cfg);

	bool ptz_control(ptz_ctrl_t* ptz_ctrl);

	bool encode_get(int slot,int stream,encode_video_t* encode_vide,void* video_arg,encode_audio_t*encode_audio, void* audio_arg);

	bool encode_set(int slot,int stream,encode_video_t* encode_vide,void* video_arg,encode_audio_t*encode_audio, void* audio_arg);

    bool start_record(int slot);

    bool stop_record(int slot);

	//udp
	virtual bool start_udp(int slot,bool is_sub,const char* ip,int port);

	virtual bool stop_udp(int slot ,bool is_sub,const char* ip,int port);

	virtual bool udp_heartbeat(int slot,bool is_sub,const char* ip,int port);

    //snap
    bool save_to_jpg(int slot,int stream,int quality,const char* path);

    //motion rowcol
    bool get_motion_rowcol(int slot,int *row,int* col);

    //record ex
    bool start_record_ex(int slot,int stream);
    bool stop_record_ex(int slot,int stream);
    bool set_record_type(int type);

    //flip
    bool enable_flip(int slot,int enable);
    bool get_flip(int slot,int* is_flip);

    //blackwhite
    bool get_blackwhite(net_blackwhite_t* bw);
    bool set_blackwhite(net_blackwhite_t* bw);

    //gpio
    bool get_gpio(net_gpio_ctrl_t* gp);
    bool set_gpio(net_gpio_ctrl_t* gp);

private:
	bool is_valid_slot(int slot);

	bool udp_control(int type,udp_live_info_t* udp_live_info);

protected:
	virtual bool on_protocol_come(hw_msg& msg);
	
protected:
	bool m_log_ok;
	server_info_t m_svrinfo;
};

typedef boost::shared_ptr<server> server_ref;

#endif

