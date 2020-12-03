#include "tests.h"

void fail(const std::string& _msg) {

	throw std::runtime_error(_msg);
}

void test(bool _thing, const std::string& _msg) {

	std::cout<<"asserting "<<_msg<<" ..."<<std::endl;

	if(!_thing) {
		fail(_msg);
	}
}
