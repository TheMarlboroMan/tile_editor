#pragma once

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <functional>

namespace tools {

class message_manager_exception
	: public std::runtime_error {
	public:
	message_manager_exception(const std::string& _str):
		std::runtime_error(_str) {}
};

class message_manager {

	public:

	enum class notify_event_type {
		add, expire, clear
	};

	using notify_callback=std::function<void (notify_event_type)>;

	                    message_manager(float);

	//! Adds a new message to the queue.
	void                add(const std::string&);

	//! Ticks the messages so they can be deleted after time has passed.
	void                tick(float);

	//! Returns the size of the message queue.
	std::size_t         size() const {return messages.size();}

	//! Clears all messages
	void                clear();

	//! Gets all messages, with the newest in the first place.
	std::vector<std::string>    get() const;

	//! Gets the last N messages (less if N > than the message queue size) with the
	//! newest in the first place.
	std::vector<std::string>    get(std::size_t) const;

	//! Returns the last message. Throws it there's none.
	std::string                 last() const;

	//! Subscribes a function to changes in the container (add, clear, expiration) through a key.
	void                        subscribe(const std::string&, notify_callback);

	//! Unsubscribes the given key from change in the container.
	void                        unsubscribe(const std::string&);

	private:

	void                        notify(notify_event_type);

	float                       max_time;

	struct msg {
		std::string             message;
		float                   age;
	};

	std::deque<msg>             messages;
	std::map<std::string, notify_callback>   subscribers;

};

}
