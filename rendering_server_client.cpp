#include "rendering_server_client.h"
#include <cassert>
#include <iostream>

namespace grt {
	
	constexpr const char* leave_session_id = "session_leave_req";

	void rendering_server_client::register_function(std::string id, function_callback callback) {
		//assert(register_functions_.find(id) == register_functions_.end());
		auto r = register_functions_.emplace(id, callback);
		assert(r.second);
	}

	void rendering_server_client::unregister_function(std::string id) {
		const auto count = register_functions_.erase(id);
		assert(count == 1);
	}

	void rendering_server_client::set_connect_event(std::promise<bool> receiver) {
		connect_event_ = std::move(receiver);
	}

	void rendering_server_client::on_message(std::string msg) {
		async_parse_message(msg, this);
	}

	void rendering_server_client::on_connect() {
		std::cout << "on connect called\n";
		connect_event_.set_value(true);
	}


	void rendering_server_client::on_error(std::string error) {
		std::cout << "renderering server client on error " << error<<'\n';
		std::cout << "handle this condition soon\n";
		//assert(false);
		auto iter = register_functions_.find(error_id);
		assert(iter != register_functions_.end());
		iter->second(message_type::connection_error, error + "rendering server close error", {});
	}

	void rendering_server_client::on_close() {

	}

	void rendering_server_client::on_message(message_type type, absl::any msg, absl::optional<absl::any> unparsed_msg) {
		switch (type) {
		case message_type::window_create_res:
		{
			const auto res = absl::any_cast<wnd_create_res>(msg);
			auto iter = register_functions_.find(res.id);
			assert(iter != register_functions_.end());
			iter->second(type, msg, {});
		}
			break;
		case message_type::wnd_close_req_res:
		{
			const auto res = absl::any_cast<std::pair<bool, std::string>>(msg);
			auto iter = register_functions_.find(res.second);
			assert(iter != register_functions_.end());
			iter->second(type, msg, {});
		}
			break;
		case message_type::session_leave_req:
		{
			auto iter = register_functions_.find(leave_session_id);
			assert(iter != register_functions_.end());
			iter->second(type, msg, {});
			break;
		}
		case message_type::cam_toggle:
		{
			auto iter = register_functions_.find(cam_toggle_id);
			assert(iter != register_functions_.end());
			iter->second(type, msg, {});
			break;
		}
		case message_type::mic_toggle:
		{
			auto iter = register_functions_.find(mic_toggle_id);
			assert(iter != register_functions_.end());
			iter->second(type, msg, {});
			break;
		}
		default:
			assert(false);
			break;
		}
	}



	server_sender::server_sender() = default;

	server_sender::~server_sender() {
		if (is_connected_) {
			signaller_.disconnect();
		}
	}
	//void sender::connect(std::string address, std::string port) {
	//	server_callback_ = std::make_shared<rendering_server_client>();
	//	
	//	auto future = sync_connect(address, port); //connect_event.get_future();
	//	std::thread{ [this, future = std::move(future)]() mutable{
	//		auto status = future.wait_for(std::chrono::seconds(5));
	//		assert(status != std::future_status::timeout);
	//		 is_connected_ = future.get();
	//		 assert(is_connected_);
	//		} }.detach();
	//}

	std::future<bool> 
		server_sender::sync_connect(std::string address, std::string port) {
		assert(server_callback_.get() == nullptr);
		server_callback_ = std::make_shared<rendering_server_client>();
		assert(server_callback_);
		std::promise<bool> connect_event;
		auto future = connect_event.get_future();
		server_callback_->set_connect_event(std::move(connect_event));
		signaller_.connect(address, port, server_callback_);
		function_thread_.register_id(SENDER_ID, [this](std::string m) {
			signaller_.send(m);
		});
		is_connected_ = true;
		return future;
	}

	void server_sender::send_to_renderer(std::string id, std::string message, function_callback response) {
		server_callback_->register_function(id, response);
		function_thread_.dispatch(SENDER_ID, message);
	}
	void server_sender::done(std::string id){
		server_callback_->unregister_function(id);
	}

	void server_sender::register_for_session_leave_msg(
		function_callback response) {
		this->register_for_message(leave_session_id, response);
	}

	void server_sender::register_for_message(std::string id, function_callback response) {
		server_callback_->register_function(id, response);
	}



	std::unique_ptr< sender> get_rendering_server_client() {
		return std::make_unique< server_sender>();
	}

	
}//namespace grt
