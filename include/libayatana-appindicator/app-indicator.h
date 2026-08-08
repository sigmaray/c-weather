/* Minimal AppIndicator API declarations for linking against
 * libayatana-appindicator3 without the -dev package. */
#ifndef APP_INDICATOR_H
#define APP_INDICATOR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
    APP_INDICATOR_CATEGORY_APPLICATION_STATUS,
    APP_INDICATOR_CATEGORY_COMMUNICATIONS,
    APP_INDICATOR_CATEGORY_SYSTEM_SERVICES,
    APP_INDICATOR_CATEGORY_HARDWARE,
    APP_INDICATOR_CATEGORY_OTHER
} AppIndicatorCategory;

typedef enum {
    APP_INDICATOR_STATUS_PASSIVE,
    APP_INDICATOR_STATUS_ACTIVE,
    APP_INDICATOR_STATUS_ATTENTION
} AppIndicatorStatus;

typedef struct _AppIndicator AppIndicator;

GType app_indicator_get_type(void);

AppIndicator *app_indicator_new(const gchar *id,
                                const gchar *icon_name,
                                AppIndicatorCategory category);

AppIndicator *app_indicator_new_with_path(const gchar *id,
                                          const gchar *icon_name,
                                          AppIndicatorCategory category,
                                          const gchar *icon_theme_path);

void app_indicator_set_status(AppIndicator *self, AppIndicatorStatus status);
void app_indicator_set_menu(AppIndicator *self, GtkMenu *menu);
void app_indicator_set_icon(AppIndicator *self, const gchar *icon_name);
void app_indicator_set_icon_full(AppIndicator *self, const gchar *icon_name,
                                 const gchar *icon_desc);
void app_indicator_set_icon_theme_path(AppIndicator *self,
                                       const gchar *icon_theme_path);
void app_indicator_set_label(AppIndicator *self, const gchar *label,
                             const gchar *guide);
void app_indicator_set_title(AppIndicator *self, const gchar *title);

G_END_DECLS

#endif
