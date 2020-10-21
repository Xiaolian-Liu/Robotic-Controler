#include "SharedMemory.hpp"  
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

using std::cerr;

SharedMemory::~SharedMemory()
{
    close(fd);
    sem_close(sem);
}

SharedMemory::SharedMemory(const string & name, size_t len) 
{
    this->shmName = "SharedMemory" + name;
    this->semName = "Semphore" + name;
    this->len = len;
}

int SharedMemory::init() 
{
    this->fd = shm_open(this->shmName.c_str(), O_RDWR, 0);
    this->sem = sem_open(this->semName.c_str(), 0);
    if(fd < 0 || sem == SEM_FAILED)
    {
        cerr << "shm " << shmName << " open failed\n";
        return -1;
    }

    int res = ftruncate(fd, len);
    if(-1 == res)
    {
        cerr << "shm " << shmName << " ftruncate failed\n";
        return -1;
    }
    return 0;
}

void SharedMemory::read(void *data) const
{
    sem_wait(sem);
    void *ptr;
    ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(data, ptr, len);
    sem_post(sem);
}

void SharedMemory::write(const void *data) const
{
    sem_wait(sem);
    void *ptr;
    ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(ptr, data, len);
    sem_post(sem);
}

