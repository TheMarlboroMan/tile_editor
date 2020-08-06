#include "editor_types/property_manager.h"

using namespace tile_editor;

bool property_manager::has_property(const std::string& _key) const {

	return int_properties.count(_key)
		|| string_properties.count(_key)
		|| double_properties.count(_key);
}

std::size_t property_manager::size() const {

	return int_properties.size()
		+ string_properties.size()
		+ double_properties.size();
}
