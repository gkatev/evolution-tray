shared_library('org-gnome-evolution-tray',
	['tray.c', 'sn.c', 'ucount.c', 'properties.c'],
	
	dependencies: [evolutionshell, evolutionmail,
		libemailengine, gtk, glib, dbusmenuglib],
	
	include_directories: config_inc,
	
	install: true,
	install_dir: plugindir,
)
