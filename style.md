#style-naming guide for myself...

About time I made one...

When writing tools:

- emulate the stl and have lowercase everywhere, absolutely everywhere.
- underscores are there for a reason, do not camelcase stuff
- enum classes go in plural
- use smart tabs
- name stuff for what it does, not the type it is.
- pragma once
- prefix parameter names with an underscore
- indent like you mean it, except for namespaces
- in case of name collision, thing again:
	- are your enums plural?
	- are you naming an instance the same as its class? why?
