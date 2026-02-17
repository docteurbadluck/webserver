/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_http_response.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdeliot <tdeliot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:22:48 by tdeliot           #+#    #+#             */
/*   Updated: 2025/09/11 18:02:00 by tdeliot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetHandler.hpp"

std::string GetHandler::build_http_response(int status_code,
											const std::string &body,
											const std::string &error_page_filepath,
											const std::string &filepath)
{
	std::ostringstream buffer;
	std::string response_body = load_response_body(status_code, body, error_page_filepath);

	buffer << status_line(status_code);
	buffer << build_headers(response_body, filepath, status_code);
    close_flag = find_out_close_flag(buffer.str());
	if (this->fd_stream == -1)
		buffer << response_body;
	return buffer.str();
}

std::string GetHandler::load_response_body(int status_code,
											const std::string &body,
											const std::string &error_page_filepath)
{
	std::string response_body = interpret_html_body(body, this->session_handler);

	if (status_code != 200 && !error_page_filepath.empty())
	{
		std::ifstream file(error_page_filepath.c_str());
		if (file)
		{
			std::ostringstream file_content;
			file_content << file.rdbuf();
			response_body = file_content.str();
		}
	}
	return response_body;
}

std::string GetHandler::status_line(int status_code)
{
	if (status_code == 404)
		return this->http_version + " 404 Not Found\r\n";
	else if (status_code == 403)
		return this->http_version + " 403 Forbidden\r\n";
	else if (status_code == 500)
		return this->http_version + " 500 Internal Server Error\r\n";
	return this->http_version + " 200 OK\r\n";
}

std::string GetHandler::build_headers(const std::string &response_body,
										const std::string &filepath,
										int status_code)
{
	std::ostringstream buffer;
	buffer << handler_connection_type();
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	
	if (get_mime_type(filepath, status_code) != "text/plain" && get_mime_type(filepath, status_code) != "text/html" )
	{
		this->fd_stream = open(filepath.c_str(), O_RDONLY| O_NONBLOCK);
	}

	if (this->set_cookie_flag == false)
	{
		buffer	<< "Set-Cookie: session_id="
				<< this->session_handler->generate_session_id()
				<< "; Path=/\r\n";
	}
	buffer << "\r\n";
	return buffer.str();
}

