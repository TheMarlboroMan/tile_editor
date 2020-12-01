#pragma once

#include <string>
#include <vector>

namespace tools {

class message_manager {

	public:

	                    message_manager(int);

	//! Adds a new message to the queue.
	void                add(const std::string&);

	//! Ticks the messages so they can be deleted after time has passed.
	void                tick(float);

	//! Returns the size of the message queue.
	std::size_t         size() const {return messages.size();}

	//! Clears all messages
	void                clear();

	//! Gets all messages
	std::vector<std::string>    get() const;

	//! Gets the last N messages (less if N > than the message queue size).
	std::vector<std::string>    get(std::size_t) const;

	std::string                 last() const;

	private:

	float                       max_time;

	struct msg {
		std::string             message;
		float                   age;
	};

	std::vector<msg>            messages;
};

}
