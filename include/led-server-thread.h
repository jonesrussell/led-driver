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

struct LedOperation
{
    int opcode;
    std::string param;
};

typedef std::queue<struct LedOperation *> led_thread_queue;

typedef std::vector<LedDriverAbstract *> led_devices;

class LedServerThread
{
public:
    LedServerThread();
    virtual ~LedServerThread();

    void Run(void);
    void Terminate(void);

    void PushOperation(int opcode);
    void PushOperation(int opcode, const std::string &param);

protected:
    void Delay(int ms);
    void OpenDevices(void);

    bool terminate;
    pthread_mutex_t *mutex;
    led_thread_queue queue;
    led_devices devices;

    std::string moodlight;
    std::string alert;
};

// vi: expandtab shiftwidth=4 softtabstop=4 tabstop=4
