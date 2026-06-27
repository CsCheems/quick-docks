
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <plugin-support.h>
#include <util/config-file.h>

#include <QApplication>
#include <QDockWidget>
#include <QMainWindow>
#include <QPointer>
#include <QTimer>

#include <vector>
#include <memory>
#include <unordered_set>
#include <string>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

struct DockHotkey {
	obs_hotkey_id hotkey = OBS_INVALID_HOTKEY_ID;
	std::string id;
	QString key;
	QPointer<QDockWidget> dock;
};

void test_obs_finish_loading(enum obs_frontend_event event, void *){
	if(event != OBS_FRONTEND_EVENT_FINISHED_LOADING)
	{
		obs_log(LOG_INFO, "OBS has not done loading");
		return;
	}

	obs_log(LOG_INFO, "OBS Done Loading");
	return;	
}

bool obs_module_load(void)
{
	obs_frontend_add_event_callback(test_obs_finish_loading, nullptr);

	obs_log(LOG_INFO, "Quick Docks plugin loaded successfully (version %s)", PLUGIN_VERSION);

	obs_frontend_add_event_callback(test_obs_finish_loading, nullptr);

	obs_log(LOG_INFO, "[DockSwitcher] Main window at load = %p", obs_frontend_get_main_window());

	obs_frontend_add_event_callback(test_obs_finish_loading, nullptr);

	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "Quick Docks plugin unloaded" );
}