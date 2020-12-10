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
#include "tile_editor/app/set_layer_loader.h"

#include <lm/sentry.h>
#include <ldv/line_representation.h>
#include <ldv/bitmap_representation.h>
#include <ldv/box_representation.h>

#include <algorithm>

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
	mouse_pos{0,0},
	tile_list{_screen_w, _screen_h, grid_list_w, grid_list_h},
	thing_list{_screen_h, vertical_list_h},
	poly_list{_screen_h, vertical_list_h} {

	tile_list.set_margin_w(grid_list_margin);
	tile_list.set_margin_h(grid_list_margin);
	thing_list.set_margin_h(vertical_list_margin);
	poly_list.set_margin_h(8);

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
		return;
	}

	if(_input.is_input_down(input::layer_settings)) {

		open_layer_settings();
		return;
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

		if(_input.is_input_pressed(input::lctrl)) {

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

	if(_input.is_input_down(input::zoom_in)) {

		zoom_in();
		return;
	}
	else if(_input.is_input_down(input::zoom_out)) {

		zoom_out();
		return;
	}

	if(_input.is_input_down(input::space)) {

		toggle_layer_draw_mode();
		return;
	}

	if(_input.is_input_down(input::tab)) {

		show_set=!show_set;
	}

	auto mpos=_input().get_mouse_position();
	mouse_pos={mpos.x, mpos.y};

	if(_input.is_input_down(input::pageup)) {

		previous_layer();
		return;
	}
	else if(_input.is_input_down(input::pagedown)) {

		next_layer();
		return;
	}

	if(_input.is_input_down(input::left_click)) {

		int click_modifiers=click_modifier_none;

		if(_input.is_input_pressed(input::del)) {

			click_modifiers|=click_modifier_delete;
		}

		if(_input.is_input_pressed(input::lshift)) {

			click_modifiers|=click_modifier_lshift;
		}

		if(_input.is_input_pressed(input::lctrl)) {

			click_modifiers|=click_modifier_lctrl;
		}

		click_input(input::left_click, click_modifiers);
		return;
	}

	if(show_set) {

		int movement_x=0,
			movement_y=0;

		if(_input.is_input_down(input::up)) {
			movement_y=-1;
		}
		else if(_input.is_input_down(input::down)) {
			movement_y=1;
		}
		else if(_input.is_input_down(input::left)) {
			movement_x=-1;
		}
		else if(_input.is_input_down(input::right)) {
			movement_x=1;
		}

		if(movement_x || movement_y) {

			 arrow_input_set(movement_x, movement_y);
		}

		return;
	}
	else {

		typedef  bool (dfw::input::*input_fn)(int) const;
		input_fn movement_fn=_input.is_input_pressed(input::lctrl)
			? &dfw::input::is_input_down
			: &dfw::input::is_input_pressed;

		const int factor=_input.is_input_pressed(input::lctrl)
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

			 arrow_input_map(movement_x, movement_y);
		}

		return;
	}
}

void editor::click_input(
	int _input,
	int _modifiers
) {
	struct : tile_editor::layer_visitor {
		editor * controller{nullptr};
		int input{0};
		int modifiers{0};
		void visit(tile_editor::tile_layer& _layer) {controller->click_input(input, modifiers, _layer);}
		void visit(tile_editor::thing_layer&) {}
		void visit(tile_editor::poly_layer&) {}
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.input=_input;
	dispatcher.modifiers=_modifiers;
	map.layers.at(current_layer)->accept(dispatcher);
}

void editor::click_input(
	int _input,
	int _modifiers,
	tile_editor::tile_layer& _layer
) {
	auto world_pos=get_world_position(mouse_pos);
	auto grid=get_grid_position(world_pos);

	//Process multiclick with lshift. Multiclick acts by delimiting a box.
	if(! (_modifiers & click_modifier_lshift)) {

		multiclick.engaged=false;
	}
	else {

		if(multiclick.point!=grid) {

			multiclick.engaged=true;
			multiclick.rangex={std::min(grid.x, multiclick.point.x), std::max(grid.x, multiclick.point.x)};
			multiclick.rangey={std::min(grid.y, multiclick.point.y), std::max(grid.y, multiclick.point.y)};
		}
	}

	multiclick.point=grid;

	auto find_tile_at=[&_layer](editor_point _pt) {

		return std::find_if(
			std::begin(_layer.data),
			std::end(_layer.data),
			[_pt](const tile_editor::tile& _tile) {
				return _tile.x==_pt.x && _tile.y==_pt.y;
			}
		);
	};

	//Delete...
	//TODO: the key combination is not practical.
	if(_modifiers & click_modifier_delete) {

		auto delete_tile_if_exists=[&_layer, find_tile_at](editor_point _pt) {

			auto it=find_tile_at(_pt);
			if(it!=std::end(_layer.data)) {

				_layer.data.erase(it);
			}
		};

		if(!multiclick.engaged) {
			delete_tile_if_exists(grid);
			return;
		}

		for(int x=multiclick.rangex.min; x<=multiclick.rangex.max; x++) {
			for(int y=multiclick.rangey.min; y<=multiclick.rangey.max; y++) {
				delete_tile_if_exists({x, y});
			}
		}
		return;
	}

	//Insert or update...
	auto set_tile=[&_layer, find_tile_at](editor_point _pt, int _type) {

		auto it=find_tile_at(_pt);
		if(it!=std::end(_layer.data)) {
			it->type=_type;
		}
		//did not find anything...
		else {
			_layer.data.push_back({_pt.x, _pt.y, _type});
		}
	};

	if(!multiclick.engaged) {
		set_tile(grid, tile_list.get().first);
		return;
	}

	for(int x=multiclick.rangex.min; x<=multiclick.rangex.max; x++) {
		for(int y=multiclick.rangey.min; y<=multiclick.rangey.max; y++) {
			set_tile({x, y}, tile_list.get().first);
		}
	}

	return;
}

void editor::arrow_input_set(
	int _movement_x,
	int _movement_y
) {

	struct : tile_editor::const_layer_visitor {
		editor * controller{nullptr};
		int      movement_x{0},
		         movement_y{0};

		void visit(const tile_editor::tile_layer&) {

			if(movement_y < 0) {
				controller->tile_list.previous_row();
				return;
			}
			else if(movement_y > 0) {
				controller->tile_list.next_row();
				return;
			}

			if(movement_x < 0) {
				controller->tile_list.previous();
			}
			else if(movement_x > 0) {
				controller->tile_list.next();
			}
		}
		void visit(const tile_editor::thing_layer&) {

			if(movement_y < 0) {
				controller->thing_list.previous();
			}
			else if(movement_y > 0) {
				controller->thing_list.next();
			}
		}
		void visit(const tile_editor::poly_layer&) {

			if(movement_y < 0) {
				controller->poly_list.previous();
			}
			else if(movement_y > 0) {
				controller->poly_list.next();
			}
		}
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.movement_x=_movement_x;
	dispatcher.movement_y=_movement_y;

	map.layers.at(current_layer)->accept(dispatcher);
}

void editor::arrow_input_map(
	int _movement_x,
	int _movement_y
) {

	camera.move_by(_movement_x, _movement_y);
}

void editor::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(session.bg_color.r, session.bg_color.g, session.bg_color.b, session.bg_color.a));

	draw_grid(_screen);
	draw_layers(_screen);
	draw_hud(_screen);
	if(show_set) {
		draw_set(_screen);
	}
	draw_messages(_screen);
}

void editor::draw_messages(ldv::screen& _screen) {

	last_message_rep.draw(_screen);
}

void editor::draw_set(
	ldv::screen& _screen
) {

	if(!map.layers.size()) {

		return;
	}

	struct : tile_editor::const_layer_visitor {

		editor *         controller{nullptr};
		ldv::screen *    screen{nullptr};
		void visit(const tile_editor::tile_layer& _layer) {controller->draw_set(*screen, _layer);}
		void visit(const tile_editor::thing_layer& _layer) {controller->draw_set(*screen, _layer);}
		void visit(const tile_editor::poly_layer& _layer) {controller->draw_set(*screen, _layer);}

	} dispatcher;

	dispatcher.controller=this;
	dispatcher.screen=&_screen;
	map.layers.at(current_layer)->accept(dispatcher);
}

int editor::draw_set_background(
	ldv::screen& _screen
) {
	int w=_screen.get_w() * (session.toolbox_width_percent / 100.);

	ldv::box_representation box(
		{0,0, w, _screen.get_h()},
		ldv::rgba8(0,0,0,128)
	);

	box.set_blend(ldv::representation::blends::alpha);
	box.align(
		screen_rect,
		{
			ldv::representation_alignment::h::inner_right,
			ldv::representation_alignment::v::inner_top
		}
	);

	box.draw(_screen);

	return box.get_position().x;
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::tile_layer& _layer
) {

	//draw the tiles...
	ldv::bitmap_representation bmp(
		*tileset_textures.at(session.tilesets.at(_layer.set).image_path)
	);

	const int background_start{draw_set_background(_screen)};
	const unsigned int item_w{tile_list.get_item_w()},
	                   item_h{tile_list.get_item_h()};

	for(const auto& item : tile_list.get_page()) {

		if(item.index==tile_list.get_current_index()) {

			ldv::box_representation current_box(
				{background_start+item.x-1,item.y-1, item_w+2 , item_h+2},
				ldv::rgba8(0,0,255,128)
			);

			current_box.draw(_screen);
		}

		bmp.set_clip(item.item.second.get_rect());
		bmp.set_location({background_start+item.x, item.y, item_w, item_h});
		bmp.draw(_screen);
	}
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::thing_layer& _layer
) {
	const int background_start{draw_set_background(_screen)};
	const unsigned int item_h{thing_list.get_item_h()};

	for(const auto& item : thing_list.get_page()) {

		const auto prototype=session.thingsets.at(_layer.set).table.at(item.item.type_id);

		draw_set_text(
			_screen,
			background_start,
			item.y,
			item_h,
			prototype.color,
			prototype.name,
			item.index==thing_list.get_current_index()
		);
	}
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::poly_layer& _layer
) {

	const int background_start{draw_set_background(_screen)};
	const unsigned int item_h{poly_list.get_item_h()};

	for(const auto& item : poly_list.get_page()) {

		const auto prototype=session.polysets.at(_layer.set).table.at(item.item.poly_id);

		draw_set_text(
			_screen,
			background_start,
			item.y,
			item_h,
			prototype.color,
			prototype.name,
			item.index==poly_list.get_current_index()
		);
	}
}

void editor::draw_set_text(
	ldv::screen& _screen,
	int _x,
	int _y,
	int _h,
	tile_editor::color _color,
	const std::string& _name,
	bool _is_current
) {

	//Color...
	ldv::box_representation color_box(
		{_x, _y, _h , _h},
		ldv::rgba8(_color.r, _color.g, _color.b, _color.a)
	);

	color_box.draw(_screen);

	//Name...
	ldv::ttf_representation txt_name{
		ttf_manager.get("main", 14),
		_is_current
			? ldv::rgba8(0, 0, 255, 255)
			: ldv::rgba8(255, 255, 255, 255),
		_name
	};

	txt_name.go_to({_x+_h+2, _y});
	txt_name.draw(_screen);
}

//TODO: Take me to some toolset I can reuse.
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
	int ruler_units=session.grid_data.horizontal_ruler * session.grid_data.size;

	while(x < x_max) {

		auto gridcolor=0==x
			? to_color(session.grid_data.origin_color)
			: (
				(x % ruler_units)
					? to_color(session.grid_data.color)
					: to_color(session.grid_data.ruler_color)
			);

		ldv::line_representation line(
			{x, focus.origin.y},
			{x, y_max},
			gridcolor
		);

		line.draw(_screen, camera);

		x+=session.grid_data.size;
	}

	//Horizontal lines...
	module=euclidean_module(focus.origin.y, session.grid_data.size);
	int y=focus.origin.y-module;
	ruler_units=session.grid_data.vertical_ruler * session.grid_data.size;
	while(y < y_max) {

		auto gridcolor=0==y
			? to_color(session.grid_data.origin_color)
			: (
				(y % ruler_units)
					? to_color(session.grid_data.color)
					: to_color(session.grid_data.ruler_color)
			);

		ldv::line_representation line(
			{focus.origin.x, y},
			{x_max, y},
			gridcolor
		);

		line.draw(_screen, camera);

		y+=session.grid_data.size;
	}
}

void editor::draw_layers(ldv::screen& _screen) {

	struct :tile_editor::const_layer_visitor {

		ldv::screen * screen{nullptr};
		editor * controller{nullptr};
		void visit(const tile_editor::tile_layer& _layer) {controller->draw_layer(*screen, _layer);}
		void visit(const tile_editor::thing_layer& _layer) {controller->draw_layer(*screen, _layer);}
		void visit(const tile_editor::poly_layer& _layer) {controller->draw_layer(*screen, _layer);}
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

		std::vector<ldv::point> points(poly.points.size());
		std::transform(
			std::begin(poly.points),
			std::end(poly.points),
			std::begin(points),
			[](const ldv::point& _point) -> tile_editor::poly_point {

				return {_point.x, -_point.y};
			}
		);

		shape.set_points(points);
		shape.set_color(ldv::rgba8(poly.color.r, poly.color.g, poly.color.b, poly.color.a));
		shape.draw(_screen, camera);
	}
}

void editor::draw_hud(ldv::screen& _screen) {

	std::stringstream ss;

	if(!map.layers.size()) {

		ss<<"no layers";
	}
	else {

		struct :tile_editor::const_layer_visitor {

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

	tile_editor::map_loader ml{
		log,
		message_manager,
		session.thingsets,
		session.polysets
	};
	map=ml.load_from_file(_path);
	current_filename=_path;
	current_layer=0;
	load_layer_toolset();
}

void editor::load_session(const std::string& _path) {

	lm::log(log, lm::lvl::info)<<"map editor will load session data from "<<_path<<std::endl;

	tile_editor::blueprint_parser cfp;
	session=cfp.parse_file(_path);

	//set the toolbox width...
	int w=screen_rect.w * (session.toolbox_width_percent / 100.);
	tile_list.set_available_w(w);

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
	load_layer_toolset();
	layer_change_cleanup();
}

void editor::next_layer() {

	const auto size=map.layers.size();
	if(!size || current_layer+1==size) {

		return;
	}

	++current_layer;
	load_layer_toolset();
	layer_change_cleanup();
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

void editor::load_layer_toolset() {

	if(!map.layers.size()) {

		return;
	}

	tile_editor::set_layer_loader ll{
		tile_list,
		thing_list,
		poly_list,
		session.tilesets,
		session.thingsets,
		session.polysets
	};

	map.layers.at(current_layer)->accept(ll);
}

ldt::point_2d<int> editor::get_world_position(ldt::point_2d<int> _pos) const {

	_pos.x/=camera.get_zoom();
	_pos.y/=camera.get_zoom();

	_pos.x+=camera.get_x();
	_pos.y+=camera.get_y();

	return {_pos.x, _pos.y};
}

ldt::point_2d<int> editor::get_grid_position(ldt::point_2d<int> _point) const {

	double size{session.grid_data.size};

	return {
		(floor(_point.x / size)) ,
		(floor(_point.y / size))
	};
}

void editor::layer_change_cleanup() {

	selected_poly=nullptr;
	selected_thing=nullptr;
	multiclick.engaged=false;
}

void editor::open_layer_settings() {

	if(!map.layers.size()) {

		return;
	}

	struct :public tile_editor::layer_visitor {
		editor * controller{nullptr};
		void visit(tile_editor::tile_layer& _layer) {

			controller->exchange_data.layer=&_layer;
			controller->exchange_data.blueprint=&controller->session;
			start(state_tile_editor_properties);
		}
		void visit(tile_editor::thing_layer&) {}
		void visit(tile_editor::poly_layer&) {}

		private:
		void start(int _state) {
			controller->exchange_data.put(_state);
			controller->push_state(_state);
		}
	} dispatcher;
	dispatcher.controller=this;
	map.layers.at(current_layer)->accept(dispatcher);
}
