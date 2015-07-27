/*
 * main.c - Sample Project Main Entry Point
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

#include <stdlib.h>
#include "sample-window.h"

int main(int argc, char **argv)
{
        /* We never access this again, and will get a compiler warning
         * about an unused variable. Mark it unused */
        __attribute__ ((unused)) SampleWindow *window;

        /* Initialize the GTK system */
        gtk_init(&argc, &argv);

        /* Construct a new SampleWindow, and run the main loop */
        window = sample_window_new();
        gtk_main();

        return EXIT_SUCCESS;
}
