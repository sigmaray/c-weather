/* Stub AppIndicator for non-Linux builds (macOS / Windows CI).
 * Provides linkable no-op implementations of the minimal API. */

#include <libayatana-appindicator/app-indicator.h>

#include <stdlib.h>
#include <string.h>

struct _AppIndicator {
    gchar *id;
    gchar *icon_name;
    gchar *icon_theme_path;
    gchar *label;
    gchar *title;
    AppIndicatorCategory category;
    AppIndicatorStatus status;
    GtkMenu *menu;
};

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
    if (self) {
        self->status = status;
    }
}

void app_indicator_set_menu(AppIndicator *self, GtkMenu *menu) {
    if (self) {
        self->menu = menu;
    }
}

void app_indicator_set_icon(AppIndicator *self, const gchar *icon_name) {
    if (!self) {
        return;
    }
    g_free(self->icon_name);
    self->icon_name = g_strdup(icon_name);
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
}

void app_indicator_set_label(AppIndicator *self, const gchar *label,
                             const gchar *guide) {
    (void)guide;
    if (!self) {
        return;
    }
    g_free(self->label);
    self->label = g_strdup(label);
}

void app_indicator_set_title(AppIndicator *self, const gchar *title) {
    if (!self) {
        return;
    }
    g_free(self->title);
    self->title = g_strdup(title);
}
