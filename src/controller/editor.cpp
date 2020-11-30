#include "../../include/controller/editor.h"

//local
#include "../../include/input/input.h"

using namespace controller;

editor::editor(lm::logger& plog)
	:log(plog) {

}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {
		set_leave(true);
		return;
	}
}

void editor::draw(ldv::screen& screen, int /*fps*/) {
	screen.clear(ldv::rgba8(0, 0, 0, 255));

	//TODO: Why don't we start by drawing the messages???
}

void editor::add_message(const std::string& /*_msg*/) {

	//TODO: Add the message to some queue.
}
