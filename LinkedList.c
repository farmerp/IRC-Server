
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"

//
// Initialize a linked list
//
void llist_init(LinkedList * list)
{
	list->head = NULL;
}

//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void llist_print(LinkedList * list) {
	
	ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return;
	}

	printf("{");

	e = list->head;
	while (e != NULL) {
		printf("%d", e->value);
		e = e->next;
		if (e!=NULL) {
			printf(", ");
		}
	}
	printf("}\n");
}

//
// Appends a new node with this value at the beginning of the list
//
void llist_add(LinkedList * list, int value) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

//
// Returns true if the value exists in the list.
//
int llist_exists(LinkedList * list, int value) {
	ListNode * e; 
	e = list->head;
	while(e != NULL)
	{
		if (e->value == value) {
			return 1;
		}
		e= e->next;
	}
	return 0;
}

//
// It removes the entry with that value in the list.
//
int llist_remove(LinkedList * list, int value) {
	ListNode * e;
	ListNode * ln;
	e = list->head;
	ln = e->next;
	while(ln->next != NULL) {
		if(ln->value == value) {
			e->next = ln->next;
			free(ln);
			return 1;
		}
		e = e->next;
		ln= ln->next;
	}
	return 0;
}

//
// It stores in *value the value that correspond to the ith entry.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_get_ith(LinkedList * list, int ith, int * value) {
	int i = 0;
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if(i == ith) {
			*value = e->value;
			return 1;
		}
		i++;
		e = e->next;
	}
	return 0;
}

//
// It removes the ith entry from the list.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_remove_ith(LinkedList * list, int ith) {
	int value = 0;
	if(llist_get_ith(list, ith, &value) != 0) {
		llist_remove(list,value);
		return 1;
	}
	return 0;
}

//
// It returns the number of elements in the list.
//
int llist_number_elements(LinkedList * list) {
	ListNode * e;
	int counter = 0;
	e = list->head;
	while(e != NULL) {
		e = e->next;
		counter++;
	}
	return counter;
}

	//
// It saves the list in a file called file_name. The format of the
// file is as follows:
//
// value1\n
// value2\n
// ...
//
int llist_save(LinkedList * list, char * file_name) {
	ListNode * e;
	FILE * file = fopen(file_name, "w");
	if (file == NULL) {
		return 0;
	}
	e = list->head;
	while(e != NULL) {
		fprintf(file,"%d\n", e->value);
		e = e->next;
	}

	if (fclose(file)) {
		return 1;
	}
	return 0;
}

//
// It reads the list from the file_name indicated. If the list already has entries, 
// it will clear the entries.
//
int llist_read(LinkedList * list, char * file_name) {
	FILE *fl = fopen(file_name, "r");
	ListNode * e;
	int a = 0;
	llist_init(list);
	if(fl == NULL) {
		return 0;
	}
	while (fscanf(fl,"%d", &a)!=EOF) {
		llist_add(list,a);
	}
	fclose(fl);
	return 1;
}


//
// It sorts the list. The parameter ascending determines if the
// order si ascending (1) or descending(0).
//
void llist_sort(LinkedList * list, int ascending) {

}

//
// It removes the first entry in the list and puts value in *value.
// It also frees memory allocated for the node
//
int llist_remove_first(LinkedList * list, int * value) {
	ListNode * e;
	e = list->head;
	if (e!= NULL) {
		*value = e->value;
		list->head = e->next;
		free(e);
		return 1;
	}
	return 0;
}

//
// It removes the last entry in the list and puts value in *value/
// It also frees memory allocated for node.
//
int llist_remove_last(LinkedList * list, int *value) {
  	ListNode * e;
	ListNode * ln;
	if (list->head ==NULL) {
		return 0;
	} else {
		e = list->head;
	}
	while (e != NULL) {
		if (e->next != NULL) {
			ln = e;
			e = e->next;
		} else {
			break;
		}
	}
	if (e != NULL) {
		*value = e->value;
		ln->next = NULL;
		free(ln->next);
		return 1;
	}
	return 0;
}

//
// Insert a value at the beginning of the list.
// There is no check if the value exists. The entry is added
// at the beginning of the list.
//
void llist_insert_first(LinkedList * list, int value) {
	ListNode * e;
	llist_add(list, value);
}

//
// Insert a value at the end of the list.
// There is no check if the name already exists. The entry is added
// at the end of the list.
//
void llist_insert_last(LinkedList * list, int value) {
	ListNode * e;
	ListNode * f=(ListNode *) malloc(sizeof(ListNode));
	e = list->head;
	while (e != NULL) {
		if (e->next != NULL) {
			e = e->next;
		}
		else {
			break;
		}
	}

	f->value = value;
	f->next = NULL;
	if (e != NULL) {
		e->next = f;
	} else {
		list->head = f;
	}
}

//
// Clear all elements in the list and free the nodes
//
void llist_clear(LinkedList *list)
{
	list->head = NULL;
	free(list->head);
}
