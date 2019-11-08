#ifndef __RENDERING_SERVER_CLIENT_H__
#define __RENDERING_SERVER_CLIENT_H__
#include "signaller/signaller.h"
#include "json_parser/json_parser.h"
#include <functional>
#include <map>
#include <future>
#include "./function_thread_handler/func_thread_handler.h"
#include "signaller/websocket_signaller.h"
constexpr const char* RENDERING_SERVER_IP = "localhost";
constexpr const char* RENDERING_SERVER_PORT = "8002";

constexpr const char* mic_toggle_id = "mic_toggle";
constexpr const char* cam_toggle_id = "cam_toggle";

namespace grt {

	using function_callback = std::function<void(message_type, absl::any msg)>;

	class rendering_server_client : public signaller_callback , 
		public parser_callback{

	private:
		std::map< std::string, function_callback > register_functions_;
		std::promise<bool> connect_event_;
	public:

		void register_function(std::string id, function_callback callback);
		void unregister_function(std::string id);
		void set_connect_event(std::promise<bool>);
		/***signaller_callback interfaces*/
		void on_message(std::string msg) override;
		void on_connect() override;
		void on_error(std::string error) override;
		void on_close() override;

		//parser_callback interface implementation
		void on_message(message_type, absl::any msg) override;

	};

	class sender {
	public:
		sender();
		~sender();
		//void connect(std::string address, std::string port);
		std::future<bool> sync_connect(std::string address, std::string port);
		void send_to_renderer(std::string id, std::string message, function_callback response);
		void done(std::string id);
		void register_for_session_leave_msg(function_callback response);
		void register_for_message(std::string id, function_callback response);
	private:
		util::func_thread_handler function_thread_;
		websocket_signaller_unsecure signaller_;
		std::shared_ptr< rendering_server_client> server_callback_;
		bool is_connected_{ false };
	};

}//namespace grt


#endif//__RENDERING_SERVER_CLIENT_H__