#include "movepriorityqueue.h"
#include "move.h"

MovePriorityQueue::MovePriorityQueue() {
	backingArray = new int[INITIAL_SIZE];
	priorityArray = new short[INITIAL_SIZE];
	index = 0;
	length = INITIAL_SIZE;
}

MovePriorityQueue::MovePriorityQueue(const MovePriorityQueue& mpq) {
	backingArray = new int[mpq.length];
	priorityArray = new short[mpq.length];
	
	index = mpq.index;
	length = mpq.length;
	
	for(int i = 0; i < mpq.length; ++i) {
		backingArray[i] = mpq.backingArray[i];
		priorityArray[i] = mpq.priorityArray[i];
	}
}

MovePriorityQueue& 
MovePriorityQueue::operator =(const MovePriorityQueue& mpq) {
	if (this != &mpq) {
		int* newBackingArray = new int[mpq.length];
		short* newPriorityArray = new short[mpq.length];
		
		for(int i = 0; i < mpq.length; ++i) {
			newBackingArray[i] = mpq.backingArray[i];
			newPriorityArray[i] = mpq.priorityArray[i];
		}
		
		delete[] backingArray;
		delete[] priorityArray;
		
		backingArray = newBackingArray;
		priorityArray = newPriorityArray;
		length = mpq.length;
		index = mpq.index;
	}
	
	return *this;
}

MovePriorityQueue::~MovePriorityQueue() {
	delete[] backingArray;
	delete[] priorityArray;
}

void
MovePriorityQueue::add(int move, short priority) {
	if (index >= length)
		expandBackingArray();
	backingArray[index] = move;
	priorityArray[index] = priority;
	int currentIndex = index;
	++index;
	// bubble up
	while (currentIndex != 0) {
		int parentIndex = getParent(currentIndex);
		if (priorityArray[parentIndex] >= priorityArray[currentIndex]) {
			// we're good
			break;
		}
		int backupParent = backingArray[parentIndex];
		short backupPriority = priorityArray[parentIndex];
		
		backingArray[parentIndex] = move;
		priorityArray[parentIndex] = priority;
		backingArray[currentIndex] = backupParent;
		priorityArray[currentIndex] = backupPriority;
		currentIndex = parentIndex;
	}
}

int 
MovePriorityQueue::removeMaximum() {
	int minNode = backingArray[0];
	int last = backingArray[index - 1];
	short lastPriority = priorityArray[index - 1];
	backingArray[0] = last;
	priorityArray[0] = lastPriority;
	backingArray[index - 1] = 0;
	priorityArray[index - 1] = 0;
	
	--index;
	int currentIndex = 0;
	// current index is less than both children, great
	// otherwise swap it with one that it is less than
	while (currentIndex < index) {
		//int current = backingArray[currentIndex];
		int currentPriority = priorityArray[currentIndex];
		
		int leftChildIndex = currentIndex * 2 + 1;
		int rightChildIndex = currentIndex * 2 + 2;

		int swapIndex = -1;
		if (leftChildIndex >= index) {
			// don't need to do anything, this is the bottom of the tree
			break;
		}
		else if (rightChildIndex >= index) {
			// need to check the left child and make sure priorities are okay
			int leftPriority = priorityArray[leftChildIndex];

			if (currentPriority > leftPriority)
				break;
			swapIndex = leftChildIndex;
		}
		else {
			int leftPriority = priorityArray[leftChildIndex];
			int rightPriority = priorityArray[rightChildIndex];

			swapIndex = -1;

			if (currentPriority > leftPriority &&
					currentPriority > rightPriority) {
				// looks good
				break;
			}

			if (leftPriority > rightPriority) {
				// swap with left child
				swapIndex = leftChildIndex;
			}
			else {
				// swap with right child
				swapIndex = rightChildIndex;
			}
		}
		int temp = backingArray[swapIndex];
		short tempPriority = priorityArray[swapIndex];
		backingArray[swapIndex] = last;
		priorityArray[swapIndex] = lastPriority;
		backingArray[currentIndex] = temp;
		priorityArray[currentIndex] = tempPriority;

		currentIndex = swapIndex;
	}

	return minNode;
}

bool
MovePriorityQueue::isEmpty() {
	return (index == 0);
}

int
MovePriorityQueue::getMaximum() {
	return backingArray[0];
}

int
MovePriorityQueue::getParent(int index) {
	return (index - 1) / 2;
}

void
MovePriorityQueue::expandBackingArray() {
	int* newBackingArray = new int[length * 2];
	short* newPriorityArray = new short[length * 2];
	for(int i = 0; i < length; ++i) {
		newBackingArray[i] = backingArray[i];
		newPriorityArray[i] = priorityArray[i];
	}

	delete[] backingArray;
	delete[] priorityArray;
	
	backingArray = newBackingArray;
	priorityArray = newPriorityArray;
	length *= 2;
}

std::string 
MovePriorityQueue::toMoveString() {
	MovePriorityQueue displayQueue(*this);
	std::string returnString;

	returnString += "[";
	while(!displayQueue.isEmpty()) {
		int node = displayQueue.backingArray[0];
		returnString += " ";
		returnString += MoveToString(node);
		displayQueue.removeMaximum();
		if (!displayQueue.isEmpty())
			returnString += ",";
	}
	returnString += "]";
	
	return returnString;	
}

std::string 
MovePriorityQueue::toMoveDetailString() {
	MovePriorityQueue displayQueue(*this);
	std::string returnString;

	returnString += "[";
	while(!displayQueue.isEmpty()) {
		int node = displayQueue.backingArray[0];
		int priority = displayQueue.priorityArray[0];
		returnString += " ";
		returnString += MoveToString(node) + "(" + int_to_string(priority) + ")";
		displayQueue.removeMaximum();
	}
	returnString += "]";
	
	return returnString;
}
