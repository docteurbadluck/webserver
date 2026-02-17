#include "SessionStorageFramework.hpp"
#include "ISessionPolicy.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

int SessionStorageFramework::compte_current_session()
{
	return (this->sessions.size());
}

void SessionStorageFramework::store_session(t_session_data session_data)
{
	std::map<std::string, t_session_data>::iterator it;

	it = sessions.find(session_data.session_id);
	if (it == sessions.end())
		this->sessions[session_data.session_id] = session_data;
}

void	SessionStorageFramework::purge_expired_sessions(ISessionPolicy &policy)
{
	std::vector<std::string> to_delete;
	for (std::map<std::string, t_session_data>::iterator it = sessions.begin(); it != sessions.end(); ++it)
	{
		if (policy.is_this_session_expired(it->second))
			to_delete.push_back(it->second.session_id);
	}
	for (std::vector<std::string>::iterator it = to_delete.begin(); it != to_delete.end(); ++it)
	{
		delete_session(*it);
	}
}

void SessionStorageFramework::delete_session(const std::string &session_id)
{
	this->sessions.erase(session_id);
}

t_session_data &SessionStorageFramework::get_session(const std::string &session_id)
{
	std::map<std::string, t_session_data>::iterator it = sessions.find(session_id);
	if (it != sessions.end())
		return (this->sessions[session_id]);
	throw std::runtime_error("Session ID not found");
}

std::string SessionStorageFramework::generate_session_id() const 
{
	unsigned char buf[16];
	std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
	std::ostringstream result_stream;


	if (!urandom)
		throw std::runtime_error("Cannot open /dev/urandom");
	urandom.read(reinterpret_cast<char*>(buf), sizeof(buf));
	urandom.close();
	for (int i = 0; i < 16; i++)
		result_stream << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];
	return (result_stream.str());
}

bool	SessionStorageFramework::is_valid_session(const std::string &session_id) const
{
	std::map<std::string, t_session_data>::const_iterator it = sessions.find(session_id);
	if (it == sessions.end())
		return (false);
	return (true);
}
