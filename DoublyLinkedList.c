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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "DoublyLinkedList.h"
#include "Commands_aux.h"

#include "Game.h"
#include "Error_Print.h"


NODE *head = NULL, *tail = NULL;

/* moves back or forth in  linked list
 * @param direction: whether to go back (-1) or forth (1)
 * @param NODE* current_head: the head of the list
 * @param board: the game board
 * @param fixed_board: the board with fixed values
 * @params ROWS,COLS: board dimensions
 * @param state: the game state.
 * @return the updated current_head */
NODE* move_back_or_forth(int direction,NODE* current_head,int* board,
		int* fixed_board,int ROWS,
		int COLS, GAME_STATE state){

	NODE* current = current_head;

	if (current->moves_count==-1){
		if(direction==-1){
			printf("Nothing to undo! try to make a move first! \n");
			return current;
		}
	}

	if(direction==-1){
		undo_redo_moves(direction, (current)->moves, (current)->moves_count, board, fixed_board, ROWS, COLS, state);
		return(current)->prev;
	}

	if((current)->next != NULL){
		if (direction==1){
			undo_redo_moves(direction, (current)->next->moves, (current)->next->moves_count, board, fixed_board, ROWS, COLS, state);

			return(current)->next;

		}
	}
	else{
		printf("Nothing to redo! try to undo a move first! \n");
	}
	return current;
}


/*
 * NODE* create_list()-  creates a new list. returns a pointer to a "fake" first node which is marked by move_count ==-1
 * no @params
 * @return a pointer to the fake first node.
 * */
NODE* create_list(){
	NODE* new_head;
	new_head = (NODE *) malloc(sizeof (NODE));

	if (new_head!=NULL) {
		new_head->moves = NULL;
		new_head->moves_count = -1;
		new_head->next = NULL;
		new_head->prev = NULL;
	}
	else {
		printFunctionError("malloc");
	}

	head = new_head;
	return new_head;

}

/*
 *  add_after(NODE* current, MOVE* moves, int moves_count) - a function that adds a new NODE with the given moves.
 *  @param current : the current node to add the new node after.
 *  @param after_moves: the moves to add to the new node.
 *  @param after_moves_count: the number of moves to add.
 *  @return a pointer to the new node created.
 */
NODE* add_after(NODE* current, MOVE* after_moves, int after_moves_count)
{
	NODE* after;

	after = (NODE *) malloc(sizeof (NODE));

	if (after!=NULL) {
		after->moves = after_moves;
		after->moves_count = after_moves_count;
		after->next = NULL;
		after->prev = current;
	} else {
		printf("unable to allocate memory \n");
		return NULL;
	}

	if((current)->next!=NULL){
		delete_all_after(&current);
	}
	(current)->next = after;
	return after;
}


/*void delete_all_after(NODE** node) - deletes all the nodes after the current node, not inclusive.
 * @param node: the pointer to the current node, to delete all whats after it.
 * @return void */
void delete_all_after(NODE** node){

	   NODE* current = (*node)->next;
	   NODE* next;

	if (current == NULL) {
		return;
	}
	(*node)->next = NULL;

	while (current !=NULL) {
		next = (current)->next;
		free(current->moves);
		free(current);
		current = next;
	}
}


/*NODE* delete_list(NODE** redo_list_head) - deletes the entire list and takes care of memory freeing.

 * @param node: the pointer to a node, if its the head or not, to delete all whats after it.
 * @return pointer to the head of the list (the fake first node) */

NODE* delete_list(NODE** head_cur)
{
	NODE* tmp_ptr = *head_cur;

	/* set tmp_ptr to the FAKE node */
	while ((tmp_ptr)->moves_count!= -1) {
		(tmp_ptr) = (tmp_ptr)->prev;
	}

	delete_all_after(&tmp_ptr);

	return (tmp_ptr);
}
