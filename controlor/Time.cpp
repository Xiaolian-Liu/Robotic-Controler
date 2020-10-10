#include "Time.hpp"  

Time::Time()
{
    this->tv_sec = 0;
    this->tv_nsec = 0;
}

Time::Time(__time_t sec, __syscall_slong_t nsec)
{
    this->tv_sec = sec;
    this->tv_nsec = nsec;
}

Time::Time(uint64_t nsec) 
{
    this->tv_sec = nsec / NANO_PER_SEC;
    this->tv_nsec = nsec % NANO_PER_SEC;
}

Time::Time(const timespec & t)
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
    Time diff;
    int64_t tot1, tot2;
    tot1 = t.tv_nsec + t.tv_sec * NANO_PER_SEC;
    tot2 = this->tv_nsec + this->tv_sec * NANO_PER_SEC;
    diff.tv_nsec = (tot2 - tot1) % NANO_PER_SEC;
    diff.tv_sec = (tot2 - tot1) / NANO_PER_SEC;
    return diff;
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

Time& Time::operator-=(const Time &t) 
{
    int64_t tot1, tot2;
    tot1 = t.tv_nsec + t.tv_sec * NANO_PER_SEC;
    tot2 = this->tv_nsec + this->tv_sec * NANO_PER_SEC;
    this->tv_nsec = (tot2 - tot1) % NANO_PER_SEC;
    this->tv_sec = (tot2 - tot1) / NANO_PER_SEC;
    return *this;
}

uint64_t Time::totalNanoSec() 
{
    return tv_sec * NANO_PER_SEC + tv_nsec;
}

std::ostream& operator<<(std::ostream &os, const Time t) 
{
    // os << t.tv_sec << "\tseconds\t" << t.tv_nsec << "\t\tnanoSecs\t" << t.tv_sec * Time::NANO_PER_SEC + t.tv_nsec << "\t\ttotal nanoSecs";
    os << t.tv_sec << "\t\t" << t.tv_nsec << "\t\t" << t.tv_sec * Time::NANO_PER_SEC + t.tv_nsec << "\t\t";
    return os;
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

int32_t Time::diffNanoSec(const Time &t1, const Time &t2) 
{
    return (t1.tv_nsec - t2.tv_nsec) + (t1.tv_sec - t2.tv_sec) * NANO_PER_SEC;
}

