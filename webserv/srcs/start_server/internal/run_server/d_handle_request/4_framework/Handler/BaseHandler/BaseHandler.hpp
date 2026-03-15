#pragma once
#include "parsed_request.hpp"
#include "SessionHandlerUC.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>   // open, O_RDONLY, O_NONBLOCK
#include <unistd.h>  // close

class BaseHandler
{
	public :
		BaseHandler();
		virtual ~BaseHandler();
		virtual std::string handle(const t_parsed_request &req,
								const t_server_rules &server_rules,
								SessionHandlerUC &session_handler) = 0;
		std::vector<std::string>	get_file_content();
		int							get_fd_stream();
		pid_t						get_cgi_pid();


		int							close_flag; 
	protected :
		virtual void		verify_mandatory_field(const t_parsed_request &req) = 0;
		virtual std::string build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath) = 0;

		virtual std::string interpret_html_body(const std::string body, SessionHandlerUC *session_handler);
		std::string 		add_default_file_if_directory(const std::string &path, const t_server_rules &server_rules);
		virtual std::string 		build_file_path(const std::string &path,
									const t_server_rules &server_rules);
		virtual int 		check_file_status(const std::string &file_path, const t_server_rules &server_rules);
		std::string read_file_content(const std::string &file_path);
		std::string map_to_url_path(const std::string &path,
		const std::pair<std::string, std::string> &file_system_root);
		std::string get_mime_type(const std::string &path, int status_code);
		void		init_field(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);
		bool manage_file_system_root(const std::string &path, const t_server_rules &server_rules, std::string &file_path, std::string &relative_path);
		std::string normalize_directory(const std::string &file_path);
		std::string append_default_file(const std::string &file_path,
										const t_server_rules &server_rules);
		std::vector<std::string> 	read_file_chunks(const std::string &file_path, std::size_t chunk_size);
		std::string					handler_connection_type();
		int							find_out_close_flag(const std::string &answer);
		std::string					load_error_page(const std::string &error_page_filepath);
		std::string					status_line(int status_code);
		static std::string			reason_phrase(int status_code);
		static std::string			replace_all_occurrences(const std::string &src, const std::string &from, const std::string &to);
		std::string					build_file_path_simple(const std::string &path, const t_server_rules &server_rules);
		std::string					build_standard_response(int status_code,
										const std::string &response_body,
										const std::string &filepath,
										bool include_cookie,
										bool force_close);
		std::vector<std::string>	file_content;
		std::string					http_version;
		bool						directory_listing;
		bool						set_cookie_flag;
		t_parsed_request			request;
		SessionHandlerUC			*session_handler;
		t_server_rules				server_rules;

		int							fd_stream;
		pid_t						cgi_pid;
};

