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
    std::string filepath;
	prepare_request(req, server_rules, session_handler);
	std::string url_path = resolve_url_path(req, server_rules);
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
	return url_path;
}

int	GetHandler::resolve_file_path(const std::string &url_path,
								const t_server_rules &server_rules,
								std::string &filepath)
{
	filepath = build_file_path(url_path, server_rules);
	return check_file_status(filepath, server_rules);
}


std::string GetHandler::build_final_response(
    const std::string &url_path,
    const std::string &filepath,
    const t_server_rules &server_rules)
{
    if (!this->directory_listing)
    {
        std::string body = read_file_content(filepath);
        this->file_content = read_file_chunks(filepath, 8192);
        return build_http_response(200, body, server_rules.error_page_filepath, filepath);
    }

    std::string index_file = filepath;
    if (!index_file.empty() && index_file[index_file.size() - 1] != '/')
        index_file += "/";
    index_file += server_rules.default_filepath;

    struct stat st_index;
    if (stat(index_file.c_str(), &st_index) == 0)
    {
        std::string body = read_file_content(index_file);
        this->file_content = read_file_chunks(index_file, 8192);
        return build_http_response(200, body, server_rules.error_page_filepath, index_file);
    }
    std::string listing = build_directory_listing(filepath, url_path);
    return build_http_response(200, listing, server_rules.error_page_filepath, filepath);
}

