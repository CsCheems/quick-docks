
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

static std::unordered_set<std::string> g_registeredHotkeys;
static std::vector<std::unique_ptr<DockHotkey>> g_dockHotkeys;
static QPointer<QTimer> g_rescanTimer;

static QString GetDockKey(QDockWidget *dock)
{
	if (!dock)
		return {};

	QString key = dock->objectName();

	if (key.isEmpty()) {
		key = dock->windowTitle().simplified();
		key.replace(' ', '_');
	}

	key.replace(' ', '_');
	return key;
}

static void LoadHotkeyBinding(DockHotkey &entry)
{
	config_t *config = obs_frontend_get_profile_config();
	if (!config)
		return;

	const char *json =
		config_get_string(config, "Hotkeys", entry.id.c_str());

	if (!json || !*json)
		return;

	obs_data_t *data = obs_data_create_from_json(json);
	if (!data)
		return;

	obs_data_array_t *bindings =
		obs_data_get_array(data, "bindings");

	if (bindings) {
		obs_hotkey_load(entry.hotkey, bindings);

		obs_log(LOG_INFO,
			"[DockSwitcher] Loaded binding for '%s' count=%zu",
			entry.id.c_str(),
			obs_data_array_count(bindings));

		obs_data_array_release(bindings);
	}

	obs_data_release(data);
}

static void SaveHotkeyBindings()
{
	config_t *config = obs_frontend_get_profile_config();
	if (!config)
		return;

	for (auto &entryPtr : g_dockHotkeys) {
		auto &entry = *entryPtr;

		if (entry.hotkey == OBS_INVALID_HOTKEY_ID)
			continue;

		obs_data_array_t *bindings =
			obs_hotkey_save(entry.hotkey);

		obs_data_t *data = obs_data_create();

		if (bindings) {
			obs_data_set_array(data, "bindings", bindings);
		}

		const char *json = obs_data_get_json(data);

		config_set_string(
			config,
			"Hotkeys",
			entry.id.c_str(),
			json ? json : "{}");

		obs_log(LOG_INFO,
			"[DockSwitcher] Saved binding for '%s' count=%zu",
			entry.id.c_str(),
			bindings ? obs_data_array_count(bindings) : 0);

		if (bindings)
			obs_data_array_release(bindings);

		obs_data_release(data);
	}
}

static void FocusDock(QDockWidget *dock)
{
	if (!dock)
		return;

	if (!dock->isVisible())
		dock->show();

	dock->raise();

	if (auto *w = dock->focusWidget())
		w->setFocus(Qt::OtherFocusReason);
	else
		dock->setFocus(Qt::OtherFocusReason);

	obs_log(LOG_INFO,
		"[DockSwitcher] Focused dock: %s",
		dock->windowTitle().toUtf8().constData());
}

static void DockHotkeyPressed(
	void *data,
	obs_hotkey_id,
	obs_hotkey_t *,
	bool pressed)
{
	if (!pressed)
		return;

	auto *hk = static_cast<DockHotkey *>(data);

	if (!hk)
		return;

	if (hk->dock) {
		FocusDock(hk->dock);
	}
}

static void RegisterDiscoveredDocks()
{
	auto *main =
		reinterpret_cast<QMainWindow *>(
			obs_frontend_get_main_window());

	if (!main)
		return;

	auto docks = main->findChildren<QDockWidget *>();

	for (auto *dock : docks) {
		if (!dock)
			continue;

		const QString title = dock->windowTitle();

		if (title.isEmpty())
			continue;

		const QString key = GetDockKey(dock);

		if (key.isEmpty())
			continue;

		const std::string hotkeyId =
			("focus_dock_" + key).toStdString();

		if (g_registeredHotkeys.find(hotkeyId) !=
		    g_registeredHotkeys.end())
		{
			continue;
		}

		auto entry =
			std::make_unique<DockHotkey>();

		entry->id = hotkeyId;
		entry->key = key;
		entry->dock = dock;

		const QByteArray desc =
			("Focus Dock: " + title).toUtf8();

		entry->hotkey =
			obs_hotkey_register_frontend(
				hotkeyId.c_str(),
				desc.constData(),
				DockHotkeyPressed,
				entry.get());

		if (entry->hotkey == OBS_INVALID_HOTKEY_ID) {
			obs_log(LOG_WARNING,
				"[DockSwitcher] Failed to register hotkey '%s'",
				hotkeyId.c_str());
			continue;
		}

		LoadHotkeyBinding(*entry);

		g_registeredHotkeys.insert(hotkeyId);
		g_dockHotkeys.push_back(std::move(entry));

		obs_log(LOG_INFO,
			"[DockSwitcher] Registered dock='%s' key='%s'",
			title.toUtf8().constData(),
			key.toUtf8().constData());
	}
}

static void ResolveDocks()
{
	obs_log(LOG_INFO, "[DockSwitcher] ResolveDocks()");

	auto *main =
		reinterpret_cast<QMainWindow *>(
			obs_frontend_get_main_window());

	if (!main) {
		obs_log(LOG_WARNING,
			"[DockSwitcher] Main window is null");
		return;
	}

	auto docks = main->findChildren<QDockWidget *>();

	obs_log(LOG_INFO,
		"[DockSwitcher] Found %d docks",
		docks.size());

	for (auto *dock : docks) {
		if (!dock)
			continue;

		const QString title = dock->windowTitle();

		if (title.isEmpty())
			continue;

		const QString key = GetDockKey(dock);

		for (auto &entryPtr : g_dockHotkeys) {
			auto &entry = *entryPtr;

			if (entry.key == key) {
				entry.dock = dock;

				obs_log(LOG_INFO,
					"[DockSwitcher] Linked hotkey to dock: %s",
					title.toUtf8().constData());

				break;
			}
		}
	}
}

static void DumpHotkeyState()
{
	for (auto &entryPtr : g_dockHotkeys) {
		auto &entry = *entryPtr;

		if (entry.hotkey == OBS_INVALID_HOTKEY_ID)
			continue;

		obs_data_array_t *arr =
			obs_hotkey_save(entry.hotkey);

		obs_log(LOG_INFO,
			"[DockSwitcher] key='%s' bindings=%zu",
			entry.key.toUtf8().constData(),
			arr ? obs_data_array_count(arr) : 0);

		if (arr)
			obs_data_array_release(arr);
	}
}

static void OnFrontendEvent(
	enum obs_frontend_event event,
	void *)
{
	obs_log(LOG_INFO,
		"[DockSwitcher] Frontend event: %d",
		(int)event);

	if (event != OBS_FRONTEND_EVENT_FINISHED_LOADING)
		return;

	obs_log(LOG_INFO,
		"[DockSwitcher] OBS finished loading");

	RegisterDiscoveredDocks();
	ResolveDocks();
	DumpHotkeyState();

	if (!g_rescanTimer) {
		g_rescanTimer = new QTimer(qApp);

		QObject::connect(
			g_rescanTimer,
			&QTimer::timeout,
			[]()
			{
				RegisterDiscoveredDocks();
				ResolveDocks();
			});

		g_rescanTimer->start(3000);
	}
}

bool obs_module_load(void)
{
	obs_log(LOG_INFO,
		"plugin loaded successfully (version %s)",
		PLUGIN_VERSION);

	obs_log(LOG_INFO,
		"[DockSwitcher] obs_module_load()");

	obs_log(LOG_INFO,
		"[DockSwitcher] Main window at load = %p",
		obs_frontend_get_main_window());

	obs_frontend_add_event_callback(
		OnFrontendEvent,
		nullptr);

	obs_frontend_add_save_callback(
		[](obs_data_t *, bool saving, void *)
		{
			obs_log(LOG_INFO,
				"[DockSwitcher] Save callback saving=%d",
				saving);

			if (saving)
				SaveHotkeyBindings();
		},
		nullptr);

	return true;
}

void obs_module_unload(void)
{
	if (g_rescanTimer) {
		g_rescanTimer->stop();
		g_rescanTimer->deleteLater();
		g_rescanTimer = nullptr;
	}

	obs_frontend_remove_event_callback(
		OnFrontendEvent,
		nullptr);

	obs_log(LOG_INFO,
		"plugin unloaded");
}