/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bind_socket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdeliot <tdeliot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 12:48:44 by tdeliot           #+#    #+#             */
/*   Updated: 2025/09/03 12:50:32 by tdeliot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <iostream>

void	ServerSocket::bind_socket(unsigned long ip, int port)
{
	struct sockaddr_in	addr;

	test_input(ip, port);
	init_sockaddr(port, ip, &addr);
	if (bind(this->sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		throw std::runtime_error("Bind failed");
	}
	print_socket_ip(port, ip);
}
void	ServerSocket::test_input(unsigned long ip, int port)
{
	if (port <= 0 || port > 65535)
		throw std::runtime_error("Invalid port");
	if (ip == 0)
		throw std::runtime_error("Invalid IP (INADDR_ANY not allowed)");
}

void	ServerSocket::init_sockaddr(int port, unsigned long ip, struct sockaddr_in *addr)
{
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = htonl(ip);
}

void	ServerSocket::print_socket_ip(int port, unsigned long ip)
{
	unsigned char		bytes[4];
	char				ip_str[16];

	bytes[0] = (ip >>24) & 0xFF;
	bytes[1] = (ip >>16) & 0xFF;
	bytes[2] = (ip >>8) & 0xFF;
	bytes[3] = ip & 0xFF;	
	snprintf(ip_str, sizeof(ip_str), "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]); // forbiden
	std::cout << "socket bind on : " << ip_str << "::" << port << std::endl;
}