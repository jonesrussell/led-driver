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

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include <stdbool.h>
#include <string.h>
#include <libusb.h>

#include "hidapi.h"

#include "ld-color.h"
#include "ld-abstract.h"
#include "ld-lightpack.h"

LedDriverLightpack::LedDriverLightpack()
    : dh(NULL), buffer_read(NULL), buffer_write(NULL)
{
    buffer_read = new unsigned char[LP_BUFFER_SIZE];
    if (buffer_read == NULL) {
        throw std::runtime_error("buffer_read: out of memory");
    }
    buffer_write = new unsigned char[LP_BUFFER_SIZE];
    if (buffer_write == NULL) {
        delete [] buffer_read;
        throw std::runtime_error("buffer_write: out of memory");
    }
}

LedDriverLightpack::~LedDriverLightpack()
{
    delete [] buffer_read;
    delete [] buffer_write;
    hid_close(dh);
}

void LedDriverLightpack::Open(void)
{
    struct hid_device_info *device, *devices;

    device = devices = hid_enumerate(LP_USB_VID, LP_USB_PID);

    while (device) {
        if (device->path) {
            dh = hid_open_path(device->path);

            if(dh != NULL) {
                hid_set_nonblocking(dh, 1);
                
                if(device->serial_number != NULL &&
                    wcslen(device->serial_number) > 0) {
                    std::cerr << "found lightpack: "
                        << device->path << std::endl;
                }
                else {
                    std::cerr << "found lightpack, no serial: "
                        << device->path << std::endl;
                }
            }
            else
                std::cerr << "lightpack not found" << std::endl;

            break;
        }

        device = device->next;
    }

    hid_free_enumeration(devices);

    if (dh == NULL)
        throw std::runtime_error("no devices found");
}

void LedDriverLightpack::Close(void)
{
    if (dh != NULL) {
        hid_close(dh);
        dh = NULL;
    }
}

void LedDriverLightpack::SetLedsOff(void)
{
    Write(LPOP_SET_LEDS_OFF);
}

void LedDriverLightpack::SetLedColors(LedColors &colors)
{
    int index;
    const size_t led_color_size = 6;
    //const int led_remap_default[LP_LEDS] = { 4, 3, 2, 0, 1, 5, 6, 7, 8, 9 };
    const int led_remap_custom[LP_LEDS] = { 4, 3, 0, 1, 2, 5, 6, 7, 8, 9 };
    const double k = 4095 / 255.0;

    memset(buffer_write, 0, LP_BUFFER_SIZE);

    for (int i = 0; i < colors.size(); i++) {
        LedColor color;

        // Normalize to 12-bit
        color.r = colors[i].r * k;
        color.g = colors[i].g * k;
        color.b = colors[i].b * k;

        // No remap
        //index = LP_INDEX_DATA + i * led_color_size;
        // Default remap
        //index = LP_INDEX_DATA + led_remap_default[i % LP_LEDS] * led_color_size;
        // Custom remap
        index = LP_INDEX_DATA + led_remap_custom[i % LP_LEDS] * led_color_size;

        // Send main 8 bits for compability with existing devices
        buffer_write[index++] = (color.r & 0x0FF0) >> 4;
        buffer_write[index++] = (color.g & 0x0FF0) >> 4;
        buffer_write[index++] = (color.b & 0x0FF0) >> 4;

        // Send over 4 bits for devices revision >= 6, ignored by existing devices
        buffer_write[index++] = (color.r & 0x000F);
        buffer_write[index++] = (color.g & 0x000F);
        buffer_write[index++] = (color.b & 0x000F);
    }

    Write(LPOP_LED_UPDATE);
}

void LedDriverLightpack::SetOption(const char *option, int value)
{
    int op = LPOP_NOP;

    if (strcmp(option, "refresh-delay") != 0) {
        op = LPOP_SET_TIMER_OPTIONS;
        buffer_write[LP_INDEX_DATA] = value & 0xFF;
        buffer_write[LP_INDEX_DATA + 1] = (value >> 8);
    }
    else if (strcmp(option, "color-depth") != 0) {
        op = LPOP_SET_PWM_LEVEL_MAX_VALUE;
        buffer_write[LP_INDEX_DATA] = (unsigned char)value;
    }
    else if (strcmp(option, "smooth-slowdown") != 0) {
        op = LPOP_SET_SMOOTH_SLOWDOWN;
        buffer_write[LP_INDEX_DATA] = (unsigned char)value;
    }

    if (op == LPOP_NOP)
        throw std::runtime_error("invalid option");
        
    Write(op);
}

bool LedDriverLightpack::Read(void)
{
    int bytes = hid_read(dh, buffer_read, LP_BUFFER_SIZE);
        
    if (bytes < 0) {
        std::cerr << "error reading data" << std::endl;
        return false;
    }

    return true;
}

bool LedDriverLightpack::Write(int opcode)
{
    buffer_write[LP_INDEX_RID] = 0x00;
    buffer_write[LP_INDEX_OPCODE] = (unsigned char)opcode;
    
    int rc = hid_write(dh, buffer_write, LP_BUFFER_SIZE);
    if (rc < 0) {
        // Try again...
        rc = hid_write(dh, buffer_write, LP_BUFFER_SIZE);
        if(rc < 0) {
            std::cerr << "error writing data: " << rc << std::endl;
            return false;
        }
    }

    return true;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
