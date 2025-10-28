/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buid_file_path.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdeliot <tdeliot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 16:39:12 by tdeliot           #+#    #+#             */
/*   Updated: 2025/09/11 17:33:23 by tdeliot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BaseHandler.hpp"

std::string	BaseHandler::build_file_path(const std::string &path,
							const t_server_rules &server_rules)
{
	std::string	file_path;
	std::string	relative_path;

	if (!manage_file_system_root(path, server_rules, file_path, relative_path))
		return "";
	file_path = normalize_directory(file_path);
	file_path = append_default_file(file_path, server_rules);
	return file_path;
}

bool BaseHandler::manage_file_system_root(const std::string &path,
					const t_server_rules &server_rules,
					std::string &file_path,
					std::string &relative_path)
{
	file_path = server_rules.file_system_root.second;

	if (!file_path.empty() && file_path[file_path.size() - 1] != '/')
		file_path += '/';
	relative_path = path;
	if (!server_rules.file_system_root.first.empty())
	{
		if (relative_path.find(server_rules.file_system_root.first) == 0)
		{
			if (relative_path.size() == server_rules.file_system_root.first.size() || 
				relative_path[server_rules.file_system_root.first.size()] == '/')
			relative_path = relative_path.substr(server_rules.file_system_root.first.size());
			else 
				return false;
		}
		else
			return false; 
	}
	if (!relative_path.empty() && relative_path[0] == '/')
		relative_path = relative_path.substr(1);  
	file_path += relative_path;
	return true;
}

std::string BaseHandler::normalize_directory(const std::string &file_path)
{
	struct stat st;

	if (stat(file_path.c_str(), &st) == 0
		&& S_ISDIR(st.st_mode)
		&& file_path[file_path.size() - 1] != '/')
	{
		return file_path + "/";
	}
	return file_path;
}

std::string	BaseHandler::append_default_file(const std::string &file_path,
								const t_server_rules &server_rules)
{
	if (!file_path.empty() && file_path[file_path.size() - 1] == '/' )
	{
		std::string candidate = file_path + server_rules.default_filepath;
		struct stat st;
		if (stat(candidate.c_str(), &st) == 0)
			return candidate;
	}
	return file_path;
}
