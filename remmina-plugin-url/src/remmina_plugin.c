/*
 *     Project: Remmina Plugin URL
 * Description: Remmina protocol plugin to open an URL in an external browser.
 *      Author: Fabio Castelli (Muflone) <muflone@vbsimple.net>
 *   Copyright: 2011-2016 Fabio Castelli (Muflone)
 *     License: GPL-2+
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of ERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "plugin_config.h"
#include <remmina/remmina_plugin.h>
#if GTK_VERSION == 3
  # include <gtk/gtkx.h>
#endif

static gpointer browsers_list[] =
{
  "xdg-open", "Automatically detected",
  "x-www-browser", "Graphical browser",
  "www-browser", "Console browser",
  "gnome-www-browser", "GNOME default browser",
  "firefox", "Mozilla Firefox",
  "iceweasel", "Iceweasel",
  "epiphany", "Epiphany",
  NULL
};

static RemminaPluginService *remmina_plugin_service = NULL;

static void remmina_plugin_init(RemminaProtocolWidget *gp)
{
  remmina_plugin_service->log_printf("[%s] remmina_plugin_init\n", PLUGIN_NAME);
}

static gboolean remmina_plugin_open_connection(RemminaProtocolWidget *gp)
{
  remmina_plugin_service->log_printf("[%s] remmina_plugin_open_connection\n", PLUGIN_NAME);
  #define GET_PLUGIN_STRING(value) \
    g_strdup(remmina_plugin_service->file_get_string(remminafile, value))
  #define GET_PLUGIN_BOOLEAN(value) \
    remmina_plugin_service->file_get_int(remminafile, value, FALSE)
  #define GET_PLUGIN_INT(value, default_value) \
    remmina_plugin_service->file_get_int(remminafile, value, default_value)
  #define GET_PLUGIN_PASSWORD(value) \
    g_strdup(remmina_plugin_service->file_get_secret(remminafile, value));

  RemminaFile *remminafile;
  gboolean ret;
  GPid pid;
  GError *error = NULL;
  gchar *argv[50];
  gint argc;
  gint i;

  remminafile = remmina_plugin_service->protocol_plugin_get_file(gp);

  argc = 0;
  argv[argc++] = g_strdup(GET_PLUGIN_STRING("browser"));
  argv[argc++] = g_strdup(GET_PLUGIN_STRING("server"));
  argv[argc++] = NULL;

  ret = g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
    NULL, NULL, &pid, &error);

  for (i = 0; i < argc; i++)
    g_free (argv[i]);

  if (!ret)
    remmina_plugin_service->protocol_plugin_set_error(gp, "%s", error->message);

  return FALSE;
}

static gboolean remmina_plugin_close_connection(RemminaProtocolWidget *gp)
{
  remmina_plugin_service->log_printf("[%s] remmina_plugin_close_connection\n", PLUGIN_NAME);
  remmina_plugin_service->protocol_plugin_emit_signal(gp, "disconnect");
  return FALSE;
}

static const RemminaProtocolSetting remmina_plugin_basic_settings[] =
{
  { REMMINA_PROTOCOL_SETTING_TYPE_TEXT, "server", N_("URL"), FALSE, NULL, NULL },
  { REMMINA_PROTOCOL_SETTING_TYPE_SELECT, "browser", N_("Browser"), FALSE, browsers_list, NULL },
  { REMMINA_PROTOCOL_SETTING_TYPE_END, NULL, NULL, FALSE, NULL, NULL }
};

static RemminaProtocolPlugin remmina_plugin =
{
  REMMINA_PLUGIN_TYPE_PROTOCOL,
  PLUGIN_NAME,
  PLUGIN_DESCRIPTION,
  GETTEXT_PACKAGE,
  PLUGIN_VERSION,
  PLUGIN_APPICON,
  PLUGIN_APPICON,
  remmina_plugin_basic_settings,
  NULL,
  REMMINA_PROTOCOL_SSH_SETTING_NONE,
  NULL,
  remmina_plugin_init,
  remmina_plugin_open_connection,
  remmina_plugin_close_connection,
  NULL,
  NULL
};

G_MODULE_EXPORT gboolean remmina_plugin_entry(RemminaPluginService *service)
{
  remmina_plugin_service = service;

  if (!service->register_plugin((RemminaPlugin *) &remmina_plugin))
  {
    return FALSE;
  }
  return TRUE;
}
