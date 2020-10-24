
#pragma once
#include "base/MessageQueue.hpp"
#include "base/Time.hpp"
struct incPos_t
{
	int32_t targetPosition[6];
};

class PositionQueue : public MessageQueue
{
	private:

	public:
        PositionQueue();
        PositionQueue(int flag);
        int getPosition(incPos_t * pos);
        int getPosition(incPos_t * pos, Time timeOut);
		int sendPosition(incPos_t pos);
		int sendPosition(incPos_t pos, Time timeOut);
		~PositionQueue();
};
