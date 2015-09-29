#ifndef application_h
#define application_h

#include "settings.h"

class Application {
public:
	static Application* getApplication() {
		static Application app;
		return &app;
	}
	Settings getSettings() { return settings; }
	bool isExecuting() { return executing; }
	void startExecuting() { executing = true; }
	void stopExecuting() { executing = false; }
private:
	Application() {};
	Application(Application const&) = delete;
	void operator=(Application const&) = delete;

	Settings settings;
	bool executing = false;
};
#endif
