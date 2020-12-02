#pragma once

//local
#include "states.h"
#include "tools/message_manager.h"

#include <ldv/ttf_representation.h>
#include <dfw/controller_interface.h>
#include <lm/logger.h>
#include <ldtools/ttf_manager.h>

#include <cmath>

namespace controller {

class editor:
	public dfw::controller_interface {

	public:

								editor(lm::logger&, ldtools::ttf_manager&, tools::message_manager&, unsigned int, unsigned int);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/) {}
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	ldt::point_2d<int>          get_mouse_position(dfw::input&) const;
	void                        draw_messages(ldv::screen&);
	void                        draw_hud(ldv::screen&);
	void                        zoom_in();
	void                        zoom_out();
	void                        receive_message(tools::message_manager::notify_event_type);

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager        ttf_manager;
	tools::message_manager&     message_manager;

	ldv::rect                   screen_rect;
	ldv::camera                 camera;
	ldv::ttf_representation     last_message_rep;
	ldt::point_2d<int>			mouse_pos;

};
}

