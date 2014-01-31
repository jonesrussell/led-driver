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

#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>

#include <sys/socket.h>

#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "hidapi.h"

#include "lib-socket.h"

#include "ld-color.h"
#include "ld-abstract.h"
#include "ld-lightpack.h"

#include "led-server-thread.h"

LedServerThread::LedServerThread()
    : terminate(false)
{
    int rc;
    mutex = new pthread_mutex_t;
    if ((rc = pthread_mutex_init(mutex, NULL)) != 0) {
        delete mutex;
        std::cerr << __func__ << ": pthread_mutex_init: "
            << strerror(rc) << std::endl;
        throw std::runtime_error("pthread_mutex_init");
    }
}

LedServerThread::~LedServerThread()
{
    pthread_mutex_destroy(mutex);
    while (queue.size() > 0) {
        delete queue.front();
        queue.pop();
    }
    for (led_devices::iterator i = devices.begin(); i != devices.end(); i++)
        delete (*i);
}

void LedServerThread::PushOperation(int opcode)
{
    pthread_mutex_lock(mutex);
    if (devices.size()) {
        struct LedOperation *op = new struct LedOperation;
        op->opcode = opcode;
        queue.push(op);
    }
    pthread_mutex_unlock(mutex);
}

void LedServerThread::PushOperation(int opcode, const std::string &param)
{
    pthread_mutex_lock(mutex);
    if (devices.size()) {
        struct LedOperation *op = new struct LedOperation;
        op->opcode = opcode;
        op->param = param;
        queue.push(op);
    }
    pthread_mutex_unlock(mutex);
}

void LedServerThread::Delay(int ms)
{
    for (int i = ms; i > 0 && !terminate; i -= 100)
        usleep(100 * 1000);
    if (terminate) pthread_exit(NULL);
}

void LedServerThread::OpenDevices(void)
{
    LedDriverLightpack *lightpack = new LedDriverLightpack;
    try {
        lightpack->Open();
        lightpack->SetOption("refresh-delay", 100);
        lightpack->SetOption("color-depth", 128);
        lightpack->SetOption("smooth-slowdown", 255);

        devices.push_back(lightpack);
    }
    catch (std::runtime_error &e) {
        delete lightpack;
        std::cerr << __func__ << ": Lightpack: " << e.what() << std::endl;
    }
}

void LedServerThread::Run(void)
{
    LedColors colors(LP_LEDS);
    struct LedOperation *op = NULL;

    for (int i = 0; !terminate; i++) {
        if (devices.size() == 0) {
            OpenDevices();
            if (devices.size() == 0) {
                Delay(1000);
                continue;
            }
        }

        pthread_mutex_lock(mutex);
        if (queue.size() > 0) {
            std::cerr << __func__ << ": popped operation" << std::endl;
            op = queue.front();
            queue.pop();
        }
        pthread_mutex_unlock(mutex);

        if (op != NULL) {
            switch (op->opcode) {
            case LSOP_SET_LEDS_OFF:
                devices[0]->SetLedsOff();
                moodlight.clear();
                break;
            case LSOP_SET_MOODLIGHT:
                moodlight = "default";
                break;
            default:
                std::cerr << __func__ << ": unsupported opcode: "
                    << op->opcode << std::endl;
                break;
            }

            delete op;
            op = NULL;
        }

        if (moodlight != "default") {
            std::cerr << __func__ << ": no moodlight set" << std::endl;
            Delay(1000);
            continue;
        }

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

        devices[0]->SetLedColors(colors);
        Delay(1500);
    }
}

void LedServerThread::Terminate(void)
{
    terminate = true;
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
