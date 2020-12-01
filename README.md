# Tile editor

## TODO

- Setup "load" controller.
- Setup "save" controller. This is going to hurt... quite a lot.
- Setup "grid", draw stuff...

## Building

TODO:

## How to use

TODO:
./editor.out -c #configfile# [-f #mapfile#]

## Input files

### Session file

Contains:

- Templates for map properties
- Tilesets
- Object sets
- Polygon sets
- Session data

Can be reused along many map files thast share the same building blocks.

### Map file

Contains

- Map metadata
- Map attributes (from map property templates!)
- Tiles
- Things (in the grand Doom tradition, also known as "objects").
- Polygons

## What are...

### Templates for map properties

### Tilesets

### Object sets

### Polygon sets

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
