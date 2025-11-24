#ifndef EVOLUTION_TRAY_SN_H
#define EVOLUTION_TRAY_SN_H

#define DBUS_SERVICE_NAME "org.gnome.evolution.plugin.evolution-tray"
#define SNI_INTERFACE "org.kde.StatusNotifierItem"
#define SNI_OBJECT_PATH "/StatusNotifierItem"

int sn_init(const char *icon_name);

void sn_fini(void);
void sn_set_icon(const gchar *icon_name);
const gchar *sn_get_icon(void);

#endif /* EVOLUTION_TRAY_SN_H */
