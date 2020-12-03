#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

void fail(const std::string& _msg);
void test(bool _thing, const std::string& _msg);

template<typename T> void must_throw(
	T _whatever,
	const std::string& _errmsg,
	const std::string& _type
) {

	try {
		_whatever();
		std::cerr<<"'"<<_type<<"' should have failed..."<<std::endl;
		std::exit(1);
	}
	catch(std::exception &e) {

		if(std::string{e.what()}.find(_errmsg)==std::string::npos) {

			std::cerr<<"expected '"<<_errmsg<<"', got '"<<e.what()<<"'"<<std::endl;
			std::exit(1);
		}
	}

	std::cout<<"caught: "<<_type<<std::endl;
}
