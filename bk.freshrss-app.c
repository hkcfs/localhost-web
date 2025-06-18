#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <glib/gstdio.h>

// Forward declaration for resource (will be generated)
extern GResource *freshrss_app_get_resource (void);

// --- Command-line Option Variables ---
static gchar *cli_opt_url = NULL;
static gchar *cli_opt_window_title = NULL;
static gboolean cli_opt_self_test_url_flag = FALSE;
static gchar **cli_remaining_args = NULL;

// --- Default values ---
static const char *DEFAULT_FRESHRSS_URL = "http://localhost:8090";
static const char *DEFAULT_WINDOW_TITLE = "FreshRSS App";
static const char *DEFAULT_TEST_URL = "https://www.google.com";

// --- GOptionEntry array for command-line arguments ---
static const GOptionEntry command_line_entries[] = {
  { "url", 'u', 0, G_OPTION_ARG_STRING, &cli_opt_url, "URL to load (e.g., https://rss.example.com)", "URL" },
  { "title", 't', 0, G_OPTION_ARG_STRING, &cli_opt_window_title, "Window title for the application", "TITLE" },
  { "self-test-url", 's', 0, G_OPTION_ARG_NONE, &cli_opt_self_test_url_flag, "Load a generic test URL (google.com)", NULL},
  { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &cli_remaining_args, NULL, "[URL] [TITLE]" },
  { NULL }
};

// --- 'command-line' signal handler ---
static int
on_command_line (GApplication *application, GApplicationCommandLine *cmdline)
{
  // Options have already been parsed and stored in our static variables
  // by g_application_add_main_option_entries before this handler is called.

  // For this application, the command line options just configure how the
  // main window behaves when it's activated. So, we just activate the application.
  g_application_activate (application);

  // Return 0 to indicate success.
  // The GApplicationCommandLine object is automatically unreffed by GLib
  // after this handler returns unless we explicitly ref it.
  return 0;
}


static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *header_box;
    GtkWidget *logo_image;
    GtkWidget *title_label;
    GtkWidget *webView;

    const char *url_to_load = DEFAULT_FRESHRSS_URL;
    const char *window_title_to_set = DEFAULT_WINDOW_TITLE;

    // --- Determine URL and Title based on command-line options ---
    if (cli_opt_self_test_url_flag) {
        url_to_load = DEFAULT_TEST_URL;
        g_print("Using self-test URL due to --self-test-url flag: %s\n", url_to_load);
    } else if (cli_opt_url) {
        url_to_load = cli_opt_url;
        g_print("Using URL from --url option: %s\n", url_to_load);
    } else if (cli_remaining_args && cli_remaining_args[0]) {
        url_to_load = cli_remaining_args[0];
        g_print("Using URL from positional argument: %s\n", url_to_load);
    } else {
        g_print("Using default URL: %s\n", url_to_load);
    }

    if (cli_opt_window_title) {
        window_title_to_set = cli_opt_window_title;
        g_print("Using window title from --title option: %s\n", window_title_to_set);
    } else if (cli_remaining_args && cli_remaining_args[0] && cli_remaining_args[1]) {
        window_title_to_set = cli_remaining_args[1];
        g_print("Using window title from positional argument: %s\n", window_title_to_set);
    } else {
        g_print("Using default window title: %s\n", window_title_to_set);
    }

    // --- Window Setup ---
    window = gtk_application_window_new(GTK_APPLICATION(app)); // Ensure app is cast
    gtk_window_set_title(GTK_WINDOW(window), window_title_to_set);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    gtk_window_set_icon_name(GTK_WINDOW(window), "freshrss-app");

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(header_box, 10);
    gtk_widget_set_margin_end(header_box, 10);
    gtk_widget_set_margin_top(header_box, 10);
    gtk_widget_set_margin_bottom(header_box, 10);
    gtk_box_append(GTK_BOX(main_box), header_box);

    logo_image = gtk_image_new_from_resource("/org/example/freshrssapp/logo.png");
    gtk_box_append(GTK_BOX(header_box), logo_image);

    title_label = gtk_label_new(window_title_to_set);
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(attrs, pango_attr_size_new(16 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(title_label), attrs);
    pango_attr_list_unref(attrs);
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(title_label, TRUE);
    gtk_box_append(GTK_BOX(header_box), title_label);

    webView = webkit_web_view_new();
    gtk_widget_set_vexpand(webView, TRUE);
    gtk_box_append(GTK_BOX(main_box), webView);

    g_print("Final URL to load in WebView: %s\n", url_to_load);
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), url_to_load);

    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    g_resources_register(freshrss_app_get_resource());

    app = gtk_application_new("org.example.freshrssapp", G_APPLICATION_HANDLES_COMMAND_LINE);

    g_application_add_main_option_entries(G_APPLICATION(app), command_line_entries);

    // Connect 'activate' (called when app starts normally, or after command-line processing if no specific action taken)
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    // Connect 'command-line' (called when app is invoked with command-line args and G_APPLICATION_HANDLES_COMMAND_LINE is set)
    // This signal is emitted *before* 'activate' if command-line arguments are present (other than for a new instance of a unique app).
    g_signal_connect(app, "command-line", G_CALLBACK(on_command_line), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);

    if (cli_remaining_args) {
        g_strfreev(cli_remaining_args);
    }
    // cli_opt_url and cli_opt_window_title are managed by GOptionContext.

    g_object_unref(app);
    g_resources_unregister(freshrss_app_get_resource());
    return status;
}
