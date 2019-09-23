/*
 * DoublyLinkedList module summary:
 * contains the neccesary functions for handling the Doubly Linked List class - which is used to store the user's move.
 * The list is a list of NODES. each NODE contains an array of MOVES. each MOVE contains information about the move (indices, before and after values).
 *
 * NODE* create_list()-  creates a new list. returns a pointer to a "fake" first node which is marked by move_count ==-1
NODE* move_back_or_forth(int direction,NODE* current_head,int* board, int* fixed_board,int ROWS,  int COLS, GAME_STATE state)- a function to traverse the list back or forth.
NODE* add_after(NODE* current, MOVE* moves, int moves_count) - a function that adds a new NODE with the given moves.
void delete_all_after(NODE** node) - deletes all the nodes after the current node, not inclusive.
NODE* delete_list(NODE** redo_list_head) - deletes the entire list and takes care of memory freeing.
 *
 */

#ifndef DOUBLYLINKEDLIST_H_
#define DOUBLYLINKEDLIST_H_

#include "Game.h"
typedef struct move{
	int x,y, before, after;
	struct move* prev_move;
	struct move* next_move;
}MOVE;

typedef struct node {
	MOVE* moves;
	int moves_count;
	struct node *prev;
	struct node *next;
}NODE;



NODE* create_list();

NODE* move_back_or_forth(int direction,NODE* current_head,int* board,
		int* fixed_board,int ROWS,
		int COLS, GAME_STATE state);
NODE* add_after(NODE* current, MOVE* moves, int moves_count);
void delete_all_after(NODE** node);
NODE* delete_list(NODE** redo_list_head);

int length();
void display(NODE* head);


#endif /* DOUBLYLINKEDLIST_H_ */
