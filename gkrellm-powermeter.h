// As seen in
// http://gkrellm.srcbox.net/plugins2-reference.html

#ifndef CMAKE_LIB_TEST_LIBRARY_H
#define CMAKE_LIB_TEST_LIBRARY_H

#include <gkrellm2/gkrellm.h>

#define STYLE_NAME "powermeter"
#define POWERMETER_CONFIG_KEYWORD "powermeter"

static GkrellmMonitor plugin_mon;
static GkrellmMonitor *monitor;

//Chart
static GkrellmChart *chart;
static GkrellmChartdata *chartdata;
static GkrellmChartconfig *chartconfig;
static gboolean error_reading = FALSE;

//Panels
static GkrellmPanel *panel_watt;
static GkrellmDecal *pw_decal1;
static GkrellmPanel *panel_programms;
static GkrellmDecal *programdecal;
static char programtext[512];

//Style & stuff
static GkrellmTicks *pGK;
static gint	plugin_enable;
static gint	style_id;

// Config Stuff
static gint num_progs = 5;
static char powertarget_name[128];
static gboolean show_powertarget_name = TRUE;
static char powerlog_path[CFG_BUFSIZE];

// GUI STUFF
static GtkWidget *vbox_plugin;
static GtkWidget *text_format_combobox;
static GtkWidget *prognumber_spinbutton;
static GtkWidget *powerlog_path_textbox;




static void update_plugin(void);
static void draw_plugin_chart();
static void create_plugin(GtkWidget *vbox, gint first_create);
static void create_plugin_tab(GtkWidget *tab_vbox);
static void apply_plugin_config();
static void save_config(FILE *f);
static void load_config(gchar *arg);

GkrellmMonitor * gkrellm_init_plugin(void);

//Events
static gint chart_expose_event(GtkWidget *widget, GdkEventExpose *ev);
static gint panel_watt_expose_event(GtkWidget *widget, GdkEventExpose *ev);
static gint panel_programms_expose_event(GtkWidget *widget, GdkEventExpose *ev);
static gint chart_config_event(GtkWidget *widget, GdkEventButton *ev);
static gint panel_config_event(GtkWidget *widget, GdkEventButton *ev);


#endif //CMAKE_LIB_TEST_LIBRARY_H
