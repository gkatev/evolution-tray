# Evolution Tray

Evolution Tray is an Evolution Email plugin to minimize the application to the
system tray.

This project is a fork of Evolution-on. The main differentiation point is the
implementation of the tray icon using `StatusNotifierItem` instead of the
deprecated and now poorly working `GtkStatusIcon`.

Unlike Evolution On, Evolution Tray only handles tray-related functionality.
Other features, like mail notifications, are offered by the upstream
`mail-notification` plugin.

### Notes & Limitations

- Hide-on-close: Requires Evolution >= 3.58.1.
  - [evolution/#200](https://gitlab.gnome.org/GNOME/evolution/-/merge_requests/200)

- Hide-on-minimize: Doesn't work on Wayland.
  - No proper support for detecting minimization.

### Building/Installing

#### AUR

&rarr; [evolution-tray-git](https://aur.archlinux.org/packages/evolution-tray-git)

#### From Source

The build system is based on meson. To configure and build the project you
could use something like:

```bash
$ meson setup build
$ meson compile -C build
$ meson install -C build
```

Optional setup options:
- `-Dinstall-schemas=false`: Don't install GSettings schema
- `-Ddebugbuild=true`: Debug build

FYI: The first time you install the plugin, you might then also need to compile
the GSettings schemas system-wide -- something that the build script does not
handle. The approach may vary depending on the platform.

On Arch Linux: \
`# glib-compile-schemas /usr/share/glib-2.0/schemas`
