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
#include <glib/gprintf.h>
#include "sample-window.h"

/* If you share headers, or interface with other libraries, you do
 * not want to expose internals. Instead, use a private struct to store
 * instance related data */
struct _SampleWindowPriv {
        gchar* fname;
};

G_DEFINE_TYPE_WITH_PRIVATE(SampleWindow, sample_window, GTK_TYPE_WINDOW)

/* Boilerplate GObject code */
static void sample_window_class_init(SampleWindowClass *klass);
static void sample_window_init(SampleWindow *self);
static void sample_window_dispose(GObject *object);

/* Runs when the button is clicked */
static void fileset_cb(GtkWidget *widget, gpointer userdata);

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
        GtkWidget *grid, *button, *image;


        /* Initialize the private storage, the method is generated
         * for you when you use G_DEFINE_TYPE_WITH_PRIVATE, and
         * follows on from your base name (sample_window)
         */
        self->priv = sample_window_get_instance_private(self);

        /* Request a size before becoming visible */
        gtk_window_set_default_size(GTK_WINDOW(self), 400, 400);

        /* Ensure we quit when a user closes the window */
        g_signal_connect(self, "destroy", gtk_main_quit, NULL);

        /* make a grid layout thingy */
        grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

        /* make a file open button */
        button = gtk_file_chooser_button_new("Click me",
                GTK_FILE_CHOOSER_ACTION_OPEN);
        gtk_file_chooser_button_set_width_chars(
                GTK_FILE_CHOOSER_BUTTON(button),
                32);
        g_object_set(G_OBJECT(button), "margin", 10, NULL);

        /* Center the button so it doesn't fill available space */
        gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

        /* The clicked_cb method is called on click */
        g_signal_connect(button, "file-set", G_CALLBACK(fileset_cb),
                self);

        /* make an image */
        image = gtk_image_new();
        gtk_widget_set_size_request(image, 400, 400);

        /* add all the layout stuff */
        gtk_grid_attach(GTK_GRID(grid),
                        button,
                        0,0,1,1);
        gtk_grid_attach(GTK_GRID(grid),
                        image,
                        0,1,1,1);
        gtk_container_add(GTK_CONTAINER(self), grid);

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

static void fileset_cb(GtkWidget *widget, gpointer userdata)
{
        SampleWindow *self;
        gchar *fname = NULL;
        CvCapture* cap;
        IplImage* img;

        /* We passed our window via the userdata pointer */
        self = SAMPLE_WINDOW(userdata);
        fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
        self->priv->fname = fname;

        g_printf("filename: %s\n", fname);

        cap = cvCaptureFromFile(fname);
        img = cvQueryFrame(cap);
        if(img != NULL){
            g_printf("Got a non-null frame!");
        }

        cvReleaseCapture(cap);
}
