#include "SessionHandlerUC.hpp"
#include "ISessionStorage.hpp"
#include "ISessionPolicy.hpp"
#include <iostream>


void SessionHandlerUC::purge_expired_sessions()
{
	this->storage.purge_expired_sessions(this->policy);
}


const t_session_data &SessionHandlerUC::get_session(const std::string &session_id)
{
	return this->storage.get_session(session_id);
}


void SessionHandlerUC::modif_session_data(const std::string &session_id,
											const std::string &field,
											const std::string &new_value)
{
	t_session_data &session = this->storage.get_session(session_id);

	if (field == "session_id")
		session.session_id = new_value;
	else if (field == "username")
		session.username = new_value;
}

bool SessionHandlerUC::is_valid_session(const std::string &session_id)
{
	return (this->storage.is_valid_session(session_id));
}

void SessionHandlerUC::delete_session(const std::string &session_id)
{
	this->storage.delete_session(session_id);
}

std::string SessionHandlerUC::generate_session_id() const 
{
	return (this->storage.generate_session_id());
}

