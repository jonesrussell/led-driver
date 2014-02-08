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
#include <queue>

#include <sys/socket.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include "hidapi.h"

#include "lib-socket.h"

#include "ld-color.h"
#include "ld-abstract.h"
#include "ld-lightpack.h"

#include "led-server-thread.h"

static void *led_thread_main(void *param)
{
    LedServerThread *led_server = (LedServerThread *)param;
    led_server->Run();
    return NULL;
}

static void process_request(libSocketServer &skt_server, LedServerThread &led_thread)
{
    int i;
    struct libSocketPacket pkt; 
    ssize_t bytes = skt_server.Read((unsigned char *)&pkt, LSPKT_MAX_LENGTH);
    if (bytes == LSPKT_MAX_LENGTH) {
        switch (pkt.opcode) {
        case LSOP_SET_LEDS_OFF:
            std::cerr << "LEDs off." << std::endl;
            pkt.opcode = LSOP_OK;
            led_thread.PushOperation(LSOP_SET_LEDS_OFF);
            break;
        case LSOP_SET_ALERT:
            std::cerr << "Set alert: " << (const char *)pkt.data << std::endl;
            pkt.opcode = LSOP_OK;
            led_thread.PushOperation(LSOP_SET_ALERT, (const char *)pkt.data);
            break;
        case LSOP_GET_ALERTS:
            std::cerr << "Get alerts." << std::endl;
            for (int i = 0; i < 5; i++) {
                sprintf((char *)pkt.data, "Alert #%d", i + 1);
                bytes = skt_server.Write((unsigned char *)&pkt, LSPKT_MAX_LENGTH);
                if (bytes != LSPKT_MAX_LENGTH) break;
            }
            pkt.opcode = LSOP_EOF;
            break;
        case LSOP_SET_MOODLIGHT:
            std::cerr << "Set moodlight: " << (const char *)pkt.data << std::endl;
            pkt.opcode = LSOP_OK;
            led_thread.PushOperation(LSOP_SET_MOODLIGHT, (const char *)pkt.data);
            break;
        case LSOP_GET_MOODLIGHTS:
            std::cerr << "Get moodlights." << std::endl;
            for (int i = 0; i < 5; i++) {
                sprintf((char *)pkt.data, "Moodlight #%d", i + 1);
                bytes = skt_server.Write((unsigned char *)&pkt, LSPKT_MAX_LENGTH);
                if (bytes != LSPKT_MAX_LENGTH) break;
            }
            pkt.opcode = LSOP_EOF;
            break;
        default:
            pkt.opcode = LSOP_ERROR;
            std::cerr << "Invalid opcode." << std::endl;
        }
    }
    else if (bytes == -1) return;

    bytes = skt_server.Write((unsigned char *)&pkt, LSPKT_MAX_LENGTH);
}

static void usage(int rc)
{
    std::cerr << "led-server <options>" << std::endl;
    std::cerr << "  -s,--service <service>" << std::endl;
    std::cerr << "    Specify service name/number (default: 3500)." << std::endl;
    std::cerr << "  -d,--debug" << std::endl;
    std::cerr << "    Enable debug mode." << std::endl;
    std::cerr << "  -v,--version" << std::endl;
    std::cerr << "    Display version and exit." << std::endl;

    exit(rc);
}

int main(int argc, char *argv[])
{
    int rc, debug = 0;
    sigset_t signal_set;
    const char *service = "3500";

    static struct option options[] =
    {
        { "service", required_argument, 0, 's' },
        { "debug", no_argument, 0, 'd' },
        { "version", no_argument, 0, 'V' },
        { "help", no_argument, 0, 'h' },

        { NULL, 0, 0, 0 }
    };

    for (optind = 1;; ) {
        int o = 0;
        if ((rc = getopt_long(argc, argv,
            "s:dVh?", options, &o)) == -1) break;
        switch (rc) {
        case 's':
            service = optarg;
            break;
        case 'd':
            debug = 1;
            break;
        case 'V':
            break;
        case '?':
            std::cerr << "Try --help for more information." << std::endl;
            return 1;
        case 'h':
            usage(0);
        }
    }

    if (!debug) {
        daemon(0, 0);

        FILE *h_pid = fopen("/var/run/led-driver/led-server.pid", "w+");
        if (h_pid != NULL) {
            fprintf(h_pid, "%d\n", getpid());
            fclose(h_pid);
        }
    }

    pthread_t thread_id;
    LedServerThread led_thread;

    if ((rc = pthread_create(&thread_id, NULL,
        &led_thread_main, (void *)&led_thread)) != 0) {
        std::cerr << "Error creating LED thread." << std::endl;
        return 1;
    }

    libSocketServer skt_server(service);
    skt_server.SetReadTimeout(1);

    sigfillset(&signal_set);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);

    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGPROF);
    pthread_sigmask(SIG_UNBLOCK, &signal_set, NULL);

    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGHUP);
    sigaddset(&signal_set, SIGTERM);
    sigaddset(&signal_set, SIGPIPE);
    sigaddset(&signal_set, SIGCHLD);
    sigaddset(&signal_set, SIGALRM);
    sigaddset(&signal_set, SIGUSR1);
    sigaddset(&signal_set, SIGUSR2);
    sigaddset(&signal_set, SIGIO);

    rc = 0;
    for (int run = 1; run != 0; ) {
        siginfo_t si;
        int sn = sigwaitinfo(&signal_set, &si);

        if (sn < 0) {
            rc = 1;
            run = 0;
            std::cerr << "sigwaitinfo: " << strerror(errno) << std::endl;
        }
        else {
            switch (sn) {
            case SIGINT:
            case SIGHUP:
            case SIGTERM:
                run = 0;
                led_thread.Terminate();
                break;

            case SIGIO:
                process_request(skt_server, led_thread);
                break;

            default:
                rc = 1;
                run = 0;
                led_thread.Terminate();
                std::cerr << "received signal: " << strsignal(sn) << std::endl;
            }
        }
    }

    return pthread_join(thread_id, NULL);
}

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
