#define RIVET_REGISTER_MOD(entryFn)                                      \
	extern "C" __declspec(dllexport) Rivet::ModDef GET_RIVET_MOD_DEF() { \
		return Rivet::ModDef{+[]() { entryFn(); }};                      \
	}
