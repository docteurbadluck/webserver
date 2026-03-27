#pragma once
#include "BaseHandler.hpp"

class GetHandler : public BaseHandler
{
	public :
		GetHandler();
		~GetHandler();
		std::string handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);
		int			get_is_big_body();

	private :
		void		verify_mandatory_field(const t_parsed_request &req);
		std::string	build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath);
		std::string	build_directory_listing(const std::string &filepath, const std::string &url_path);
		std::string	interpret_html_body(const std::string body, SessionHandlerUC *session_handler);
		void		prepare_request(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);
		int			resolve_file_path(const std::string &url_path, const t_server_rules &server_rules, std::string &filepath);
		std::string	build_final_response(const std::string &url_path, const std::string &filepath, const t_server_rules &server_rules);
		std::string	resolve_url_path(const t_parsed_request &req, const t_server_rules &server_rules);
		std::string	build_headers(const std::string &response_body,
									const std::string &filepath,
									int status_code);
};



