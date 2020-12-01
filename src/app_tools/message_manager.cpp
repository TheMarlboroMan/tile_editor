#include "tools/message_manager.h"

#include <algorithm>

using namespace tools;

message_manager::message_manager(
	int _max_time
):max_time{_max_time}
{

}

void message_manager::add(const std::string& _msg) {

	messages.push_back({_msg, 0.f});
}

void message_manager::tick(float _delta) {

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
}

void message_manager::clear() {

	messages.clear();
}

std::vector<std::string> message_manager::get() const {

	std::vector<std::string> result;
	std::transform(
		std::begin(messages),
		std::end(messages),
		std::begin(result),
		[](const msg& _msg) {

			return _msg.message;
		}
	);

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

	return messages.at(0).message;
}
