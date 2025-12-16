/* Evoution Tray plugin, fork of Evolution On
 *  Copyright (C) 2025 George Katevenis <george_kate@hotmail.com>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gio/gio.h>
#include <glib/gprintf.h>

#include <libdbusmenu-glib/server.h>

#include "sn.h"

static const gchar introspection_xml[] =
"<node>"
"  <interface name='" SNI_INTERFACE "'>"
"	<method name='Activate'>"
"	  <arg type='i' name='x' direction='in'/>"
"	  <arg type='i' name='y' direction='in'/>"
"	</method>"
"	<property name='Category' type='s' access='read'/>"
"	<property name='Id' type='s' access='read'/>"
"	<property name='Title' type='s' access='read'/>"
"	<property name='Status' type='s' access='read'/>"
"	<property name='IconName' type='s' access='read'/>"
"	<property name='ItemIsMenu' type='b' access='read'/>"
"	<property name='Menu' type='o' access='read'/>"
"  </interface>"
"</node>";

static GDBusConnection *bus = NULL;
static guint owner_id = 0;
static guint registration_id = 0;
static guint subscription_id = 0;
DbusmenuServer *menu_server = NULL;

static const gchar *current_icon = NULL;

static void register_with_watcher(void);

// -----------------------------

static void on_method_call(GDBusConnection *conn, const gchar *sender,
	const gchar *object_path, const gchar *iface, const gchar *method_name,
	GVariant *params, GDBusMethodInvocation *inv, gpointer user_data)
{
	if(g_strcmp0(method_name, "Activate") == 0) {
		void (*activate_cb)(void) = (void (*)(void)) user_data;
		activate_cb();
		
		g_dbus_method_invocation_return_value(inv, NULL);
	}
}

static GVariant *on_get_property(GDBusConnection *conn, const gchar *sender,
	const gchar *object_path, const gchar *interface_name, const gchar *property_name,
	GError **error, gpointer user_data)
{
	if(g_strcmp0(property_name, "Category") == 0)
		return g_variant_new_string("ApplicationStatus");
	if(g_strcmp0(property_name, "Id") == 0)
		return g_variant_new_string("Evolution Tray");
	if(g_strcmp0(property_name, "Title") == 0)
		return g_variant_new_string("Evolution Tray");
	if(g_strcmp0(property_name, "Status") == 0)
		return g_variant_new_string("Active");
	if(g_strcmp0(property_name, "IconName") == 0)
		return g_variant_new_string(current_icon);
	if(g_strcmp0(property_name, "ItemIsMenu") == 0)
		return g_variant_new_boolean(FALSE);
	if(g_strcmp0(property_name, "Menu") == 0)
		return g_variant_new_object_path("/Menu");
	
	return NULL;
}

static void on_snw_owner_changed(GDBusConnection *conn, const gchar *sender,
	const gchar *path, const gchar *interface, const gchar *signal_name,
	GVariant *params, gpointer user_data)
{
	const gchar *name, *old_owner, *new_owner;
	g_variant_get(params, "(&s&s&s)", &name, &old_owner, &new_owner);
	
	// If there is an owner, register
	if(new_owner && *new_owner)
		register_with_watcher();
}

static void register_with_watcher(void) {
	GError *error = NULL;
	
	GDBusProxy *watcher_proxy = g_dbus_proxy_new_sync(bus, G_DBUS_PROXY_FLAGS_NONE,
		NULL, "org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher",
		"org.kde.StatusNotifierWatcher", NULL, &error);
	
	if(!watcher_proxy) {
		g_printerr("Evolution Tray: dbus: Failed to create proxy to "
			"StatusNotifierWatcher: %s\n", error->message);
		g_clear_error(&error);
		return;
	}
	
	g_dbus_proxy_call_sync(watcher_proxy, "RegisterStatusNotifierItem",
		g_variant_new("(s)", DBUS_SERVICE_NAME), G_DBUS_CALL_FLAGS_NONE,
		-1, NULL, &error);
	
	if(error) {
		g_printerr("Evolution Tray: dbus: Failed to register with "
			"StatusNotifierWatcher: %s\n", error->message);
	}
	
	g_object_unref(watcher_proxy);
	g_clear_error(&error);
}

// -----------------------------

static void on_menu_properties(DbusmenuMenuitem *mi,
	guint timestamp, void (*menu_prefs_cb)(void))
{
	menu_prefs_cb();
}

static void on_menu_quit(DbusmenuMenuitem *mi,
	guint timestamp, void (*menu_quit_cb)(void))
{
	menu_quit_cb();
}

static DbusmenuMenuitem *build_menu(
	void (*menu_prefs_cb)(void),
	void (*menu_quit_cb)(void))
{
	DbusmenuMenuitem *root, *item;
	
	root = dbusmenu_menuitem_new();
	
	/* Properties */
	item = dbusmenu_menuitem_new();
	
	dbusmenu_menuitem_property_set(item,
		DBUSMENU_MENUITEM_PROP_LABEL, "_Properties");
	dbusmenu_menuitem_property_set(item,
		DBUSMENU_MENUITEM_PROP_ICON_NAME, "document-properties");
	
	g_signal_connect(item, DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED,
		G_CALLBACK(on_menu_properties), menu_prefs_cb);
	
	dbusmenu_menuitem_child_append(root, item);
	g_object_unref(item);
	
	/* Quit */
	item = dbusmenu_menuitem_new();
	
	dbusmenu_menuitem_property_set(item,
		DBUSMENU_MENUITEM_PROP_LABEL, "_Quit");
	dbusmenu_menuitem_property_set(item,
		DBUSMENU_MENUITEM_PROP_ICON_NAME, "application-exit");
	
	g_signal_connect(item, DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED,
		G_CALLBACK(on_menu_quit), menu_quit_cb);
	
	dbusmenu_menuitem_child_append(root, item);
	g_object_unref(item);
	
	return root;
}

gint sn_init(const char *icon_name,
	void (*activate_cb)(void),
	void (*menu_prefs_cb)(void),
	void (*menu_quit_cb)(void))
{
	GDBusProxy *bus_proxy = NULL;
	GDBusNodeInfo *introspection_data = NULL;
	GVariant *bus_reply = NULL;
	GError *error = NULL;
	
	bus = NULL;
	
	gint return_code = -1;
	
	current_icon = icon_name;
	
	// ---
	
	bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
	if(!bus) {
		g_printerr("Evolution Tray: dbus: Failed to connect to D-Bus: %s\n",
			error->message);
		goto end;
	}
	
	owner_id = g_bus_own_name_on_connection(bus, DBUS_SERVICE_NAME,
		G_BUS_NAME_OWNER_FLAGS_NONE, NULL, NULL, NULL, NULL);
	
	/* Export SNI interface */
	
	introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, &error);
	
	if(!introspection_data) {
		g_printerr("Evolution Tray: dbus: "
			"Failed to parse introspection xml data: %s\n", error->message);
		goto end;
	}
	
	static const GDBusInterfaceVTable interface_vtable = {
		.method_call = on_method_call,
		.get_property = on_get_property
	};
	
	registration_id = g_dbus_connection_register_object(bus,
		SNI_OBJECT_PATH, introspection_data->interfaces[0],
		&interface_vtable, activate_cb, NULL, &error);
	
	if(registration_id == 0) {
		g_printerr("Evolution Tray: dbus: "
			"Failed to register object: %s\n", error->message);
		goto end;
	}
	
	/* Setup DBusMenu */
	
	menu_server = dbusmenu_server_new("/Menu");
	DbusmenuMenuitem *root = build_menu(menu_prefs_cb, menu_quit_cb);
	dbusmenu_server_set_root(menu_server, root);
	g_object_unref(root);
	// ---
	
	/* Call-me-back if/when the owner of
	 * org.kde.StatusNotifierWatcher changes */
	
	subscription_id = g_dbus_connection_signal_subscribe(bus,
		"org.freedesktop.DBus", "org.freedesktop.DBus", "NameOwnerChanged",
		"/org/freedesktop/DBus", "org.kde.StatusNotifierWatcher",
		G_DBUS_SIGNAL_FLAGS_NONE, on_snw_owner_changed, NULL, NULL);
	
	/* Create a proxy and check if a watcher already exists. If not, do
	 * nothing -- we'll call register in the NameOwnerChanged callback. */
	
	bus_proxy = g_dbus_proxy_new_sync(bus, G_DBUS_PROXY_FLAGS_NONE,
		NULL, "org.freedesktop.DBus", "/org/freedesktop/DBus",
		"org.freedesktop.DBus", NULL, &error);
	
	if(!bus_proxy) {
		g_printerr("Evolution Tray: dbus: "
			"Failed to create DBus proxy: %s\n", error->message);
		goto end;
	}
	
	bus_reply = g_dbus_proxy_call_sync(bus_proxy, "NameHasOwner",
		g_variant_new("(s)", "org.kde.StatusNotifierWatcher"),
		G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
	
	if(!bus_reply) {
		g_printerr("Evolution Tray: dbus: "
			"NameHasOwner call failed: %s\n", error->message);
		goto end;
	}
	
	gboolean watcher_available;
	g_variant_get(bus_reply, "(b)", &watcher_available);
	if(watcher_available)
		register_with_watcher();
	
	return_code = 0;
	
end:
	
	g_clear_object(&bus_proxy);
	g_clear_pointer(&introspection_data, g_dbus_node_info_unref);
	g_clear_pointer(&bus_reply, g_variant_unref);
	g_clear_error(&error);
	
	if(return_code != 0)
		sn_fini();
	
	return return_code;
}

void sn_fini(void) {
	if(subscription_id > 0) {
		g_dbus_connection_signal_unsubscribe(bus, subscription_id);
		subscription_id = 0;
	}
	
	g_clear_object(&menu_server);
	
	if(registration_id > 0) {
		g_dbus_connection_unregister_object(bus, registration_id);
		registration_id = 0;
	}
	
	g_clear_handle_id(&owner_id, g_bus_unown_name);
	g_clear_object(&bus);
}

void sn_set_icon(const gchar *icon_name) {
	current_icon = icon_name;
	
	g_dbus_connection_emit_signal(bus, NULL, SNI_OBJECT_PATH,
		SNI_INTERFACE, "NewIcon", NULL, NULL);
}

const gchar *sn_get_icon(void) {
	return current_icon;
}
