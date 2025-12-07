#define RIVET_REGISTER_MOD(klass, author, name) \
	extern "C" __declspec(dllexport) Rivet::ModDef GET_RIVET_MOD_DEF() { \
		return Rivet::ModDef { \
			+[]() -> Rivet::IMod* { return new klass(); }, \
			+[]() -> const char* { return name; }, \
			+[]() -> const char* { return author; } \
		}; \
	}