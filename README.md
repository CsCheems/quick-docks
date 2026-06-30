# Español:

## Asignar atajos de teclado a los paneles acoplables (docks) de OBS

Un plugin que permite asignar atajos de teclado a cualquier panel, para que puedas enfocarlos al instante sin necesidad de volver tocar la ventana de OBS.

## ¿Qué hace?

OBS cuenta con varios paneles como:
   * Escenas
   * Fuentes
   * Mezcaldor de Audio
   * Chat
   * Estadisticas
   * Actividad de Twitch
   * Y muchas mas...

Pero no existe una forma nativa de enfocarlos mediante un atajo de teclado... hasta ahora.

Les presento el Quick-Docks, un plugin que detecta los paneles disponibles en la ventana de tu OBS (tambien se incluyen los que metas por otros plugins) y los lista en la ventana de configuracion para atajos.

Esto permite que el usuario puedo ver la lista de sus paneles y asignar un atajo a cada uno.

## Características

* Detecta automáticamente todos los paneles acoplables, incluidos los añadidos por plugins de terceros.
* Registra un atajo de teclado **"Focus Dock: ..."** (Enfocar panel: ...) para cada panel encontrado.
* Las asignaciones de atajos se guardan por perfil y se restauran al iniciar el programa.
* Vuelve a buscar nuevos paneles cada 10 segundos, por lo que los paneles de plugins que cargan más tarde se detectan automáticamente.
* Si cerraste un panel, basta con que ingreses el atajo y volvera a aparecer, haciendo que tenga el foco al instante.

## Instalación

1. Descarga la última versión para tu plataforma desde la página de [Releases](../../releases)
2. Extrae y copia los archivos en la carpeta de plugins de OBS:
3. Para el .dll copialo en la siguiente direccion:
   - **Windows**: `C:\Program Files\obs-studio\obs-plugins\64bit\`.
4. Para la carpeta quick-docks compiala en la siguiente direccion:
    - **Windows**: `C:\Program Files\obs-studio\data\obs-plugins\`.
4. Inicia OBS Studio .

## Uso

1. Abre OBS y ve a **Archivo → Configuracion → Atajos**.
2. Desplázate hacia abajo y busca las entradas etiquetadas como **"Focus Dock: ..."**.
3. Asigna una combinación de teclas a los paneles que desees.
4. Haz clic en Aplicar y Ok.
5. Listo! Mi recomendacion es usar un streamdeck para facilitar el uso de atajos.


# English:

## Assign keyboard shortcuts to OBS dockable panels (docks)

A plugin that allows you to assign keyboard shortcuts to any panel, so you can instantly focus on them without needing to interact with the main OBS window again.

## What does it do?

OBS features several panels, such as:
   * Scenes
   * Sources
   * Audio Mixer
   * Chat
   * Statistics
   * Twitch Activity
   * And many more...

However, there is no native way to focus on them using a keyboard shortcut... until now.

Introducing Quick-Docks, a plugin that detects the panels available in your OBS window (including those added by other plugins) and lists them in the hotkey configuration window.

This allows the user to view their list of panels and assign a shortcut to each one.

## Features

* Automatically detects all dockable panels, including those added by third-party plugins.
* Registers a **"Focus Dock: ..."** keyboard shortcut for each detected panel.
* Shortcut assignments are saved per profile and restored when the program starts.
* Rescans for new panels every 10 seconds, ensuring that panels from plugins that load later are automatically detected.
* If you have closed a panel, simply using the shortcut will make it reappear and instantly take focus.

## Installation

1. Download the latest version for your platform from the [Releases](../../releases) page.
2. Extract and copy the files into the OBS plugins folder:
3. Copy the .dll file to the following location:
   - **Windows**: `C:\Program Files\obs-studio\obs-plugins\64bit\`.
4. Copy the `quick-docks` folder to the following location:
    - **Windows**: `C:\Program Files\obs-studio\data\obs-plugins\`.
5. Launch OBS Studio.
  
## Usage

1. Open OBS and go to **File → Settings → Hotkeys**.
2. Scroll down and look for the entries labeled **"Focus Dock: ..."**.
3. Assign a key combination to the panels you want.
4. Click Apply and OK.
5. You're all set! I recommend using a Stream Deck to make using the hotkeys easier.
