#ifndef EVOLUTION_TRAY_TRAY_H
#define EVOLUTION_TRAY_TRAY_H

#define ICON_READ "mail-read"
#define ICON_UNREAD "mail-unread"

// Order of items is meaningful
typedef enum {
	ACTION_HIDE,
	
	ACTION_SHOW,
	ACTION_SHOW_AND_SWITCH,
	ACTION_DEICONIFY,
	ACTION_PRESENT,
	
	ACTION_AUTO,
	ACTION_QUERY,
} action_enum_t;

action_enum_t tray_action(action_enum_t requested_action);
void quit_evolution(void);

#endif
