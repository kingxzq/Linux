#pragma once

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cassert>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Log.hpp"

using namespace std; //不推荐

#define PATH_NAME "/home/whb"
#define PROJ_ID 0x66
#define SHM_SIZE 4096 //共享内存的大小，最好是页(PAGE: 4096)的整数倍

#define FIFO_NAME "./fifo"

class Init
{
public:
    Init()
    {
        umask(0);
        int n = mkfifo(FIFO_NAME, 0666);
        assert(n == 0);
        (void)n;
        Log("create fifo success",Notice) << "\n";
    }
    ~Init()
    {
        unlink(FIFO_NAME);
        Log("remove fifo success",Notice) << "\n";
    }
};

#define READ O_RDONLY
#define WRITE O_WRONLY

int OpenFIFO(std::string pathname, int flags)
{
    int fd = open(pathname.c_str(), flags);
    assert(fd >= 0);
    return fd;
}

void Wait(int fd)
{
    Log("等待中....", Notice) << "\n";
    uint32_t temp = 0;
    ssize_t s = read(fd, &temp, sizeof(uint32_t));
    assert(s == sizeof(uint32_t));
    (void)s;
}

void Signal(int fd)
{
    uint32_t temp = 1;
    ssize_t s = write(fd, &temp, sizeof(uint32_t));
    assert(s == sizeof(uint32_t));
    (void)s;
    Log("唤醒中....", Notice) << "\n";
}

void CloseFifo(int fd)
{
    close(fd);
}