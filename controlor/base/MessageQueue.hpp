
#pragma once
#include <mqueue.h>
#include <string>
using std::string;

class MessageQueue  
{
	protected:
        mqd_t mqd;
		struct mq_attr attr;
		string queueName;
		int oflag;
		mode_t mode;

	public:
		MessageQueue();
        MessageQueue(const string &name, int oflag, mode_t mode, long msgsize, long maxmsg);
        MessageQueue(const string &name);
		int init();
		void setAttr(mq_attr attr);
		mq_attr getAttr();
		~MessageQueue();
};
