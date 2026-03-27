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
	std::string response_body;

	if (status_code != 200 && status_code != 204)
		response_body = load_error_page(error_page_filepath);
	else if (status_code == 200)
		response_body = "<html><body>Login successful</body></html>";

	return build_standard_response(status_code, response_body, "", true, false);
}
