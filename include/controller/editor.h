#pragma once

//local
#include "states.h"

#include <ldv/ttf_representation.h>
#include <dfw/controller_interface.h>
#include <lm/logger.h>
#include <ldtools/ttf_manager.h>

//std
#include <cmath>

namespace controller {

class editor:
	public dfw::controller_interface {

	public:

								editor(lm::logger&, ldtools::ttf_manager&, unsigned int, unsigned int);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/) {}
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	void                        add_message(const std::string&);

	private:

	ldt::point_2d<int>          get_mouse_position(dfw::input&) const;
	void                        draw_messages(ldv::screen&);
	void                        draw_hud(ldv::screen&);
	void                        zoom_in();
	void                        zoom_out();

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager        ttf_manager;

	//properties
	struct {
		std::string             message;
		float                   time;
	}                           last_message{"", 0.0f};

	ldv::rect                   screen_rect;
	ldv::camera                 camera;
	ldv::ttf_representation     last_message_rep;
	ldt::point_2d<int>			mouse_pos;

};
}

