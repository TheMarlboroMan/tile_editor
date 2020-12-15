#pragma once

//local
#include "states.h"
#include "app/exchange_data.h"
#include "editor_types/map.h"

#include <dfw/controller_interface.h>
#include <lm/logger.h>
#include <ldtools/ttf_manager.h>

#include <cmath>

namespace controller {

class layer_selector:
	public dfw::controller_interface {

	public:

								layer_selector(lm::logger&, ldtools::ttf_manager&, tile_editor::exchange_data&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/);
	virtual void 				slumber(dfw::input& /*input*/);
	virtual bool				can_leave_state() const {return true;}
	void                        request_draw(dfw::controller_view_manager&);

	private:

	void                        insert_layer();
	void                        delete_layer();


	//references...
	lm::logger&					log;
	ldtools::ttf_manager&       ttf_manager;
	tile_editor::exchange_data& exchange_data;

	//properties
	std::vector<tile_editor::map::layerptr> *   layers;
};

}
