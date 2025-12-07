#include <rivet/modding.h>

#include <Windows.h>

class SampleMod : public Rivet::IMod {
public:
	void onRivetInitialize() override {
		MessageBoxA(nullptr, "SampleMod: onRivetInitialize called!", "SampleMod", MB_OK);
	}

	void onContraptionInitialize() override {
		MessageBoxA(nullptr, "SampleMod: onContraptionInitialize called!", "SampleMod", MB_OK);
	}
};

RIVET_REGISTER_MOD(SampleMod, "Sample Author", "Sample Mod")