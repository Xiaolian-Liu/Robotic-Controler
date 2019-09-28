#include "SeqQueue.h"
#include <stdio.h>

template <typename T>
SeqQueue<T>::SeqQueue(int n)
{
	data = new T[n];
	front = 0;
	rear = 0;
	capacity = n;
}

template <typename T>
SeqQueue<T>::~SeqQueue()
{
	delete[]data;
}

template <typename T>
bool SeqQueue<T>::isEmpty()
{
	if (front == rear)
		return true;
	else
		return false;
}

template <typename T>
bool SeqQueue<T>::isFull()
{
	if ((rear + 1) % capacity == front)
		return true;
	else
	{
		return false;
	}
}

template <typename T>
int SeqQueue<T>::size()
{
	return((rear - front + capacity) % capacity);
}

template <typename T>
bool SeqQueue<T>::In(T d)
{
	if ((rear + 1) % capacity == front) {
		printf("the Sequence Queue is full\n");
		return false;
	}
	else {
		data[rear] = d;
		rear = (rear + 1) % capacity;
		return true;
	}
}

template <typename T>
bool SeqQueue<T>::Out(T &d)
{
	if (rear == front) {
		printf("the Sequence Queue is empty\n");
		return false;
	}
	else {
		front = (front + 1) % capacity;
		d = data[front];
		return true;
	}
}
/*
template <typename T>
bool SeqQueue<T>::Out(T *d)
{
	if (rear == front) {
		printf("the Sequence Queue is empty\n");
		return false;
	}
	else {
		front = (front + 1) % capacity;
		*d = data[front];
		return true;
	}
}
*/
