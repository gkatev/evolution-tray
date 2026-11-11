project('evolution-tray', 'c',
	version: '3.0.1',
	meson_version: '>=0.58.0',
	license: 'GPL',
	default_options: ['prefix=/usr']
)

config_inc = include_directories('.')

# Dependencies
evolutionshell = dependency('evolution-shell-3.0', version: '>=3.38.3')
evolutionmail  = dependency('evolution-mail-3.0',  version: '>=3.38.3')
libemailengine = dependency('libemail-engine',     version: '>=3.38.3')
gtk            = dependency('gtk+-3.0',            version: '>=3.24')
glib           = dependency('glib-2.0',            version: '>=2.56')
dbusmenuglib   = dependency('dbusmenu-glib-0.4')

plugindir = evolutionshell.get_variable(pkgconfig: 'privlibdir') / 'plugins'

# GSettings
if get_option('install-schemas')
	install_data('src/org.gnome.evolution.plugin.evolution-tray.gschema.xml',
		install_dir: get_option('datadir') / 'glib-2.0' / 'schemas' )
	# We're not adding anything to compile any schemas. That's up to the packager.
endif

# For the .eplug file (plain text)
eplug_data = configuration_data()

eplug_data.set('PLUGIN_INSTALL_DIR', plugindir)
eplug_data.set('GETTEXT_PACKAGE', meson.project_name())
eplug_data.set('LOCALEDIR', get_option('prefix') / get_option('localedir'))

# For config.h (quoted)
conf_data = configuration_data()

# We dont want deprecated functions from evolution data server
conf_data.set('EDS_DISABLE_DEPRECATED', true)

conf_data.set10('EVOLUTION_HAS_EUIMANAGER',
	evolutionshell.version().version_compare('>=3.55.1'))

conf_data.set10('EVOLUTION_HAS_HIDE_ON_CLOSE',
	evolutionshell.version().version_compare('>=3.58.1'))

conf_data.set_quoted('GETTEXT_PACKAGE', meson.project_name())

configure_file(
	input: 'src/org-gnome-evolution-tray.eplug.in',
	output: 'org-gnome-evolution-tray.eplug',
	configuration: eplug_data,
	install: true,
	install_dir: plugindir,
)

configure_file(
	output: 'config.h',
	configuration: conf_data,
)

subdir('src')
subdir('po')

summary({
	'Evolution version': evolutionshell.version(),
	'Plugin directory': plugindir,
}, section: 'Build')

summary({
	'Install schemas': get_option('install-schemas'),
}, section: 'Options', bool_yn: true)
