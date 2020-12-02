#include "tools/message_manager.h"
#include "../tests.h"

#include <iostream>

struct subscriber {

	void                    subscribe(tools::message_manager& _mm, const std::string& _key) {

		_mm.subscribe(_key, [this](tools::message_manager::notify_event_type _type) {

			notify(_type);
		});
	}

	void                    notify(tools::message_manager::notify_event_type _type) {

		if(!must_receive) {

			throw std::runtime_error("subscriber should have not notified");
		}

		if(_type!=expected) {

			throw std::runtime_error("subscriber got unexpected notification type");
		}

		++notify_count;
	}

	bool                    must_receive=true;
	int                     notify_count=0;
	tools::message_manager::notify_event_type expected=tools::message_manager::notify_event_type::add;
};

int main(int /*_argc*/, char ** /*_argv*/) {

	tools::message_manager mm(10.f);

	try {

		//Testing the message manager observable behaviour...
		assert(0==mm.size(), "message_manager is empty");
		mm.add("Hello!");
		assert(1==mm.size(), "message_manager has one message");
		mm.tick(5.f);
		mm.add("World!");
		assert(2==mm.size(), "message_manager has still two messages");
		mm.tick(5.f);
		assert(2==mm.size(), "message_manager has two messages");
		mm.tick(1.f);
		assert(1==mm.size(), "message_manager had one message expire");
		mm.tick(5.f);
		assert(0==mm.size(), "message_manager has all messages expired");
		mm.add("a!");
		mm.add("b!");
		mm.add("c!");

		auto all=mm.get();

		assert(3==all.size(), "get returned 3 messages");
		assert("c!"==all[0], "first message");
		assert("b!"==all[1], "second message");
		assert("a!"==all[2], "third message");

		assert("c!"==mm.last(), "last message");

		auto part=mm.get(2);
		assert(2==part.size(), "get(2) returned 2 messages");
		assert("c!"==part[0], "first message");
		assert("b!"==part[1], "second message");

		auto whole=mm.get(20);
		assert(3==whole.size(), "get(20) returned 3 messages");
		assert("c!"==whole[0], "first message");
		assert("b!"==whole[1], "second message");
		assert("a!"==whole[2], "third message");

		mm.clear();
		assert(0==mm.size(), "message_manager got cleared");

		//Testings subscriptions...
		subscriber sa{};
		sa.subscribe(mm, "subscriber_a");

		try {
			sa.subscribe(mm, "subscriber_a");
			assert(false, "cannot subscribe twice");
		}
		catch(tools::message_manager_exception& e) {

			assert(true, "cannot subscribe twice");
		}

		mm.add("a!");
		assert(sa.notify_count==1, "subscriber a was notified of addition");

		subscriber sb{};
		sb.subscribe(mm, "subscriber_b");

		mm.tick(6.f);

		mm.add("b!");
		assert(sa.notify_count==2, "subscriber a was notified of addition, again");
		assert(sb.notify_count==1, "subscriber b was notified of addition, again");
		assert(true, "expire notifications were received");

		sa.expected=tools::message_manager::notify_event_type::expire;
		sb.expected=tools::message_manager::notify_event_type::expire;
		mm.tick(11.f);
		assert(sa.notify_count==3, "subscriber a was notified of expiry");
		assert(sb.notify_count==2, "subscriber b was notified of expiry");

		sa.expected=tools::message_manager::notify_event_type::clear;
		sb.expected=tools::message_manager::notify_event_type::clear;
		mm.clear();
		assert(sa.notify_count==4, "subscriber a was notified of clear");
		assert(sb.notify_count==3, "subscriber b was notified of clear");

		mm.unsubscribe("subscriber_b");
		sb.must_receive=false;

		sa.expected=tools::message_manager::notify_event_type::add;
		mm.add("c!");
		assert(sa.notify_count==5, "subscriber a was notified of addition");

		mm.unsubscribe("subscriber_a");
		sa.must_receive=false;

		try {
			mm.unsubscribe("subscriber_none");
			assert(false, "cannot unsubscribe from non-existing key");
		}
		catch(tools::message_manager_exception& e) {

			assert(true, "cannot unsubscribe from non-existing key");
		}

		mm.clear();
	}
	catch(std::exception& e) {

		std::cerr<<"failed : "<<e.what()<<std::endl;
		return 1;
	}

	std::cout<<"all done"<<std::endl;
	return 0;
}
