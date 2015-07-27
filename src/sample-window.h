/*
 * sample-window.h - Add description
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
#ifndef sample_window_h
#define sample_window_h

#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct _SampleWindow SampleWindow;
typedef struct _SampleWindowClass   SampleWindowClass;
typedef struct _SampleWindowPriv SampleWindowPrivate;

#define SAMPLE_WINDOW_TYPE (sample_window_get_type())
#define SAMPLE_WINDOW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SAMPLE_WINDOW_TYPE, SampleWindow))
#define IS_SAMPLE_WINDOW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SAMPLE_WINDOW_TYPE))
#define SAMPLE_WINDOW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), SAMPLE_WINDOW_TYPE, SampleWindowClass))
#define IS_SAMPLE_WINDOW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SAMPLE_WINDOW_TYPE))
#define SAMPLE_WINDOW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SAMPLE_WINDOW_TYPE, SampleWindowClass))

/* SampleWindow object */
struct _SampleWindow {
        GtkWindow parent;
        SampleWindowPrivate *priv;
};

/* SampleWindow class definition */
struct _SampleWindowClass {
        GtkWindowClass parent_class;
};

GType sample_window_get_type(void);

/* SampleWindow methods */

/**
 * Construct a new SampleWindow
 * @return A new SampleWindow
 */
SampleWindow *sample_window_new(void);

#endif /* sample_window_h */
