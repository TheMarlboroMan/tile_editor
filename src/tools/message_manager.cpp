#include "tools/message_manager.h"

#include <algorithm>
#include <stdexcept>

using namespace tools;

message_manager::message_manager(
	float _max_time
):max_time{_max_time}
{

}

void message_manager::add(const std::string& _msg) {

	messages.push_back({_msg, 0.f});
	notify(notify_event_type::add);
}

void message_manager::tick(float _delta) {

	auto count=messages.size();

	for(auto& m : messages) {

		m.age+=_delta;
	}

	auto it=std::remove_if(
		std::begin(messages),
		std::end(messages),
		[this](const msg& _msg) {

			return _msg.age > max_time;
		}
	);

	messages.erase(it, std::end(messages));

	if(count != messages.size()) {

		notify(notify_event_type::expire);
	}
}

void message_manager::clear() {

	messages.clear();
	notify(notify_event_type::clear);
}

std::vector<std::string> message_manager::get() const {

	std::vector<std::string> result(messages.size());
	std::transform(
		std::begin(messages),
		std::end(messages),
		std::begin(result),
		[](const msg& _msg) {

			return _msg.message;
		}
	);

	std::reverse(std::begin(result), std::end(result));

	return result;
}

std::vector<std::string> message_manager::get(std::size_t _count) const {

	auto result=get();

	if(result.size() < _count) {

		return result;
	}

	result.resize(_count);
	return result;
}

std::string message_manager::last() const {

	return messages.back().message;
}

void message_manager::subscribe(
	const std::string& _key,
	message_manager::notify_callback _fn
) {
	if(subscribers.count(_key)) {

		throw message_manager_exception(std::string{"subscriber key "}+_key+" already exists");
	}

	subscribers.emplace(_key, _fn);
}

void message_manager::unsubscribe(
	const std::string& _key
) {
	if(!subscribers.count(_key)) {

		throw message_manager_exception(std::string{"subscriber key "}+_key+" does not exist");
	}

	subscribers.erase(_key);
}

void message_manager::notify(message_manager::notify_event_type _type) {

	for(const auto& ref : subscribers) {

		ref.second(_type);
	}
}
