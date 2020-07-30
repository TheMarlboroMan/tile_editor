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
	- are your enums plural?, instead of:

		enum class      side{front, left, right, back};
		side            side_type;

	try this:

		enum class      sides{front, left, right, back};
		sides           side;

	- are you naming an instance the same as its class? That's actually tricky,
	think of your readers... A classname should represent what it is, ok, so:

	class fruit_juicer {...}

	is clearly something that makes juice of fruits. Any local name, however, 
	should not be named after what it is, right?. Would you rather see

	std::string string{"Mike Hannigan"};

	or

	std::string name{"Mike Hannigan"};
	
	? Then that's it. If you need to instance a fruit_juicer, think:

	- Does it make belong only in the current scope?. You can get away with 
	a name like juicer, fruitjuicer, fjuicer, juicer_instance...

	- Does it make belong in a broader scope (say, inside a class)? Ok then...
	is there a single instance? If that's the case, maybe you can get away with
	the generic name, but you would be better off with a name that explains what
	it does, like kitchen_fruit_juicer. If there's more than one instance you
	definitely want a name that clearly expresses intent.



		

