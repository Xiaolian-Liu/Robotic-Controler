#ifndef SEQQUEUE_H
#define SEQQUEUE_H

template <typename T>
class SeqQueue
{
public:
	SeqQueue(int n = 1024);
	~SeqQueue();
	bool isEmpty();
	bool isFull();
	int size();
	bool In(T d);
	bool Out(T &d);
//	bool Out(T *d);

private:
	T *data;
	int front;
	int rear;
	int capacity;

};

#endif