#include "gkrellm_powermeter.h"
#include "powermeter_library/powermeter_library.h"
#include "progmanager_library/progmanager_library_c.h"

#include <stdlib.h>
#include <locale.h>

static GkrellmMonitor plugin_mon =
		{
				"gkrellm_powermeter",        /* Name, for config tab.    */
				0,                  /* Id,  0 if a plugin       */
				create_plugin,      /* The create function      */
				update_plugin,      /* The update function      */
				create_plugin_tab,  /* The config tab create function   */
				apply_plugin_config,/* Apply the config function        */

				save_config,        /* Save user config   */
				load_config,        /* Load user config   */
				POWERMETER_CONFIG_KEYWORD, /* config keyword     */

				NULL,               /* Undefined 2  */
				NULL,               /* Undefined 1  */
				NULL,               /* private      */

				MON_UPTIME,         /* Insert plugin before this monitor */

				NULL,               /* Handle if a plugin, filled in by GKrellM     */
				NULL                /* path if a plugin, filled in by GKrellM       */
		};



static void update_plugin(void) {
	if(GK.second_tick) {
		progmanager_library_update();

		float current_power = powermeter_library_current_powerdraw();
		if(current_power == -1) {
			//printf("error reading current powerdraw\n");
			error_reading = TRUE;
		}
		else{
			error_reading = FALSE;
			gkrellm_store_chartdata(chart, 0, (int)(current_power*1000));
		}
	}

	draw_plugin_chart();
}



static void draw_plugin_chart() {
	float current_datapoint = gkrellm_get_current_chartdata(chartdata)/1000.f;
	//Draw the upper panel
	char buffer[64];
	sprintf(buffer, "%5.2fW %5.2fWh", current_datapoint, powermeter_library_total_energy_used());
	gint w, h;
	gkrellm_text_markup_extents(pw_decal1->text_style.font,	buffer, strlen(buffer),
								&w, &h, NULL, &pw_decal1->y_ink);
	w += pw_decal1->text_style.effect;
	gkrellm_decal_text_set_offset(pw_decal1, (pw_decal1->w - w) / 2, 0);
	gkrellm_draw_decal_text(panel_watt, pw_decal1, buffer, 0);
	gkrellm_draw_panel_layers(panel_watt);

	//Draw chart
	//Draw error message, or the powertarget name, if wanted.
	gkrellm_draw_chartdata(chart);
	if(error_reading)
		gkrellm_draw_chart_text(chart, style_id, "powerfile\nnot readable");
	else if(show_powertarget_name){
		//call the function twice, so the grid is drawn correctly...
		gkrellm_draw_chart_text(chart, style_id, "");
		gkrellm_draw_chart_text(chart, style_id, powertarget_name);
	}
	gkrellm_draw_chart_to_screen(chart);

	//Draw lower panel
	double utils[num_progs];
	pid_t pids[num_progs];
	progmanager_highest_util(num_progs, utils, pids);
	char name[50];
	double cpu_user, cpu_system;
	int pos = 0;
	memset(&programtext, 0, 512);
	for (int i = 0; i < num_progs; i++) {
		progmanager_pid_info(pids[i], &cpu_user, &cpu_system, name, sizeof(name));
		char buf[100];
		memset(&buf, 0, 100);
		sprintf(buf, "%.2lfW %s\n", (utils[i]/100.f)*current_datapoint, name);
		strcpy(&programtext[pos], buf);
		pos += (int)strlen(buf);
	}

	gkrellm_draw_decal_text(panel_programms, programdecal, programtext, 0);
	gkrellm_draw_panel_layers(panel_programms);
}


GkrellmMonitor *gkrellm_init_plugin(void) {
	setlocale(LC_NUMERIC, "C");
	pGK = gkrellm_ticks();
	style_id = gkrellm_add_meter_style(&plugin_mon, STYLE_NAME);
	monitor = &plugin_mon;
	return &plugin_mon;
}


static void create_plugin(GtkWidget *vbox, gint first_create) {
	vbox_plugin = vbox;
	if (first_create)	{
		chart = gkrellm_chart_new0();
		panel_watt = gkrellm_panel_new0();
		panel_programms = gkrellm_panel_new0();

		if(strlen(powerlog_path) == 0)
			sprintf(powerlog_path, "%s", "powerlog.csv");
		powermeter_library_init(powerlog_path);
		if(strlen(powertarget_name) != 0)
			powermeter_library_set_powertarget_by_name(powertarget_name);

		progmanager_library_init();
	}

	//Create the styles
	GkrellmStyle *style;
	GkrellmTextstyle *textstyle;
	style = gkrellm_meter_style(style_id);
	textstyle = gkrellm_meter_alt_textstyle(style_id);

	//Create the upper panel
	pw_decal1 = gkrellm_create_decal_text(panel_watt, "Aylmao", textstyle, style, 3, 5, -1);
	gkrellm_draw_decal_text(panel_watt, pw_decal1, "starttest", 0);
	gkrellm_panel_configure(panel_watt, NULL, style);
	gkrellm_panel_create(vbox, monitor, panel_watt);


	//Create the chart.
	gkrellm_set_chart_height_default(chart, 50);
	gkrellm_chart_create(vbox, monitor, chart, &chartconfig);
	chartdata = gkrellm_add_default_chartdata(chart, "Plugin Data");
	gkrellm_monotonic_chartdata(chartdata, FALSE);
	gkrellm_alloc_chartdata(chart);

	//Chartconfig
	gkrellm_set_chartconfig_auto_grid_resolution(chartconfig, FALSE);
	gkrellm_chartconfig_grid_resolution_adjustment(chartconfig, FALSE, 1000, 0, 25, 1, 10, 2, 150);
	gkrellm_chartconfig_grid_resolution_label(chartconfig, "Watts");
	gkrellm_set_chartconfig_flags(chartconfig, NO_CONFIG_AUTO_GRID_RESOLUTION);



	//Create the second panel.
	char buf[num_progs*10];
	for(int i = 0; i < num_progs; i++){
		char* p = (char*)&buf[i*10];
		if(i == num_progs -1) sprintf(p, "Aylmao,        ");
		else sprintf(p, "Aylmao,\n       ");
	}
	programdecal = gkrellm_create_decal_text(panel_programms, buf, textstyle, style, 5, 5, -1);
	gkrellm_draw_decal_text(panel_programms, programdecal, "starttext", 0);
	gkrellm_panel_configure(panel_programms, NULL, style);
	gkrellm_panel_create(vbox_plugin, monitor, panel_programms);


	if (first_create) {
		g_signal_connect(G_OBJECT(chart->drawing_area), "expose_event",
						 G_CALLBACK(chart_expose_event), NULL);
		g_signal_connect(G_OBJECT(panel_watt->drawing_area), "expose_event",
						 G_CALLBACK(panel_watt_expose_event), NULL);
		g_signal_connect(G_OBJECT(panel_programms->drawing_area), "expose_event",
						 G_CALLBACK(panel_programms_expose_event), NULL);
		g_signal_connect(G_OBJECT(chart->drawing_area), "button_press_event",
						 G_CALLBACK(chart_config_event), NULL);
		g_signal_connect(G_OBJECT(panel_watt->drawing_area), "button_press_event",
						 G_CALLBACK(panel_config_event), NULL);
		g_signal_connect(G_OBJECT(panel_programms->drawing_area), "button_press_event",
						 G_CALLBACK(panel_config_event), NULL);
	}
}



//as seen in https://github.com/dud3/gkrellm/blob/master/src/cpu.c
static void create_plugin_tab(GtkWidget *tab_vbox){
	GtkWidget		*tabs;
	GtkWidget		*vbox, *vbox1, *hbox;
	GtkWidget		*text;

	/* Make a couple of tabs.  One for setup and one for info	*/
	tabs = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_TOP);
	gtk_box_pack_start(GTK_BOX(tab_vbox), tabs, TRUE, TRUE, 0);

/* --Setup tab */
	vbox = gkrellm_gtk_framed_notebook_page(tabs, "Setup");

	//Combobox for the different Rapl targets
	vbox1 = gkrellm_gtk_category_vbox(vbox, "RAPL Domain", 4, 0, TRUE);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
	text_format_combobox = gtk_combo_box_entry_new_text();
	gtk_box_pack_start(GTK_BOX(hbox), text_format_combobox, TRUE, TRUE, 0);

	int active = 0;
	//Append all the different options to the dropdown.
	for(int i = 0; i < powermeter_library_powertarget_count(); i++){
		const char *path, *name;
		powermeter_library_get_powertarget(i, &path, &name);
		gtk_combo_box_append_text(GTK_COMBO_BOX(text_format_combobox), name);
		if(strcmp(name, powertarget_name) == 0)
			active = i;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(text_format_combobox), active);

	//Integer selection for number of active programs
	vbox1 = gkrellm_gtk_category_vbox(vbox, "Panel Options", 0, 0, TRUE);
	gkrellm_gtk_spin_button(vbox1, &prognumber_spinbutton,
							num_progs, 1, 20, 1, 1, 0, 0, NULL, NULL, FALSE, "Number of programs shown");

	//Textinput for powerlog path
	vbox1 = gkrellm_gtk_category_vbox(vbox, "Logfile Path", 0, 0, TRUE);
	powerlog_path_textbox = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(powerlog_path_textbox), powermeter_library_get_logpath());
	gtk_box_pack_start(GTK_BOX(vbox1), powerlog_path_textbox, TRUE, TRUE, 0);

/* --Info tab */
	vbox = gkrellm_gtk_framed_notebook_page(tabs, "Info");
	text = gkrellm_gtk_scrolled_text_view(vbox, NULL,
										  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	//for (i = 0; i < sizeof(plugin_info_text)/sizeof(gchar *); ++i)
	gkrellm_gtk_text_view_append(text, "<h>Gkrellm Powermeter\n");
	gkrellm_gtk_text_view_append(text, "Plugin by Hannes Pillny\n");
	gkrellm_gtk_text_view_append(text, "Lehrstuhl für Angewandte Informatik II Parallele und verteilte Systeme\n");
	gkrellm_gtk_text_view_append(text, "Universitaet Bayreuth\n");
	gkrellm_gtk_text_view_append(text, "2021\n");

}

static void apply_plugin_config(){
	//Check if the powertarget has changed.
	//then set the new powertarget
	char* name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(text_format_combobox));
	if(strcmp(powertarget_name, name) != 0){
		powermeter_library_set_powertarget_by_name(name);
		gkrellm_reset_chart(chart);
		sprintf(powertarget_name, "%s", name);

		float buf[300];
		powermeter_library_get_history(name, buf, 300);
		for(int i = 0; i < 300; i++){
			gkrellm_store_chartdata(chart, 0, (int)(buf[i]*1000));
		}
	}

	//change the number of programs shown.
	//Create the styles
	GkrellmStyle *style;
	GkrellmTextstyle *textstyle;
	style = gkrellm_meter_style(style_id);
	textstyle = gkrellm_meter_alt_textstyle(style_id);
	//get the current number
	num_progs = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prognumber_spinbutton));

	//redraw the second panel.
	gkrellm_remove_decal(panel_programms, programdecal);
	gkrellm_destroy_decal(programdecal);
	char buf[num_progs*10];
	for(int i = 0; i < num_progs; i++){
		char* p = (char*)&buf[i*10];
		if(i == num_progs -1) sprintf(p, "Aylmao,        ");
		else sprintf(p, "Aylmao,\n       ");
	}
	programdecal = gkrellm_create_decal_text(panel_programms, buf, textstyle, style, 5, 5, -1);
	gkrellm_draw_decal_text(panel_programms, programdecal, "starttext", 0);
	gkrellm_panel_configure(panel_programms, NULL, style);
	gkrellm_panel_create(vbox_plugin, monitor, panel_programms);


	//Read the logfile path and set the new logger on change
	if(strcmp(gtk_entry_get_text(GTK_ENTRY(powerlog_path_textbox)), powermeter_library_get_logpath()) != 0){
		powermeter_library_set_logpath(gtk_entry_get_text(GTK_ENTRY(powerlog_path_textbox)));
	}
}


static void save_config(FILE *f){
	fprintf(f, "%s current_powertarget_name %s\n", POWERMETER_CONFIG_KEYWORD, powertarget_name);
	fprintf(f, "%s show_name %d\n", POWERMETER_CONFIG_KEYWORD, show_powertarget_name);
	fprintf(f, "%s active_programs %d\n", POWERMETER_CONFIG_KEYWORD, num_progs);
	fprintf(f, "%s powerlog_path %s\n", POWERMETER_CONFIG_KEYWORD, powermeter_library_get_logpath());

	gkrellm_save_chartconfig(f, chartconfig, POWERMETER_CONFIG_KEYWORD, NULL);
}

static void load_config(gchar *arg) {
	gchar config[32], item[CFG_BUFSIZE];
	gint n;

	n = sscanf(arg, "%31s %[^\n]", config, item);
	if (n == 2) {
		if(!strcmp(config, "current_powertarget_name"))
			sscanf(item, "%s", powertarget_name);
		else if (!strcmp(config, "show_name"))
			sscanf(item, "%d", &show_powertarget_name);
		else if (!strcmp(config, "active_programs"))
			sscanf(item, "%d", &num_progs);
		else if (!strcmp(config, "powerlog_path"))
			sscanf(item, "%s", powerlog_path);
		else if (!strcmp(config, GKRELLM_CHARTCONFIG_KEYWORD))
			gkrellm_load_chartconfig(&chartconfig, item, 1);
	}
}

static gint panel_watt_expose_event(GtkWidget *widget, GdkEventExpose *ev) {
	gdk_draw_pixmap(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
					panel_watt->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
					ev->area.width, ev->area.height);
	return FALSE;
}

static gint panel_programms_expose_event(GtkWidget *widget, GdkEventExpose *ev) {
	gdk_draw_pixmap(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
					panel_programms->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
					ev->area.width, ev->area.height);
	return FALSE;
}

static gint chart_expose_event(GtkWidget *widget, GdkEventExpose *ev) {
	gdk_draw_pixmap(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
					chart->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
					ev->area.width, ev->area.height);
	return FALSE;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedLocalVariable"
gint chart_config_event(GtkWidget *widget, GdkEventButton *ev) {
	//printf("chartEvent!\n");
	if (ev->button == 3 || (ev->button == 1 && ev->type == GDK_2BUTTON_PRESS))
		gkrellm_chartconfig_window_create(chart);
	else if(ev->button == 1)
		show_powertarget_name = !show_powertarget_name;
	return 0;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedLocalVariable"
gint panel_config_event(GtkWidget *widget, GdkEventButton *ev) {
	//printf("panelEvent!\n");
	if (ev->button == 3 || (ev->button == 1 && ev->type == GDK_2BUTTON_PRESS))
		gkrellm_open_config_window(monitor);
	return 0;
}
#pragma clang diagnostic pop
