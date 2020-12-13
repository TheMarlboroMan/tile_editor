#pragma once

//local
#include "states.h"
#include "app/exchange_data.h"
#include "editor_types/property_manager.h"

#include <dfw/controller_interface.h>
#include <ldtools/ttf_manager.h>
#include <tools/options_menu.h>
#include <lm/logger.h>

namespace controller {

//!Controller to modify property tables.
class properties:
	public dfw::controller_interface {

	public:

								properties(lm::logger&, ldtools::ttf_manager&, tile_editor::exchange_data&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/);
	virtual void 				slumber(dfw::input& /*input*/);
	virtual bool				can_leave_state() const {return true;}

	private:

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager&       ttf_manager;
	tile_editor::exchange_data& exchange_data;

	enum class datatypes {
		t_int, t_string, t_double, t_special
	};

	struct keytype {
		datatypes               type;
		std::string             name;
	};

	//properties
	tile_editor::property_manager * property_manager{nullptr};
	//TODO: will never work, not a valid key!!!
	tools::options_menu<keytype>    menu;
	keytype                         current_key{datatypes::t_special, ""};
};

}
