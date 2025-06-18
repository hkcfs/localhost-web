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
static const char *DEFAULT_FRESHRSS_URL = "http://localhost:8090"; // CONFIGURE THIS
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
  g_application_activate (application);
  return 0;
}


static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box;
    // GtkWidget *header_box; // No longer needed
    GtkWidget *logo_image_for_headerbar; // For the GtkHeaderBar
    // GtkWidget *title_label; // Title is handled by GtkHeaderBar
    GtkWidget *webView;
    GtkWidget *header_bar; // The new GtkHeaderBar

    const char *url_to_load = DEFAULT_FRESHRSS_URL;
    const char *window_title_to_set = DEFAULT_WINDOW_TITLE;

    // --- Determine URL and Title based on command-line options (same as before) ---
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
    window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), window_title_to_set); // GtkHeaderBar will use this
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    gtk_window_set_icon_name(GTK_WINDOW(window), "freshrss-app");

    // --- Create and Set GtkHeaderBar ---
    header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header_bar), TRUE); // Show min/max/close buttons

    // Add logo to the start of the header bar
    logo_image_for_headerbar = gtk_image_new_from_resource("/org/example/freshrssapp/logo.png");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), logo_image_for_headerbar);
    // The title set on the window will be displayed by the header bar.
    // If you wanted a custom widget for the title:
    // GtkWidget *custom_title_label = gtk_label_new(window_title_to_set);
    // gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header_bar), custom_title_label);

    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar); // This replaces the WM title bar

    // --- Main Layout Box (Vertical) - now only contains the webView ---
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_box); // main_box is the window's direct child content area

    // --- WebKitWebView ---
    webView = webkit_web_view_new();
    gtk_widget_set_vexpand(webView, TRUE); // Make webview fill remaining vertical space
    gtk_box_append(GTK_BOX(main_box), webView); // Add webView to the main_box

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
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "command-line", G_CALLBACK(on_command_line), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);

    if (cli_remaining_args) {
        g_strfreev(cli_remaining_args);
    }

    g_object_unref(app);
    g_resources_unregister(freshrss_app_get_resource());
    return status;
}
