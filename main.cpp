// LED Driver
// Copyright (C) 2014 Darryl Sokoloski <darryl@sokoloski.ca>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale>
#include <iostream>
#include <vector>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hidapi.h"

#include "ld-color.h"
#include "ld-abstract.h"
#include "ld-lightpack.h"

int main(int argc, char *argv[])
{
    std::locale::global(std::locale(""));

    LedDriverLightpack lightpack;

    lightpack.Open();

    if (argc == 2 && strcmp(argv[1], "on") == 0) {
        FILE *ph = popen("led-driver 2>/tmp/led-driver.log &", "r");
        return 0;
    }
    else if (argc == 2 && strcmp(argv[1], "off") == 0) {
        lightpack.SetLedsOff();
        lightpack.Close();
        return 0;
    }

    lightpack.SetOption("refresh-delay", 100);
    lightpack.SetOption("color-depth", 128);

    LedColors colors(LP_LEDS);

    for ( ;; ) {
#if 1
        for (int l = 0; l < LP_LEDS; l++) {
            switch (rand() % 7) {
            case 0:
                colors[l].r = 255;
                colors[l].g = 0;
                colors[l].b = 0;
                break;
            case 1:
                colors[l].r = 0;
                colors[l].g = 255;
                colors[l].b = 0;
                break;
            case 2:
                colors[l].r = 0;
                colors[l].g = 0;
                colors[l].b = 255;
                break;
            case 3:
                colors[l].r = 255;
                colors[l].g = 255;
                colors[l].b = 0;
                break;
            case 4:
                colors[l].r = 0;
                colors[l].g = 255;
                colors[l].b = 255;
                break;
            case 5:
                colors[l].b = 255;
                colors[l].g = 0;
                colors[l].r = 255;
                break;
            case 6:
                colors[l].b = 255;
                colors[l].g = 255;
                colors[l].r = 0;
                break;
            }
        }

        lightpack.SetOption("smooth-slowdown", 255);
        lightpack.SetLedColors(colors);
        //usleep(50000);
        usleep(1500000);
#else
        lightpack.SetOption("smooth-slowdown", 0);
        lightpack.SetLedsOff();

        for (int l = 0; l < LP_LEDS; l++) {
            colors[l].r = 255;
            colors[l].g = 0;
            colors[l].b = 0;
        }

        lightpack.SetLedColors(colors);
        usleep(500000);

        lightpack.SetLedsOff();
        lightpack.SetOption("smooth-slowdown", 255);
        lightpack.SetLedColors(colors);
        usleep(1500000);
#endif
    }

    return 0;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
