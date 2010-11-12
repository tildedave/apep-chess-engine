#ifndef MOVELINKEDLIST_H_
#define MOVELINKEDLIST_H_

#include <string>
#include "board.h"

struct MoveLinkedListNode {
	int node;
	MoveLinkedListNode* next;
};

class MoveLinkedList {
	
public:
	
	struct Iterator {
		MoveLinkedListNode* currentNode;
		
		Iterator(const MoveLinkedList& mll) {
			this->currentNode = mll.head;
		}
		
	public: 
		
		bool atEnd() {
			return (currentNode == NULL);
		}
		
		int currentMove() {
			return currentNode->node;
		}
		
		void nextMove() {
			currentNode = currentNode->next;
		}
		
	};
	
	MoveLinkedList();
	MoveLinkedList(const MoveLinkedList& moveLinkedList);
	MoveLinkedList& operator=(MoveLinkedList& moveLinkedList);
	~MoveLinkedList();
	
	void add(int move);
	void addAll(MoveLinkedList& move);
	void clear();
	int size();
	int getFirst();
	std::string toMoveString();
	std::string toMoveString(ChessBoard * board);
	std::string toString();
	
	Iterator iterator() {
		return Iterator(*this);
	}
	
protected:
	
	MoveLinkedListNode* head;
	MoveLinkedListNode* tail;
	int length;
	
private: 
	
	void toMoveStringHelper(ChessBoard * board, MoveLinkedListNode* ptr, std::string& returnString);
};

#endif /*MOVELINKEDLIST_H_*/
