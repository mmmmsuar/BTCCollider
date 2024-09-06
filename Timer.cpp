/*
 * This file is part of the BTCCollider distribution (https://github.com/JeanLucPons/BTCCollider).
 * Copyright (c) 2020 Jean Luc PONS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Timer.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifdef WIN64
LARGE_INTEGER Timer::perfTickStart;
double Timer::perfTicksPerSec;
LARGE_INTEGER Timer::qwTicksPerSec;
#else
time_t Timer::tickStart;
#endif

void Timer::Init() {
#ifdef WIN64
    QueryPerformanceFrequency(&qwTicksPerSec);
    QueryPerformanceCounter(&perfTickStart);
    perfTicksPerSec = (double)qwTicksPerSec.QuadPart;
#else
    tickStart = time(NULL);
#endif
}

double Timer::get_tick() {
#ifdef WIN64
    LARGE_INTEGER t, dt;
    QueryPerformanceCounter(&t);
    dt.QuadPart = t.QuadPart - perfTickStart.QuadPart;
    return (double)(dt.QuadPart) / perfTicksPerSec;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(tv.tv_sec - tickStart) + (double)tv.tv_usec / 1e6;
#endif
}

std::string Timer::getSeed(int size) {
    std::string ret;
    char tmp[3];
    unsigned char *buff = (unsigned char *)malloc(size);

#ifdef WIN64
    HCRYPTPROV hCryptProv = NULL;
    LPCSTR UserName = "KeyContainer";
    CryptAcquireContext(&hCryptProv, UserName, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

    if (!CryptGenRandom(hCryptProv, size, buff)) {
        std::cerr << "CryptGenRandom() failed" << std::endl;
        exit(1);
    }
    CryptReleaseContext(hCryptProv, 0);
#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (f == NULL) {
        std::cerr << "Failed to open /dev/urandom" << std::endl;
        exit(1);
    }
    if (fread(buff, 1, size, f) != size) {
        std::cerr << "Failed to read from /dev/urandom" << std::endl;
        exit(1);
    }
    fclose(f);
#endif

    for (int i = 0; i < size; i++) {
        sprintf(tmp, "%02X", buff[i]);
        ret.append(tmp);
    }
    free(buff);
    return ret;
}

std::string Timer::getResult(char *unit, int nbTry, double t0, double t1) {
    char tmp[256];
    int pIdx = 0;
    double nbCallPerSec = (double)nbTry / (t1 - t0);
    const char *prefix[] = {"", "K", "M", "G", "T"};
    while (nbCallPerSec > 1000.0 && pIdx < 5) {
        pIdx++;
        nbCallPerSec = nbCallPerSec / 1000.0;
    }
    sprintf(tmp, "%.3f %s%s/sec", nbCallPerSec, prefix[pIdx], unit);
    return std::string(tmp);
}

void Timer::printResult(char *unit, int nbTry, double t0, double t1) {
    std::cout << getResult(unit, nbTry, t0, t1) << std::endl;
}

int Timer::getCoreNumber() {
#ifdef WIN64
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#else
    return (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

void Timer::SleepMillis(uint32_t millis) {
#ifdef WIN64
    Sleep(millis);
#else
    usleep(millis * 1000);
#endif
}
