#!/bin/bash

./gif-author -d 30 -o maru.gif -s 500x_ input/*.png
./gif-author -d 30 -o maru2.gif --crop 325,278+500x281 input/*.png
