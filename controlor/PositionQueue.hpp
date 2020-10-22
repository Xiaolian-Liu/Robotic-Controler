
#pragma once
#include "MessageQueue.hpp"
#include "Time.hpp"
struct incPos_t
{
	int32_t targetPosition[6];
};

class PositionQueue : public MessageQueue
{
	private:

	public:

		PositionQueue();
		incPos_t getPosition();
		int sendPosition(incPos_t pos);
		int sendPosition(incPos_t pos, Time timeOut);
		~PositionQueue();
};