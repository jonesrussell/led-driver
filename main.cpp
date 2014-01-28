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

#include <locale>
#include <iostream>
#include <vector>

#include <unistd.h>
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

    if (argc == 2 && !strcmp(argv[1], "off")) {
        lightpack.SetLedsOff();
        lightpack.Close();
        return 0;
    }

    lightpack.SetOption("refresh-delay", 100);
    lightpack.SetOption("color-depth", 128);
    lightpack.SetOption("smooth-slowdown", 255);

    LedColors colors(LP_LEDS);

    for ( ;; ) {
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

        lightpack.SetLedColors(colors);
        usleep(1500000);
    }

    return 0;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
