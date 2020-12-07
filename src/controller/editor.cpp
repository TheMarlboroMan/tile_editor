#include "controller/editor.h"

//local
#include "input/input.h"
#include "app/map_loader.h"
#include "app/map_saver.h"
#include "app/map_loader.h"
#include "tile_editor/parser/blueprint_parser.h"
#include "tile_editor/editor_types/tile_layer.h"
#include "tile_editor/editor_types/thing_layer.h"
#include "tile_editor/editor_types/poly_layer.h"


#include <lm/sentry.h>
#include <ldv/line_representation.h>
#include <ldv/bitmap_representation.h>
#include <ldv/box_representation.h>

using namespace controller;

editor::editor(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	tools::message_manager& _message_manager,
	tile_editor::exchange_data& _exchange_data,
	unsigned int _screen_w,
	unsigned int _screen_h
)
	:log(_log),
	ttf_manager(_ttf_manager),
	message_manager{_message_manager},
	exchange_data{_exchange_data},
	screen_rect{0, 0, _screen_w, _screen_h},
	camera{
		screen_rect, //pointing at world 0,0.
		{0,0}           //at 0.0 in the screen...
	},
	last_message_rep{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 255),
	},
	mouse_pos{0,0}
{

	message_manager.subscribe("editor", [this](tools::message_manager::notify_event_type _type) {

		receive_message(_type);
	});
}

void editor::awake(dfw::input& /*_input*/) {

	lm::log(log, lm::lvl::info)<<"map editor controller awakens"<<std::endl;

	if(exchange_data.has(state_editor)) {

		exchange_data.recover(state_editor);
		if(exchange_data.file_browser_success) {

			//Save as requested...
			if(exchange_data.file_browser_allow_create) {

				current_filename=exchange_data.file_browser_choice;
				lm::log(log, lm::lvl::info)<<"map editor changes current filename to "<<current_filename<<std::endl;
				save_current();
			}
			//Entry point for when loading of a map was requested...
			else {

				load_map(exchange_data.file_browser_choice);
			}
		}
	}
}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& /*_lid*/) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {
		set_leave(true);
		return;
	}

	if(_input.is_input_down(input::help)) {

		push_state(state_help);
	}

	if(_input.is_input_down(input::load)) {

		exchange_data.file_browser_allow_create=false;
		exchange_data.file_browser_title="Load map file";
		exchange_data.file_browser_invoker_id=state_editor;
		exchange_data.put(state_file_browser);
		push_state(state_file_browser);
		return;
	}

	if(_input.is_input_down(input::save)) {

		if(_input.is_input_pressed(input::left_control)) {

			exchange_data.file_browser_allow_create=true;
			exchange_data.file_browser_title="Save map file as";
			exchange_data.file_browser_invoker_id=state_editor;
			exchange_data.put(state_file_browser);
			push_state(state_file_browser);
			return;
		}
		else {

			if(!current_filename.size()) {

				message_manager.add("cannot save without current filename, use ctrl+s");
				return;
			}
			save_current();
		}
	}

	mouse_pos=get_mouse_position(_input);

	if(_input.is_input_down(input::zoom_in)) {

		zoom_in();
		return;
	}
	else if(_input.is_input_down(input::zoom_out)) {

		zoom_out();
		return;
	}

	if(_input.is_input_down(input::pageup)) {

		previous_layer();
		return;
	}
	else if(_input.is_input_down(input::pagedown)) {

		next_layer();
		return;
	}

	if(_input.is_input_down(input::space)) {

		toggle_layer_draw_mode();
		return;
	}

	typedef  bool (dfw::input::*input_fn)(int) const;
	input_fn movement_fn=_input.is_input_pressed(input::left_control)
		? &dfw::input::is_input_down
		: &dfw::input::is_input_pressed;

	const int factor=_input.is_input_pressed(input::left_control) 
		? session.grid_data.size / 4
		: session.grid_data.size / 2;

	int movement_x=0,
		movement_y=0;

	if(std::invoke(movement_fn, _input, input::up)) {

		movement_y=-1*factor;
	}
	if(std::invoke(movement_fn, _input, input::down)) {

		movement_y=1*factor;
	}

	if(std::invoke(movement_fn, _input, input::left)) {

		movement_x=-1*factor;
	}
	if(std::invoke(movement_fn, _input, input::right)) {

		movement_x=1*factor;
	}

	if(movement_x || movement_y) {

		camera.move_by(movement_x, movement_y);
//		perform_movement(
//			movement_x,
//			movement_y,
//			_input.is_input_pressed(input::resize),
//			_input.is_input_pressed(input::align)
//		);
		return;
	}
}

void editor::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(session.bg_color.r, session.bg_color.g, session.bg_color.b, session.bg_color.a));

	draw_grid(_screen);
	draw_layers(_screen);
	draw_messages(_screen);
	draw_hud(_screen);
}

void editor::draw_messages(ldv::screen& _screen) {

	last_message_rep.draw(_screen);
}

void editor::draw_grid(
	ldv::screen& _screen
) {
	auto euclidean_module=[](int a, int b) -> int {

		int m=a%b;
		if(m < 0) {
			return b < 0 ? m-b : m+b;
		}
		return m;
	};

	auto to_color=[](const tile_editor::color _c) -> ldv::rgba_color {

		return ldv::rgba8(_c.r, _c.g, _c.b, _c.a);
	};

	const auto& focus=camera.get_focus_box();

	int x_max=focus.origin.x + focus.w;
	int y_max=focus.origin.y + focus.h;

	//Horizontal lines...
	int module=euclidean_module(focus.origin.x, session.grid_data.size);
	int x=focus.origin.x-module;
	int index=0;
	while(x < x_max) {

		ldv::line_representation line(
			{x, focus.origin.y}, 
			{x, y_max},
			index % session.grid_data.horizontal_ruler 
				? to_color(session.grid_data.color)
				: to_color(session.grid_data.ruler_color)
		);

		line.draw(_screen, camera);

		x+=session.grid_data.size;
		++index;
	}

	//Horizontal lines...
	module=euclidean_module(focus.origin.y, session.grid_data.size);
	int y=focus.origin.y-module;
	index=0;
	while(y < y_max) {

		ldv::line_representation line(
			{focus.origin.x, y}, 
			{x_max, y},
			index % session.grid_data.vertical_ruler 
				? to_color(session.grid_data.color)
				: to_color(session.grid_data.ruler_color)
		);

		line.draw(_screen, camera);

		y+=session.grid_data.size;
		++index;
	}
}

void editor::draw_layers(ldv::screen& _screen) {

	struct draw_layer_visitor:tile_editor::const_layer_visitor {

		ldv::screen * screen{nullptr};
		editor * controller{nullptr};

		void visit(const tile_editor::tile_layer& _layer) {
			controller->draw_layer(*screen, _layer);
		}

		void visit(const tile_editor::thing_layer& _layer) {
			controller->draw_layer(*screen, _layer);
		}

		void visit(const tile_editor::poly_layer& _layer) {
			controller->draw_layer(*screen, _layer);
		}
	} visitor;

	visitor.screen=&_screen;
	visitor.controller=this;

	switch(layer_draw_mode) {
	
		case layer_draw_modes::all: {
			for(std::size_t index=0; index < map.layers.size(); index++) {
				map.layers[index]->accept(visitor);
			}
		}
		break;

		case layer_draw_modes::stack: {

			for(std::size_t index=current_layer; index < map.layers.size(); index++) {
				map.layers[index]->accept(visitor);
			}
		}
		break;

		case layer_draw_modes::current: 
			map.layers[current_layer]->accept(visitor);
		break;
	}
}

void editor::draw_layer(
	ldv::screen& _screen, 
	const tile_editor::tile_layer& _layer
) {

	const auto& tileset=session.tilesets.at(_layer.set);
	const auto& table=tileset.table;

	ldv::bitmap_representation bmp(
		*tileset_textures.at(tileset.image_path)
	);

	bmp.set_alpha(_layer.alpha);
	bmp.set_blend(ldv::representation::blends::alpha);

	for(const auto& tile : _layer.data) {

		//Calculate the world position...
		int x=tile.x * session.grid_data.size,
		    y=tile.y * session.grid_data.size;

		unsigned int size=session.grid_data.size;
		bmp.set_location({x, y, size, size});

		//TODO: Check errors with "exists"?
		//TODO: If it fails -> DRAW DEFAULT.
		const auto& rect=table.get(tile.type).get_rect();
		bmp.set_clip(rect);

		bmp.draw(_screen, camera);
	}
}

void editor::draw_layer(
	ldv::screen& _screen,
	const tile_editor::thing_layer& _layer
) {

	ldv::box_representation box(
		{0,0,0,0},
		ldv::rgba8(0,0,0,0)
	);

	box.set_alpha(_layer.alpha);
	box.set_blend(ldv::representation::blends::alpha);

	for(const auto& thing : _layer.data) {

		unsigned int w=thing.w,
		             h=thing.h;

		box.set_location({thing.x, thing.y, w, h});
		box.set_color(ldv::rgba8(thing.color.r, thing.color.g, thing.color.b, thing.color.a)); 
		box.draw(_screen, camera);
	}
}

void editor::draw_layer(
	ldv::screen& _screen, 
	const tile_editor::poly_layer& _layer
) {

	ldv::polygon_representation shape(
		{{0,0}},
		ldv::rgba8(0,0,0,0)
	);

	shape.set_alpha(_layer.alpha);
	shape.set_blend(ldv::representation::blends::alpha);

	for(const auto& poly : _layer.data) {

//TODO:...
//		box.set_location({thing.x, thing.y, thing.w, thing.h});
//		shape.set_points();
//		shape.set_color(ldv::rgba8(thing.color.r, thing.color.g, thing.color.b, thing.color.a)); 
//	shape.draw(_screen, camera);
	}
}

void editor::draw_hud(ldv::screen& _screen) {

	std::stringstream ss;

	if(!map.layers.size()) {

		ss<<"no layers";
	}
	else {

		struct draw_layer_hud_visitor:tile_editor::const_layer_visitor {

			tile_editor::map_blueprint * session{nullptr};
			std::string contents;

			void visit(const tile_editor::tile_layer& _layer) {
				contents=std::string{" type: tile, set: "}+session->tilesets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}

			void visit(const tile_editor::thing_layer& _layer) {
				contents=std::string{" type: thing, set: "}+session->thingsets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}

			void visit(const tile_editor::poly_layer& _layer) {
				contents=std::string{" type: poly, set: "}+session->polysets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}
		} visitor;

		visitor.session=&session;
		map.layers[current_layer]->accept(visitor);

		ss<<"layer "
			<<" '"<<map.layers[current_layer]->id<<"'"
			<<visitor.contents
			<<" "<<(current_layer+1)<<" / "<<map.layers.size();
	}

	ss<<" "<<mouse_pos.x<<","<<mouse_pos.y<<" zoom:"<<camera.get_zoom();

	ldv::ttf_representation txt_hud{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 192),
		""
	};

	txt_hud.go_to({0,0});
	txt_hud.set_text(ss.str());
	txt_hud.draw(_screen);
}

ldt::point_2d<int> editor::get_mouse_position(dfw::input& _input) const {

	auto pos=_input().get_mouse_position();

	pos.x/=camera.get_zoom();
	pos.y/=camera.get_zoom();

	pos.x+=camera.get_x();
	pos.y+=camera.get_y();

	return {pos.x, pos.y};
}

void editor::zoom_in() {

	auto zoom=camera.get_zoom();

	if(zoom > 0.1) {

		camera.set_zoom(zoom/2.);
	}
}

void editor::zoom_out() {

	auto zoom=camera.get_zoom();

	if(zoom < 4) {

		camera.set_zoom(zoom*2.);
	}
}

void editor::receive_message(tools::message_manager::notify_event_type /*_type*/) {

	if(message_manager.size()) {

		last_message_rep.set_text(message_manager.last());

		last_message_rep.align(
			screen_rect,
			ldv::representation_alignment{
				ldv::representation_alignment::h::center,
				ldv::representation_alignment::v::inner_bottom
			}
		);
	}
	else {
		last_message_rep.set_text("");
	}
}

void editor::save_current() {

	tile_editor::map_saver ms{log};

	if(!ms.save(map, current_filename)) {
		
		message_manager.add("could not save map!");
	}
	else {

		message_manager.add("map saved");
	}
}

void editor::load_map(const std::string& _path) {

	tile_editor::map_loader ml{log, message_manager, session.thingsets};
	map=ml.load_from_file(_path);
	current_filename=_path;
}

void editor::load_session(const std::string& _path) {

	lm::log(log, lm::lvl::info)<<"map editor will load session data from "<<_path<<std::endl;

	tile_editor::blueprint_parser cfp;
	session=cfp.parse_file(_path);

	//TODO: I would enjoy if this was another component.
	lm::log(log, lm::lvl::info)<<"map editor will load textures..."<<std::endl;
	tileset_textures.clear();
	for(const auto& set : session.tilesets) {

		if(!tileset_textures.count(set.second.image_path)) {

			//TODO: Control failures and shit.
			ldv::image img(set.second.image_path);
			tileset_textures.emplace(
				set.second.image_path,
				std::unique_ptr<ldv::texture>(new ldv::texture(img))
			);

			lm::log(log, lm::lvl::info)<<"loaded "<<set.second.image_path<<std::endl;
		}
	}
}

void editor::previous_layer() {

	if(!map.layers.size() || 0==current_layer) {

		return;
	}

	--current_layer;
}

void editor::next_layer() {

	const auto size=map.layers.size();
	if(!size || current_layer+1==size) {

		return;
	}

	++current_layer;
}

void editor::toggle_layer_draw_mode() {

	switch(layer_draw_mode) {
	
		case layer_draw_modes::all:
			layer_draw_mode=layer_draw_modes::stack;
			message_manager.add("layer draw mode: stack");
			return;

		case layer_draw_modes::stack:
			layer_draw_mode=layer_draw_modes::current;
			message_manager.add("layer draw mode: current"); 
			return;

		case layer_draw_modes::current: 
			layer_draw_mode=layer_draw_modes::all; 
			message_manager.add("layer draw mode: all");
			return;
	}
}
