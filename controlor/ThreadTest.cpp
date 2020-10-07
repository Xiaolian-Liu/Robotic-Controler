#include "ThreadTest.hpp"  
#include <iostream>
#include <unistd.h>
using std::cout;
using std::endl;

ThreadTest::ThreadTest() :Thread("test")
{

}

ThreadTest::~ThreadTest()
{

}

void ThreadTest::run() 
{
    while(isRun)
    {
        cout << "TestRunFunc\n";
        sleep(1);
    }
}
