/* AppIndicator-compatible tray backend for non-Linux builds (macOS / Windows).
 * Uses GtkStatusIcon (Win32 NotifyIcon / Quartz NSStatusItem under GTK). */

#include <libayatana-appindicator/app-indicator.h>

#include <stdlib.h>
#include <string.h>

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

struct _AppIndicator {
    gchar *id;
    gchar *icon_name;
    gchar *icon_theme_path;
    gchar *label;
    gchar *title;
    AppIndicatorCategory category;
    AppIndicatorStatus status;
    GtkMenu *menu;
    GtkStatusIcon *status_icon;
};

static void refresh_tooltip(AppIndicator *self) {
    const gchar *text = NULL;
    if (self->title && self->title[0]) {
        text = self->title;
    } else if (self->label && self->label[0]) {
        text = self->label;
    } else if (self->id) {
        text = self->id;
    }
    if (self->status_icon) {
        gtk_status_icon_set_tooltip_text(self->status_icon, text);
    }
}

static void refresh_icon(AppIndicator *self) {
    if (!self->status_icon || !self->icon_name) {
        return;
    }

    if (self->icon_theme_path && self->icon_theme_path[0]) {
        gchar *file = g_strdup_printf("%s.png", self->icon_name);
        gchar *path = g_build_filename(self->icon_theme_path, file, NULL);
        g_free(file);
        if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            gtk_status_icon_set_from_file(self->status_icon, path);
            g_free(path);
            return;
        }
        g_free(path);
    }

    gtk_status_icon_set_from_icon_name(self->status_icon, self->icon_name);
}

static void popup_menu(AppIndicator *self, guint button, guint32 activate_time) {
    if (!self->menu) {
        return;
    }
    gtk_widget_show_all(GTK_WIDGET(self->menu));
    gtk_menu_popup(self->menu, NULL, NULL, gtk_status_icon_position_menu,
                   self->status_icon, button, activate_time);
}

static void on_status_icon_popup(GtkStatusIcon *status_icon, guint button,
                                 guint32 activate_time, gpointer user_data) {
    (void)status_icon;
    popup_menu((AppIndicator *)user_data, button, activate_time);
}

static void on_status_icon_activate(GtkStatusIcon *status_icon,
                                    gpointer user_data) {
    (void)status_icon;
    /* Left-click also opens the menu (common on Windows tray icons). */
    popup_menu((AppIndicator *)user_data, 1, gtk_get_current_event_time());
}

static void ensure_status_icon(AppIndicator *self) {
    if (self->status_icon) {
        return;
    }
    self->status_icon = gtk_status_icon_new();
    g_signal_connect(self->status_icon, "popup-menu",
                     G_CALLBACK(on_status_icon_popup), self);
    g_signal_connect(self->status_icon, "activate",
                     G_CALLBACK(on_status_icon_activate), self);
    gtk_status_icon_set_visible(self->status_icon, FALSE);
    refresh_icon(self);
    refresh_tooltip(self);
}

GType app_indicator_get_type(void) {
    return G_TYPE_OBJECT;
}

static AppIndicator *stub_new(const gchar *id, const gchar *icon_name,
                              AppIndicatorCategory category,
                              const gchar *icon_theme_path) {
    AppIndicator *self = g_malloc0(sizeof(*self));
    self->id = g_strdup(id);
    self->icon_name = g_strdup(icon_name);
    self->icon_theme_path = g_strdup(icon_theme_path);
    self->category = category;
    self->status = APP_INDICATOR_STATUS_PASSIVE;
    ensure_status_icon(self);
    return self;
}

AppIndicator *app_indicator_new(const gchar *id, const gchar *icon_name,
                                AppIndicatorCategory category) {
    return stub_new(id, icon_name, category, NULL);
}

AppIndicator *app_indicator_new_with_path(const gchar *id,
                                          const gchar *icon_name,
                                          AppIndicatorCategory category,
                                          const gchar *icon_theme_path) {
    return stub_new(id, icon_name, category, icon_theme_path);
}

void app_indicator_set_status(AppIndicator *self, AppIndicatorStatus status) {
    if (!self) {
        return;
    }
    self->status = status;
    ensure_status_icon(self);
    gtk_status_icon_set_visible(self->status_icon,
                                status != APP_INDICATOR_STATUS_PASSIVE);
}

void app_indicator_set_menu(AppIndicator *self, GtkMenu *menu) {
    if (!self) {
        return;
    }
    self->menu = menu;
}

void app_indicator_set_icon(AppIndicator *self, const gchar *icon_name) {
    if (!self) {
        return;
    }
    g_free(self->icon_name);
    self->icon_name = g_strdup(icon_name);
    ensure_status_icon(self);
    refresh_icon(self);
}

void app_indicator_set_icon_full(AppIndicator *self, const gchar *icon_name,
                                 const gchar *icon_desc) {
    (void)icon_desc;
    app_indicator_set_icon(self, icon_name);
}

void app_indicator_set_icon_theme_path(AppIndicator *self,
                                       const gchar *icon_theme_path) {
    if (!self) {
        return;
    }
    g_free(self->icon_theme_path);
    self->icon_theme_path = g_strdup(icon_theme_path);
    ensure_status_icon(self);
    refresh_icon(self);
}

void app_indicator_set_label(AppIndicator *self, const gchar *label,
                             const gchar *guide) {
    (void)guide;
    if (!self) {
        return;
    }
    g_free(self->label);
    self->label = g_strdup(label);
    refresh_tooltip(self);
}

void app_indicator_set_title(AppIndicator *self, const gchar *title) {
    if (!self) {
        return;
    }
    g_free(self->title);
    self->title = g_strdup(title);
    refresh_tooltip(self);
}

G_GNUC_END_IGNORE_DEPRECATIONS
