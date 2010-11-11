#include "movelinkedlist.h"
#include "move.h"
#include "moveprocess.h"

MoveLinkedList::MoveLinkedList() {
	this->head = NULL;
	this->tail = NULL;
	this->length = 0;
}

MoveLinkedList::MoveLinkedList(const MoveLinkedList& moveLinkedList) {
	this->head = NULL;
	this->tail = NULL;
	this->length = 0;
	
	MoveLinkedListNode* ptr = moveLinkedList.head;
	while(ptr != NULL) {
		int moveToAdd = ptr->node;
		ptr = ptr->next;
		add(moveToAdd);
	}
}

MoveLinkedList& 
MoveLinkedList::operator=(MoveLinkedList& moveLinkedList) {
	if (this != &moveLinkedList) {
		clear();
		MoveLinkedListNode* ptr = moveLinkedList.head;
		while(ptr != NULL) {
			int moveToAdd = ptr->node;
			ptr = ptr->next;
			this->add(moveToAdd);
		}
	}
	
	return *this;
}

MoveLinkedList::~MoveLinkedList() {
	clear();
}

void 
MoveLinkedList::add(int move) {
	MoveLinkedListNode* newNode = new MoveLinkedListNode();
	newNode->node = move;
	newNode->next = NULL;
	
	if (this->head == NULL) {
		this->head = newNode;
	}
	
	if (this->tail != NULL){ 
		this->tail->next = newNode;
	}

	this->tail = newNode;
	++length;

}

void
MoveLinkedList::addAll(MoveLinkedList& other) {
	MoveLinkedListNode* ptr = other.head;
	while(ptr != NULL) {
		this->add(ptr->node);
		//cerr << "adding " << ptr->node << " from other list " << endl;
		ptr = ptr->next;
	}
}

int 
MoveLinkedList::getFirst() {
	return this->head->node;
}

void 
MoveLinkedList::clear() {
	MoveLinkedListNode* ptr = head;
	while (ptr != NULL) {
		MoveLinkedListNode* next = ptr->next;
		delete ptr;
		ptr = next;
	}
	
	this->length = 0;
	this->head = NULL;
	this->tail = NULL;
}

int 
MoveLinkedList::size() {
	return this->length;
}

std::string 
MoveLinkedList::toMoveString() {
	std::string returnString;
	MoveLinkedListNode* ptr = this->head;
	while(ptr != NULL) {
		int node = ptr->node;
		returnString += " ";
		returnString += MoveToString(node);

		ptr = ptr->next;
	}

	return returnString;
}

std::string 
MoveLinkedList::toString() {
	std::string returnString;
	returnString.append("[");
	MoveLinkedListNode* ptr = this->head;
	while(ptr != NULL) {
		int node = ptr->node;
		returnString += " ";
		returnString += int_to_string(node);

		if (ptr->next != NULL)
			returnString.append(",");
		
		ptr = ptr->next;
	}
	returnString.append(" ]");

	return returnString;
}

std::string 
MoveLinkedList::toMoveString(ChessBoard * board) {
	//std::string returnString;
	//toMoveStringHelper(board, this->head, returnString);

	return this->toMoveString();
}

// this is *very* broken
void
MoveLinkedList::toMoveStringHelper(ChessBoard * board, MoveLinkedListNode* ptr, std::string& returnString) {
	if (ptr == NULL)
		return;
	else {
		int node = ptr->node;

		returnString.append(" ");
		returnString.append(MoveToString(node));
		if (node != -1) {
			processMove(board, node);
			int gameResult = getGameResult(board);
			if (gameResult == 1 || gameResult == -1)
				returnString.append("#");
			else if (sideToMoveIsInCheck(board))
				returnString.append("+");

			toMoveStringHelper(board, ptr->next, returnString);
			unprocessMove(board, node);
		}
	}
}