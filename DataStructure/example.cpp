#include "SeqQueue.h"
#include <iostream>
using std::cout;
using std::endl;

int main(void)
{
	SeqQueue<short> targetposition(6);
	targetposition.In(1);
	targetposition.In(2);
	targetposition.In(3);
	targetposition.In(4);
	cout << "size:" << targetposition.size() << endl;
	if (targetposition.isFull())
		cout << "the targetposition is full\n";
	short target;
	for (int i = 0; i < 6; i++)
	{
		if (targetposition.Out(target))
		{
			printf("the queue out succeed!\n");
		}
		else
		{
			printf("the queue out failed!\n");
		}
		cout << target << endl;
	}
	targetposition.~SeqQueue();
	return 0;
}
