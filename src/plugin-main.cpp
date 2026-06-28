
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

//Load the keybinds from profle

static void load_keybinds(DockHotkey &entry)
{
	config_t *profile = obs_frontend_get_profile_config();

	if(!profile)
	{
		return;
	}

	const char *json = config_get_string(profile, "Hotkeys", entry.id.c_str());

	if(!json)
	{
		return;
	}

	obs_data_t *data = obs_data_create_from_json(json);

	if(!data)
	{
		return;
	}

	obs_data_array_t *keybinds = obs_data_get_array(data, "keybindings");

	if(keybinds)
	{
		obs_hotkey_load(entry.hotkey, keybinds);
		obs_data_array_release(keybinds);
	}

	obs_data_release(data);

}

//Save the keybinds

static void save_bindings()
{
	config_t *profile = obs_frontend_get_profile_config();

	if(!profile)
	{
		return;
	}

	for( auto &entry : dockHotKey)
	{
		if(entry->hotkey == OBS_INVALID_HOTKEY_ID){
			continue;
		}

		obs_data_array_t *keybinds = obs_hotkey_save(entry->hotkey);

		obs_data_t *data = obs_data_create();

		if(keybinds)
		{
			obs_data_set_array(data, "keybindings", keybinds);
			obs_data_array_release(keybinds);
		}

		const char *json = obs_data_get_json(data);

		config_set_string(profile, "Hotkeys", entry->id.c_str(), json ? json : "{}");

		obs_data_release(data);
	}
}

//Show or Focus Dock

static void focus_dock(QDockWidget *dock)
{
	if(!dock->isVisible())
	{
		dock->show();
	}
	dock->raise();
}

//Listener for hotkeys 

static void hotkey_pressed(void *data, obs_hotkey_id, obs_hotkey_t *, bool keyPressed)
{
	if(!keyPressed)
	{
		return;
	}

	auto *entry = static_cast<DockHotkey *>(data);

	if(entry->dock){
		focus_dock(entry->dock);
	}
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

		auto entry = make_unique<DockHotkey>();
		entry->id = ("focus_dock_" + key).toStdString();
		entry->key = key;
		entry->dock = dock;

		entry->hotkey = obs_hotkey_register_frontend(
			entry->id.c_str(),
			("Focus Dock: " + title).toUtf8().constData(),
			hotkey_pressed,
			entry.get()
		);

		load_keybinds(*entry);

		dockHotKey.push_back(move(entry));
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

	if(!timer)
	{
		timer = new QTimer(qApp);
		QObject::connect(timer, &QTimer::timeout, obs_get_all_docks);
		timer->start(3000);
	}
	
}

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "Quick Docks plugin loaded successfully (version %s)", PLUGIN_VERSION);

	obs_log(LOG_INFO, "[DockSwitcher] Main window at load = %p", obs_frontend_get_main_window());

	obs_frontend_add_event_callback(get_event_obs_finish_loading, nullptr);

	obs_frontend_add_save_callback(
		[](obs_data_t *, bool save, void *){
			if(save)
			{
				save_bindings();
			}
		},
		nullptr
	);

	return true;
}

void obs_module_unload(void)
{
	if(timer)
	{
		timer->stop();
		timer->deleteLater();
		timer = nullptr;
	}

	obs_frontend_remove_event_callback(get_event_obs_finish_loading, nullptr);
	
	obs_log(LOG_INFO, "Quick Docks plugin unloaded" );
}