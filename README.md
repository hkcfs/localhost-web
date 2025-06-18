# FreshRSS App

A simple GTK4 and WebKitGTK-based desktop application to display a FreshRSS instance or any other web page.

## Features

*   Displays a web page (configurable, defaults to a FreshRSS instance placeholder).
*   Customizable window title.
*   Application logo displayed in the header.
*   Basic command-line options for setting the URL and title.
*   `.desktop` file for integration with Linux desktop environments (e.g., GNOME).

## Prerequisites

*   **GTK4 Development Libraries:** `gtk4`
*   **WebKitGTK (6.0) Development Libraries:** `webkitgtk-6.0` (or `webkit2gtk-4.1` if you adapt the code for GTK3/WebKitGTK4.1)
*   **GLib Development Libraries:** `glib2` (for `glib-compile-resources` and core utilities)
*   **A C Compiler:** `gcc` or `clang`
*   **Make:** For using the provided Makefile.
*   **pkg-config:** For discovering compiler and linker flags.

On Arch Linux, you can install these with:
```bash
sudo pacman -Syu --needed base-devel gtk4 webkitgtk-6.0 glib2
```

## Building the Application

1.  **Clone the Repository (or create the files):**
    Ensure you have the following files in your project directory:
    *   `freshrss-app.c` (the C source code)
    *   `resources.xml` (for embedding resources like the logo)
    *   `logo.png` (your application icon, e.g., 64x64 or 128x128)
    *   `freshrss-app.desktop` (the desktop entry file)
    *   `Makefile` (the build instructions file)

2.  **Configure Defaults (Optional but Recommended):**
    Open `freshrss-app.c` and modify the following default values if needed:
    ```c
    static const char *DEFAULT_FRESHRSS_URL = "https://YOUR_FRESHRSS_INSTANCE_URL_HERE";
    static const char *DEFAULT_WINDOW_TITLE = "FreshRSS App";
    ```
    Replace `https://YOUR_FRESHRSS_INSTANCE_URL_HERE` with the actual URL you want to load by default.

3.  **Compile (Dynamic Linking - Recommended):**
    Open a terminal in the project directory and run:
    ```bash
    make
    ```
    This will create an executable named `freshrss-app`.

    Alternatively, for a quick single-command compilation:
    ```bash
    glib-compile-resources resources.xml --target=freshrss-app-resources.c --sourcedir=. --generate-source --c-name freshrss_app && \
    gcc freshrss-app.c freshrss-app-resources.c -o freshrss-app $(pkg-config --cflags --libs gtk4 webkitgtk-6.0)
    ```

## Running the Application

*   **Directly:**
    ```bash
    ./freshrss-app
    ```

*   **With Command-Line Options:**
    ```bash
    ./freshrss-app --help  # To see available options

    # Example: Load a specific URL and set a custom title
    ./freshrss-app --url https://news.ycombinator.com --title "Hacker News Reader"

    # Example: Use positional arguments (URL then Title)
    ./freshrss-app https://www.archlinux.org "Arch Linux News"

    # Example: Use the built-in self-test URL (google.com)
    ./freshrss-app --self-test-url
    ```

## Installing (Local User Install)

This installs the application for the current user without requiring root privileges.

1.  **Compile the application** as described above.
2.  **Run the install command from the Makefile:**
    ```bash
    make install-local
    ```
    (This assumes the Makefile has an `install-local` target like the one typically configured for `PREFIX=~/.local`)

    Or, manually:
    ```bash
    mkdir -p ~/.local/bin
    mkdir -p ~/.local/share/applications
    mkdir -p ~/.local/share/icons/hicolor/64x64/apps # Or other appropriate icon sizes

    cp freshrss-app ~/.local/bin/
    cp freshrss-app.desktop ~/.local/share/applications/
    cp logo.png ~/.local/share/icons/hicolor/64x64/apps/freshrss-app.png # Ensure icon name matches .desktop

    update-desktop-database ~/.local/share/applications
    gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor
    ```

After installation, "FreshRSS App" should appear in your desktop environment's application launcher.

## Uninstalling (Local User Install)

1.  **Run the uninstall command from the Makefile:**
    ```bash
    make uninstall-local
    ```
    (This assumes the Makefile has an `uninstall-local` target.)

    Or, manually:
    ```bash
    rm -f ~/.local/bin/freshrss-app
    rm -f ~/.local/share/applications/freshrss-app.desktop
    rm -f ~/.local/share/icons/hicolor/64x64/apps/freshrss-app.png

    update-desktop-database ~/.local/share/applications
    gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor
    ```

## Static Linking (Advanced and Experimental)

The provided Makefile also includes an *attempt* at static linking. This is highly complex and requires you to have static versions (`.a` files) of GTK4, WebKitGTK, and all their numerous dependencies manually built and installed.

To attempt a static build (expect linker errors if static libraries are not available):
```bash
# Set LINK_MODE=static (if Makefile supports this variable)
# or use a specific target like `make static_build` if defined.
# The previous Makefile examples used an LDFLAGS_STATIC directly.
make clean
# If using the most recent Makefile example that defaults to static-like linking:
make
```
Refer to the Makefile comments for more details on the challenges of static linking. It is generally **not recommended** for applications of this complexity due to the effort involved and downsides regarding updates and size.

## Contributing

Feel free to fork the project, make improvements, and submit pull requests. Suggestions and bug reports are also welcome via issues.
