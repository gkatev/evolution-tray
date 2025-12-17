/* Evoution Tray plugin, fork of Evolution On
 * Copyright (C) 2008-2012 Lucian Langa <cooly@gnome.eu.org>
 * Copyright (C) 2012-2013 Kostadin Atanasov <pranayama111@gmail.com>
 * Copyright (C) 2022 Ozan Türkyılmaz <ozan.turkyilmaz@gmail.com>
 * Copyright (C) 2025 George Katevenis <george_kate@hotmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>

#include <e-util/e-util.h>

#include "properties.h"

/******************************************************************************
 * Query dconf
 *****************************************************************************/
gboolean
is_part_enabled(gchar *schema, const gchar *key)
{
	GSettings *settings = g_settings_new(schema);
	gboolean res = g_settings_get_boolean(settings, key);
	g_object_unref(settings);
	return res;
}

static void
set_part_enabled(gchar *schema, const gchar *key, gboolean enable)
{
	GSettings *settings = g_settings_new (schema);
	g_settings_set_boolean (settings, key, enable);
	g_object_unref (settings);
}

/******************************************************************************
 * Callback for configuration widget
 *****************************************************************************/
static void
toggled_hidden_on_startup_cb(GtkWidget *widget, gpointer data)
{
	g_return_if_fail(widget != NULL);
	set_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDDEN_ON_STARTUP,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

static void
toggled_hidde_on_minimize_cb(GtkWidget *widget, gpointer data)
{
	g_return_if_fail(widget != NULL);
	set_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDE_ON_MINIMIZE,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

static void
toggle_hidden_on_close_cb(GtkWidget *widget, gpointer data)
{
	g_return_if_fail(widget != NULL);
	set_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDE_ON_CLOSE,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

/******************************************************************************
 * Properties widget
 *****************************************************************************/

static GtkWidget *
get_cfg_widget()
{
	GtkWidget *container, *check;
	
	container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	
	check = gtk_check_button_new_with_mnemonic(_("Hidden on startup"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
		is_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDDEN_ON_STARTUP));
	g_signal_connect(G_OBJECT(check), "toggled",
		G_CALLBACK(toggled_hidden_on_startup_cb), NULL);
	gtk_box_pack_start(GTK_BOX(container), check, FALSE, FALSE, 0);
	
	check = gtk_check_button_new_with_mnemonic(_("Hide on minimize"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (check),
		is_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDE_ON_MINIMIZE));
	g_signal_connect(G_OBJECT (check), "toggled",
		G_CALLBACK(toggled_hidde_on_minimize_cb), NULL);
	gtk_box_pack_start(GTK_BOX(container), check, FALSE, FALSE, 0);
	
	if(GDK_IS_WAYLAND_DISPLAY(gdk_display_get_default())) {
		GtkWidget *hbox, *icon, *note;
		
		hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
		
		icon = gtk_image_new_from_icon_name("dialog-warning", GTK_ICON_SIZE_MENU);
		gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
		
		note = gtk_label_new(_("Will not work properly on Wayland"));
		gtk_label_set_xalign(GTK_LABEL(note), 0.0);
		gtk_box_pack_start(GTK_BOX(hbox), note, FALSE, FALSE, 0);
		
		gtk_widget_set_margin_start(hbox, 24);
		gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE, 0);
	}
	
	check = gtk_check_button_new_with_mnemonic(_("Hide on close"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
		is_part_enabled(TRAY_SCHEMA, CONF_KEY_HIDE_ON_CLOSE));
	g_signal_connect(G_OBJECT(check), "toggled",
		G_CALLBACK(toggle_hidden_on_close_cb), NULL);
	gtk_box_pack_start(GTK_BOX(container), check, FALSE, FALSE, 0);
	
	gtk_widget_show_all(container);
	
	return container;
}

GtkWidget *
e_plugin_lib_get_configure_widget(EPlugin *epl)
{
	return get_cfg_widget();
}

void properties_show(void) {
	GtkWidget *cfg, *dialog, *label, *vbox, *hbox;
	GtkWidget *content_area;
	gchar *text;

	cfg = get_cfg_widget();
	if (!cfg)
		return;

	text = g_markup_printf_escaped("<span size=\"x-large\">%s</span>",
			_("Evolution Tray"));

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	label = gtk_label_new(NULL);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_label_set_yalign(GTK_LABEL(label), 0.5);
	gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
	gtk_label_set_markup(GTK_LABEL(label), text);
	g_free (text);

	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
	gtk_widget_show(vbox);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	label = gtk_label_new("   ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show_all(hbox);

	gtk_box_pack_start(GTK_BOX (vbox), cfg, TRUE, TRUE, 0);

	dialog = gtk_dialog_new_with_buttons(_("Evolution On Properties"),
			NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			_("_Close"), GTK_RESPONSE_CLOSE, NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	gtk_container_add(GTK_CONTAINER(content_area), vbox);
	gtk_container_set_border_width(GTK_CONTAINER (vbox), 10);
	gtk_widget_set_size_request(dialog, 350, -1);
	g_signal_connect_swapped(dialog, "response",
			G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show(dialog);
}
