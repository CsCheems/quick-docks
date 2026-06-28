
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

using namespace std;

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

struct DockHotkey {
	obs_hotkey_id hotkey = OBS_INVALID_HOTKEY_ID;
	std::string id;
	QString key;
	QPointer<QDockWidget> dock;
};

static vector<unique_ptr<DockHotkey>> dockHotKey;
static QPointer<QTimer> timer;


// static void obs_test_list_docks()
// {
// 	auto *main = reinterpret_cast<QMainWindow *>(obs_frontend_get_main_window());

// 	if(!main)
// 	{
// 		return;
// 	}

// 	auto docks = main->findChildren<QDockWidget *>();

// 	obs_log(LOG_INFO, "Found %d docks", docks.size());

// }



//Helpers

static QString normalize_dock_name(QDockWidget *dock)
{
	QString dock_name = dock->objectName();

	if(dock_name.isEmpty())
	{
		dock_name = dock->windowTitle().simplified();
	}
	
	dock_name.replace(' ', '_');

	return dock_name;
}

static DockHotkey *find_key_comb(QString &key)
{
	for(auto &e : dockHotKey)
	{
		if(e->key == key)
		{
			return e.get();
		}
	}
	return nullptr;
}

//Get available docks

static void obs_get_all_docks()
{
	auto *main = reinterpret_cast<QMainWindow *>(obs_frontend_get_main_window());

	if(!main)
	{
		return;
	}

	auto docks = main->findChildren<QDockWidget *>();

	for(auto *dock : docks)
	{
		QString title = dock->windowTitle();

		if(title.isEmpty()){
			continue;
		}

		QString key = normalize_dock_name(dock);

		DockHotkey *dock_hk_exists = find_key_comb(key);
		if(dock_hk_exists)
		{
			dock_hk_exists->dock = dock;
			continue;
		}

	}
}

static void get_event_obs_finish_loading(enum obs_frontend_event event, void *){
	if(event != OBS_FRONTEND_EVENT_FINISHED_LOADING)
	{
		obs_log(LOG_INFO, "OBS has not done loading");
		return;
	}

	obs_log(LOG_INFO, "OBS Done Loading");

	obs_get_all_docks();
	
	return;	
}

static void obs_load_event(void)
{
	obs_frontend_add_event_callback(get_event_obs_finish_loading, nullptr);


}



bool obs_module_load(void)
{
	obs_log(LOG_INFO, "Quick Docks plugin loaded successfully (version %s)", PLUGIN_VERSION);

	obs_log(LOG_INFO, "[DockSwitcher] Main window at load = %p", obs_frontend_get_main_window());

	obs_frontend_add_event_callback(get_event_obs_finish_loading, nullptr);

	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(get_event_obs_finish_loading, nullptr);
	
	obs_log(LOG_INFO, "Quick Docks plugin unloaded" );

	
}