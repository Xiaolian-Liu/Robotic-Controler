
#pragma once
#include <string>
#include <sys/mman.h>
#include <semaphore.h>

using std::string;

class SharedMemory  
{
	protected:
		int fd;
		size_t len;
		sem_t *sem;
		string shmName;
		string semName;
		SharedMemory(const string & name, size_t len);

	public:
		int init();
		void read(void *data) const;
		void write(const void *data) const;
		virtual ~SharedMemory();
};