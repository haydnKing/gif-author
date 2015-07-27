/*
 * sample-window.c
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <string.h>
#include "sample-window.h"

/* If you share headers, or interface with other libraries, you do
 * not want to expose internals. Instead, use a private struct to store
 * instance related data */
struct _SampleWindowPriv {
        int hit_count;
};

G_DEFINE_TYPE_WITH_PRIVATE(SampleWindow, sample_window, GTK_TYPE_WINDOW)

/* Boilerplate GObject code */
static void sample_window_class_init(SampleWindowClass *klass);
static void sample_window_init(SampleWindow *self);
static void sample_window_dispose(GObject *object);

/* Runs when the button is clicked */
static void button_cb(GtkWidget *widget, gpointer userdata);

/* Initialisation */
static void sample_window_class_init(SampleWindowClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        /* Override parent methods */
        g_object_class->dispose = &sample_window_dispose;
}

static void sample_window_init(SampleWindow *self)
{
        GtkWidget *button;

        /* Initialize the private storage, the method is generated
         * for you when you use G_DEFINE_TYPE_WITH_PRIVATE, and
         * follows on from your base name (sample_window)
         */
        self->priv = sample_window_get_instance_private(self);

        /* Request a size before becoming visible */
        gtk_window_set_default_size(GTK_WINDOW(self), 400, 400);

        /* Ensure we quit when a user closes the window */
        g_signal_connect(self, "destroy", gtk_main_quit, NULL);

        /* Add a new button to ourselves */
        button = gtk_button_new_with_label("Click me");
        gtk_container_add(GTK_CONTAINER(self), button);

        /* Center the button so it doesn't fill available space */
        gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

        /* The clicked_cb method is called on click */
        g_signal_connect(button, "clicked", G_CALLBACK(button_cb),
                self);

        /* Show ourselves */
        gtk_widget_show_all(GTK_WIDGET(self));
}

static void sample_window_dispose(GObject *object)
{
        /* Chain load destruction up to parent class */
        G_OBJECT_CLASS (sample_window_parent_class)->dispose (object);
}

/* Utility; return a new SampleWindow */
SampleWindow *sample_window_new(void)
{
        SampleWindow *self;

        self = g_object_new(SAMPLE_WINDOW_TYPE, NULL);
        return self;
}

static void button_cb(GtkWidget *widget, gpointer userdata)
{
        SampleWindow *self;
        gchar *text = NULL;
        int hit_count;

        /* We passed our window via the userdata pointer */
        self = SAMPLE_WINDOW(userdata);
        hit_count = self->priv->hit_count + 1;

        /* Set the label to a new string */
        text = g_strdup_printf("Clicked: #%d", hit_count);
        if (!text) {
                g_warning("Unable to allocate memory!");
                return;
        }
        self->priv->hit_count = hit_count;
        gtk_button_set_label(GTK_BUTTON(widget), text);

        /* Always cleanup */
        g_free(text);
}
