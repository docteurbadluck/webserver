/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_http_response_login.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdeliot <tdeliot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:40:48 by tdeliot           #+#    #+#             */
/*   Updated: 2025/09/03 17:41:43 by tdeliot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostLoginHandler.hpp"

std::string PostLoginHandler::build_http_response(int status_code,
												const std::string &body,
												const std::string &error_page_filepath,
												const std::string &filepath)
{
	(void)body;
	(void)filepath;

	std::ostringstream buffer;
	std::string response_body = load_response_body(status_code, error_page_filepath);
	buffer << status_line(status_code);
	buffer << build_headers(response_body);
	if (status_code != 204)
		buffer << response_body;
    close_flag = find_out_close_flag(buffer.str());
	return buffer.str();
}

std::string PostLoginHandler::load_response_body(int status_code,
												const std::string &error_page_filepath)
{
	std::string response_body;

	if (status_code != 200 && status_code != 204 && !error_page_filepath.empty())
	{
		std::ifstream file(error_page_filepath.c_str());
		if (file)
		{
			std::ostringstream file_content;
			file_content << file.rdbuf();
			response_body = file_content.str();
		}
	}
	else if (status_code == 200)
	{
		response_body = "<html><body>Login successful</body></html>";
	}
	return response_body;
}

std::string PostLoginHandler::status_line(int status_code)
{
	if (status_code == 204)
		return this->http_version + " 204 No Content\r\n";
	else if (status_code == 403)
		return this->http_version + " 403 Forbidden\r\n";
	else if (status_code == 500)
		return this->http_version +" 500 Internal Server Error\r\n";
	return this->http_version +" 200 OK\r\n";
}

std::string PostLoginHandler::build_headers(const std::string &response_body)
{
	std::ostringstream buffer;

	buffer << handler_connection_type();
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: text/html\r\n";

	if (this->set_cookie_flag == false)
	{
		buffer << "Set-Cookie: session_id="
		       << this->session_handler->generate_session_id()
		       << "; Path=/\r\n";
	}
	buffer << "\r\n";
	return buffer.str();
}
