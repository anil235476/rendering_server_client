#ifndef __RENDERING_SERVER_CLIENT_H__
#define __RENDERING_SERVER_CLIENT_H__
#include "signaller.h"
#include "json_parser.h"
#include <functional>
#include <map>
#include <future>
#include "func_thread_handler.h"
#include "websocket_signaller.h"
constexpr const char* RENDERING_SERVER_IP = "localhost";
constexpr const char* RENDERING_SERVER_PORT = "8002";

constexpr const char* mic_toggle_id = "mic_toggle";
constexpr const char* cam_toggle_id = "cam_toggle";
constexpr const char* error_id = "error";

namespace grt {

	using function_callback = std::function<void(message_type, absl::any msg, absl::optional<absl::any> unparsed_msg)>;

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
		void on_message(message_type, absl::any msg, absl::optional<absl::any> unparsed_msg) override;

	};

	class sender {
	public:
		
		virtual ~sender() {}
		//void connect(std::string address, std::string port);
		virtual std::future<bool> sync_connect(std::string address, std::string port) = 0;
		virtual void send_to_renderer(std::string id, std::string message, function_callback response) = 0;
		virtual void done(std::string id) = 0;
		virtual void register_for_session_leave_msg(function_callback response) = 0;
		virtual void register_for_message(std::string id, function_callback response) = 0;
	
	};

	class server_sender : public sender {
	public:
		server_sender();
		~server_sender() override;
		
		std::future<bool> sync_connect(std::string address, std::string port) override;
		void send_to_renderer(std::string id, std::string message, function_callback response) override;
		void done(std::string id) override;
		void register_for_session_leave_msg(function_callback response) override;
		void register_for_message(std::string id, function_callback response) override;
	private:
		util::func_thread_handler function_thread_;
		websocket_signaller_unsecure signaller_;
		std::shared_ptr< rendering_server_client> server_callback_;
		bool is_connected_{ false };
	};

	util::func_thread_handler* get_renderer_function_thread();
	class local_sender : public sender {
	public:
		local_sender();
		~local_sender();
		std::future<bool> sync_connect(std::string address, std::string port) override;
		void send_to_renderer(std::string id, std::string message, function_callback response) override;
		void done(std::string id) override;
		void register_for_session_leave_msg(function_callback response) override;
		void register_for_message(std::string id, function_callback response) override;
	private:
		std::shared_ptr< rendering_server_client> server_callback_;
		util::func_thread_handler* sender_{ get_renderer_function_thread() };
	};

	std::unique_ptr< sender> get_rendering_server_client();




}//namespace grt


#endif//__RENDERING_SERVER_CLIENT_H__