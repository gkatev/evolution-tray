shared_library('liborg-gnome-evolution-tray',
	[
		'tray.c',
		'tray.h',
		'sn.c',
		'sn.h',
		'ucount.c',
		'ucount.h',
		'properties.c',
		'properties.h',
	],
	
	name_prefix: '',
	
	dependencies: [
		evolutionshell,
		evolutionmail,
		libemailengine,
		gtk,
		glib,
		dbusmenuglib,
	],
	
	install: true,
	install_dir: plugindir,
	install_mode: 'rwxr-xr-x',
)
