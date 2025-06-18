# Makefile for FreshRSS App - Attempted Static Build (DEBUGGING LDFLAGS)

APP_NAME = freshrss-app
APP_ID = org.example.freshrssapp
PREFIX ?= /usr/local # For installation target
DESTDIR ?=

# Source files
C_SOURCES = freshrss-app.c freshrss-app-resources.c
OBJS = $(C_SOURCES:.c=.o)

# Compiler
CC = gcc

# --- CFLAGS (Compiler Flags) ---
CFLAGS_PKG = $(shell pkg-config --cflags gtk4 webkitgtk-6.0)
CFLAGS = $(CFLAGS_PKG)
# If your static libs are in a custom prefix like /opt/my-static-libs, add:
# CFLAGS += -I/opt/my-static-libs/include

# --- LDFLAGS (Linker Flags) for STATIC build ---

# Path where your self-compiled/obtained static libraries MIGHT reside.
# For debugging the "/lib: read: Is a directory" error,
# let's explicitly set the primary library path.
# Change this if your static libraries are in a specific custom location like /opt/my-static-libs/lib
PRIMARY_STATIC_LIB_DIR = /usr/lib

# Define library search paths.
# This should ONLY contain -L options.
# We are hardcoding one path here for debugging the specific error.
LDFLAGS_STATIC_PATHS = -L$(PRIMARY_STATIC_LIB_DIR)
# If you have static libs in other specific directories, add them:
# LDFLAGS_STATIC_PATHS += -L/path/to/other/static/libs

# List of libraries to attempt to link statically.
STATIC_LIBS = \
    -lwebkitgtk-6.0 \
    -ljavascriptcoregtk-6.0 \
    -lsoup-3.0 \
    -lgtk-4 \
    -lgsk-4 \
    -lgdk-4 \
    -lpangocairo-1.0 \
    -lpangoft2-1.0 \
    -lpango-1.0 \
    -lharfbuzz \
    -lfontconfig \
    -lfreetype \
    -lcairo \
    -lcairo-gobject \
    -lgdk_pixbuf-2.0 \
    -lgio-2.0 \
    -lgobject-2.0 \
    -lglib-2.0 \
    -lxml2 \
    -lxslt \
    -licuuc -licudata -licui18n \
    -lsqlite3 \
    -lhyphen \
    -lwoff2dec \
    -lwebp \
    -lopenjp2 \
    -ljpeg \
    -lpng16 \
    -lz \
    -lbrotlidec -lbrotlicommon \
    -lgmodule-2.0 \
    -lharfbuzz-subset \
    -lgraphite2 \
    -lpixman-1 \
    -lexpat \
    -luuid \
    -lEGL \
    -lGL \
    -lX11 -lXext -lXfixes -lXrender -lXdamage -lXcomposite -lXrandr -lXi \
    -lwayland-client -lwayland-cursor -lwayland-egl \
    -lxkbcommon \
    -lepoxy \
    -ldbus-1 \
    -lsystemd

LDFLAGS_STATIC = $(LDFLAGS_STATIC_PATHS) -Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic -lpthread -lm -ldl -lrt

# --- glib-compile-resources ---
GLIB_COMPILE_RESOURCES = glib-compile-resources

.PHONY: all clean install uninstall

all: $(APP_NAME)

freshrss-app-resources.c: resources.xml logo.png
	$(GLIB_COMPILE_RESOURCES) resources.xml --target=$@ --sourcedir=. --generate-source --c-name freshrss_app

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(APP_NAME): $(OBJS)
	@echo "--- DEBUG: Value of LDFLAGS_STATIC before linking ---"
	@echo "LDFLAGS_STATIC = $(LDFLAGS_STATIC)"
	@echo "--- Linking statically (attempting) ---"
	$(CC) $(OBJS) -o $@ $(LDFLAGS_STATIC)
	@echo "--- Static build attempt finished. Check for linker errors. ---"
	@echo "Run 'ldd $(APP_NAME)' to see what is still dynamically linked."

clean:
	rm -f $(OBJS) $(APP_NAME) freshrss-app-resources.c

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(APP_NAME) $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	cp freshrss-app.desktop $(DESTDIR)$(PREFIX)/share/applications/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/
	cp logo.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/freshrss-app.png
	if [ -x "$$(command -v gtk-update-icon-cache)" ]; then gtk-update-icon-cache -q -f $(DESTDIR)$(PREFIX)/share/icons/hicolor || true; fi
	if [ -x "$$(command -v update-desktop-database)" ]; then update-desktop-database -q $(DESTDIR)$(PREFIX)/share/applications || true; fi
	@echo "Installed $(APP_NAME) to $(DESTDIR)$(PREFIX)"

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(APP_NAME)
	rm -f $(DESTDIR)$(PREFIX)/share/applications/freshrss-app.desktop
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/freshrss-app.png
	if [ -x "$$(command -v gtk-update-icon-cache)" ]; then gtk-update-icon-cache -q -f $(DESTDIR)$(PREFIX)/share/icons/hicolor || true; fi
	if [ -x "$$(command -v update-desktop-database)" ]; then update-desktop-database -q $(DESTDIR)$(PREFIX)/share/applications || true; fi
	@echo "Uninstalled $(APP_NAME)"
