# Tile editor

## TODO

- Think about the HUD for a bit.
	- The map contains absolutely NO information on what order the layers
	are stacked on, which creates interesting display conundrums.
		- We could change the format for that
			- Makes sense, actually: all layers are just stacked in a "layers" 	node... We can mostly keep the structure (add type in layer) and the code (just read the type first) but the data structure makes no allowances for this. We would have to make the layers be a vector of pointers to layers so the data structure makes sense.
		- We could add metadata for that...
			- Makes sense too: something like "display_order": ["T1", "T2", "P1", "T1"] or just in the meta part: "display_order", 0, maybe "display": true so it can be toggled. It would be a part of the map data structure and should be updated when the data changes. Drawing would be a matter of reading a vector which references the order.
			- IDEA: I like this one... seems easier to implement in the end than to change what a layer is.

	- The HUD should have:
		- info on the current layer
		- info on the number of layer we are in
		- the set selector
	- Each layer is separate, there's no poly+tile+thing layer, but rather three separate layers.
	- adding a new layer should either prompt for the type or maybe we could just have shortcuts to add a thing, poly or tile layers.
	- layers should be able to be rearranged for display.
	- TODO: A problem, the order of the layers for display might just fuck up different maps between the same application (one goes Thing, Tile, Poly, Tile and the other goes Tile, Tile, Poly, Thing... perhaps layers could have an alias on their meta?, this would do the whole ordering thing easier by having a meta order with just their aliases, it would also make easy to add a new layer with a controller: just add the alias, type, set and shit.

- Setup "grid", draw stuff...

## Building

TODO:

## How to use

./editor.out -c #configfile# [-f #mapfile#]

## Input files
### configfile

Contains:

- Templates for map properties
- Tilesets
- Object sets
- Polygon sets
- Session data

Can be reused along many map files thast share the same building blocks.

### mapfile

Contains

- Map metadata
- Map attributes (from map property templates!)
- Tiles
- Things (in the grand Doom tradition, also known as "objects").
- Polygons

## What are...
### Templates for map properties

TODO

### Tilesets

TODO

### Object sets

TODO

### Polygon sets

TODO

## Output file format

TODO

## History

- As of November 5th, 2015, the tool has been patched with a compatibility layer to support the many changes made to the original libdansdl2. Should the tool be updated, the compatibility layer will be removed. The tool now needs the SDL2_ttf library to link with (even though it's not used).
- As of July 2017 the dependencies have been added and the project has been patched to work with the bundled dependencies.-
- 19-7-2017: Added keyboard tile selection shortcuts.
- 21-7-2017: Many changes...
	- Added grid and file properties dialog.
	- Added also a little message in the bottom, to know what you are doing.
	- Fixed bug in tile selection shortcut.
	- Added background to tile selection list, to make it more usable.
	- Removed old bracket input to change tiles.
- 22-7-2017: Many more changes.
	- Added dnot export. Still not 100% functional.
	- Changed the way how properties work in objects, using a simple key->value sequence instead of the numerical index thing.
- 23-7-2017: Changes.
	- Added dnot export-import.
	- Fixed bug in dnot_parser, of course, only in the frozen branch here.
- 3-8-2017: Changes.
	- Added alpha to each layer so shadow layers can be done.
	- Added alpha and separator space to layer properties controller.
	- Added the possibility to change cell size, which previously was impossible without fiddling with the map files.
	- Also added this value to importers and exporters.
	- Changed a bit the structure of each layer to further presentational changes (such as background colours) can be changed easily.
	- Hacked the life away to support png alpha.
- 7-8-2017: Changes.
	- Added the possibility to change the size of logic objects... This actually has consequences in many places (loaders, exporters, logic editor...).

## Credits

Bebas Neue font by Dharma Type.
Example tileset 1 by Paulina Riva
Example tileset 2 by mfburn
