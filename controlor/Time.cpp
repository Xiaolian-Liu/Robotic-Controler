#include "Time.hpp"  

Time::Time()
{
    this->tv_sec = 0;
    this->tv_nsec = 0;
}

Time::Time(__time_t sec, __syscall_slong_t nsec) 
{
    this->tv_sec = sec;
    this->tv_nsec;
}

Time::Time(uint64_t nsec) 
{
    this->tv_sec = nsec / NANO_PER_SEC;
    this->tv_nsec = nsec % NANO_PER_SEC;
}

Time::Time(timespec t) 
{
    this->tv_sec = t.tv_sec;
    this->tv_nsec = t.tv_nsec;
}

void Time::addSec(__time_t second) 
{
    this->tv_sec += second;
}

void Time::addNanoSec(__syscall_slong_t nanoSecond) 
{
    this->tv_nsec += nanoSecond;
    this->tv_sec += this->tv_nsec / NANO_PER_SEC;
    this->tv_nsec %= NANO_PER_SEC;
}

Time Time::operator-(const Time &t) const
{
    Time sum;
    sum.tv_nsec = this->tv_nsec - t.tv_nsec;
    sum.tv_sec = this->tv_sec - t.tv_sec + sum.tv_nsec / NANO_PER_SEC;
    sum.tv_nsec %= NANO_PER_SEC;
}

Time Time::operator+(const Time &t) const
{
    Time sum;
    sum.tv_nsec = this->tv_nsec + t.tv_nsec;
    sum.tv_sec = this->tv_sec + t.tv_sec + sum.tv_nsec / NANO_PER_SEC;
    sum.tv_nsec %= NANO_PER_SEC;
    return sum;
}

Time &Time::operator=(const Time &t)
{
    if(this != &t)
    {
        this->tv_sec = t.tv_sec;
        this->tv_nsec = t.tv_nsec;
    }
    return *this;
}

uint64_t Time::totalNanoSec() 
{
    return tv_sec * NANO_PER_SEC + tv_nsec;
}

Time& Time::operator+=(const Time &t) 
{
    this->tv_nsec += t.tv_nsec;
    this->tv_sec += t.tv_sec + this->tv_nsec / NANO_PER_SEC;
    this->tv_nsec %= NANO_PER_SEC;
    return *this;
}

Time::~Time()
{

}