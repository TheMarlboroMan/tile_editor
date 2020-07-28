#include "blueprint_types/property_table.h"

using namespace tile_editor;

bool property_table::property_exists(const std::string& _key) const {

	return int_properties.count(_key)
		|| string_properties.count(_key)
		|| double_properties.count(_key);
}

std::size_t property_table::size() const {

	return int_properties.size() + string_properties.size() + double_properties.size();
}
