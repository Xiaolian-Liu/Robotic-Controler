
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
		bool attrDefault;

	public:
		MessageQueue();
		MessageQueue(const string &name, int oflag, mode_t mode, mq_attr attr);
		int init();
		void setAttr(mq_attr attr);
		mq_attr getAttr();
		~MessageQueue();
};