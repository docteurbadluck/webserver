/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdeliot <tdeliot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:21:48 by tdeliot           #+#    #+#             */
/*   Updated: 2025/09/11 17:59:50 by tdeliot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetHandler.hpp"

std::string	GetHandler::handle(const t_parsed_request &req,
								const t_server_rules &server_rules,
								SessionHandlerUC &session_handler) 
{
	prepare_request(req, server_rules, session_handler);

	std::string url_path = resolve_url_path(req, server_rules);
	std::string filepath;
	int status_code = resolve_file_path(url_path, server_rules, filepath);
	if (status_code != 200)
		return build_http_response(status_code, "", server_rules.error_page_filepath, filepath);
	return build_final_response(url_path, filepath, server_rules);
}

void	GetHandler::prepare_request(const t_parsed_request &req,
								const t_server_rules &server_rules,
								SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
}

std::string GetHandler::resolve_url_path(const t_parsed_request &req,
										const t_server_rules &server_rules)
{
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);
	std::string redir_response = verify_redirection(url_path, server_rules);

	if (!redir_response.empty())
		throw redir_response;
	return url_path;
}

int	GetHandler::resolve_file_path(const std::string &url_path,
								const t_server_rules &server_rules,
								std::string &filepath)
{
	filepath = build_file_path(url_path, server_rules);
	return check_file_status(filepath, server_rules);
}

std::string	GetHandler::build_final_response(const std::string &url_path,
											const std::string &filepath,
											const t_server_rules &server_rules)
{
	if (directory_listing == true)
	{
		std::string listing = build_directory_listing(filepath, url_path);
		return build_http_response(200, listing, server_rules.error_page_filepath, filepath);
	}
	std::string body = read_file_content(filepath);
	this->file_content = read_file_chunks(filepath, 8192);
	return build_http_response(200, body, server_rules.error_page_filepath, filepath);
}
