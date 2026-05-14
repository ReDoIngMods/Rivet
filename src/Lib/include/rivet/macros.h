#define RIVET_REGISTER_MOD(entryFn, author, name)                        \
	extern "C" __declspec(dllexport) Rivet::ModDef GET_RIVET_MOD_DEF() { \
		return Rivet::ModDef{                                            \
		    +[]() { entryFn(); },                                        \
		    +[]() -> const char* { return name; },                       \
		    +[]() -> const char* { return author; }};                    \
	}
