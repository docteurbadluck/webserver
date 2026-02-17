#include "ISessionStorage.hpp"
#include <map>
#include <stdexcept>

class SessionStorageFramework : public ISessionStorage
{
	public :
		SessionStorageFramework(){};
		t_session_data* create_session(const std::string &username);
		t_session_data	&get_session(const std::string &username);
		void			remove_inactive_session();
		bool			is_valid_session(const std::string &session_id) const;
		void			store_session(t_session_data session_data);
		void			purge_expired_sessions( ISessionPolicy &policy);
		void			delete_session(const std::string &session_id);
		int				compte_current_session();
		std::string		generate_session_id() const;

	private :
		std::map<std::string, t_session_data>	sessions;
};