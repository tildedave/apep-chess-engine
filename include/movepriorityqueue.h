#ifndef MOVEPRIORITYQUEUE_H_
#define MOVEPRIORITYQUEUE_H_

#include <string>

class MovePriorityQueue {
	static const int INITIAL_SIZE = 32;

public:
	MovePriorityQueue();
	MovePriorityQueue(const MovePriorityQueue& mpq);
	MovePriorityQueue& operator=(const MovePriorityQueue& mpq);
	~MovePriorityQueue();
	
	void add(int move, short priority);
	int removeMaximum();
	bool isEmpty();
	int getMaximum();

	std::string toMoveString();
	std::string toMoveDetailString();
		
protected: 
	int* backingArray;
	short* priorityArray;
	int index;
	int length;

	
	int getParent(int index); 
	void expandBackingArray();	
};

#endif /*MOVEPRIORITYQUEUE_H_*/
