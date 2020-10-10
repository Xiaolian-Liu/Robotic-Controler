#ifndef __TIME_H__
#define __TIME_H__

#include <time.h>
#include <stdint.h>
#include <iostream>

class Time : public timespec
{
	// private:
	// 	__time_t tv_sec;
	// 	__syscall_slong_t tv_nsec;

	public:
        static const uint32_t NANO_PER_SEC = 1000000000;
		static int32_t diffNanoSec(const Time &t1, const Time &t2);
		Time();
		Time(__time_t sec, __syscall_slong_t nsec);
		Time(uint64_t nsec);
		Time(const timespec & t);
		void addSec(__time_t second);
		void addNanoSec(__syscall_slong_t nanoSecond);
		Time operator+(const Time &t) const;
		Time operator-(const Time &t) const;
		Time &operator=(const Time &t);
		Time &operator+=(const Time &t);
		Time &operator-=(const Time &t);
		uint64_t totalNanoSec();
		friend std::ostream &operator<<(std::ostream &os, const Time t);
		~Time();
};
#endif // __TIME_H__
