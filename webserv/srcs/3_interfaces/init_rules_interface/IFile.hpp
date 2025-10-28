#include <string>

class IFile {
public:
	virtual std::string	read() = 0;
	virtual void		close() = 0;
	virtual				~IFile() {}
};
