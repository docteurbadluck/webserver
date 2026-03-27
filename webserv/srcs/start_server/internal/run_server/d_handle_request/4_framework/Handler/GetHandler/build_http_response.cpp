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
	std::string response_body;

	if (status_code != 200)
	{
		std::ostringstream code_str;
		code_str << status_code;
		response_body = load_error_page(error_page_filepath);
		response_body = replace_all_occurrences(response_body, "{{statusCode}}", code_str.str());
		response_body = replace_all_occurrences(response_body, "{{reasonPhrase}}", reason_phrase(status_code));
	}
	else
		response_body = interpret_html_body(body, this->session_handler);

	buffer << status_line(status_code);
	buffer << build_headers(response_body, filepath, status_code);
    close_flag = find_out_close_flag(buffer.str());
	if (this->fd_stream == -1)
		buffer << response_body;
	return buffer.str();
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
		this->fd_stream = open(filepath.c_str(), O_RDONLY| O_NONBLOCK);

	if (this->set_cookie_flag == false)
	{
		buffer	<< "Set-Cookie: session_id="
				<< this->session_handler->generate_session_id()
				<< "; Path=/\r\n";
	}
	buffer << "\r\n";
	return buffer.str();
}
