////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Massachusetts Institute of Technology              //
// All rights reserved.                                                   //
//                                                                        //
// Redistribution and use in source and binary forms, with or without     //
// modification, are permitted provided that the following conditions are //
// met:                                                                   //
//                                                                        //
//     * Redistributions of source code must retain the above copyright   //
//       notice, this list of conditions and the following disclaimer.    //
//                                                                        //
//     * Redistributions in binary form must reproduce the above          //
//       copyright notice, this list of conditions and the following      //
//       disclaimer in the documentation and/or other materials provided  //
//       with the distribution.                                           //
//                                                                        //
//     * Neither the name of the Massachusetts Institute of Technology    //
//       nor the names of its contributors may be used to endorse or      //
//       promote products derived from this software without specific     //
//       prior written permission.                                        //
//                                                                        //
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    //
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      //
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  //
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   //
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  //
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       //
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  //
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  //
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    //
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  //
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   //
////////////////////////////////////////////////////////////////////////////

/*
 * File:   slist.h
 * Author: Dagvadorj Galbadrakh <dagvadorj@gmail.com>
 *
 * Tip:
 * It is a linked list for arbitrary-sized elements. The user is
 * responsible for knowing the types of elements he is adding and 
 * removing. This list can be used as stack and queue, of which
 * the methods are written as wrappers.
 *
 * Created on September 27, 2010, 3:25 PM
 */

#ifndef SLIST_H
#define SLIST_H
#include <stdio.h>
#include <stdlib.h>

/*
 * Structure of a node
 * prev: points to the prevoius node
 * next: points to the next node
 * obj: points to an abstract data,
 *          thus, user is responsible for knowing
 *          the type and size of data
 */

typedef struct slist_node_s {
    struct slist_node_s * prev;
    struct slist_node_s * next;
    void * obj;
} slist_node;

/*
 * Structure of a list
 * head: points to the head node of the list
 * tail: points to the tail node of the list
 * len: length of the list
 */

typedef struct slist_s {
    slist_node * head;
    slist_node * tail;
    int len;
} slist;

/*
 * Initialize a list like:
 * slist * l = slist_init();
 */
slist * slist_init();

/*
 * Returns a node certain index of a list
 * list: points to the list
 * index: index of the node to be returned
 *          The index can be positive or negative;
 *          negative means the list will be indexed
 *          from the end (tail) of the list
 * NOTE: Indexing in the list is O(n)
 */

slist_node * slist_get(slist * list, int index);

/*
 * Adds a node to a list in a given index
 * list: points to the list
 * node: points to an abstract data,
 *          thus, user is responsible for knowing
 *          the type and size of data
 * index: index of the new node to be added
 *          The index can be positive or negative;
 *          negative means the list will be indexed
 *          from the end (tail) of the list
 */

void slist_add(slist * list, void * node, int index);

/*
 * Removes a node in a given index in a list
 * list: points to the list
 * index: index of the new node to be added
 *          The index can be positive or negative;
 *          negative means the list will be indexed
 *          from the end (tail) of the list
 */

void * slist_remove(slist * list, int index);

/*
 * Pushes the node at the end of a list (use as a stack)
 * list: points to the list
 * node: points to an abstract data,
 *          thus, user is responsible for knowing
 *          the type and size of data
 * NOTE: Pushing to the list is O(1)
 */

void slist_push(slist * list, void * node);

/*
 * Popes the node at the end of a list (use as a stack)
 * list: points to the list
 * NOTE: Popping from the list is O(1)
 */

void * slist_pop(slist * list);

/*
 * Enqueues the node at the end of a list (use as a queue)
 * list: points to the list
 * node: points to an abstract data,
 *          thus, user is responsible for knowing
 *          the type and size of data
 * NOTE: Enqueueing to the list is O(1)
 */

void slist_enqueue(slist * list, void * node);

/*
 * Dequeues the node at the beginning of a list (use as a queue)
 * list: points to the list
 * NOTE: Dequeueing from the list is O(1)
 */

void * slist_dequeue(slist * list);

/*
 * Prints the content of a list
 * list: points to the list
 * (*func) (void *): function to return the content of a node
 *          as string. User is free to write a function of
 *          his own depending on the type/size of the data
 *          in a node
 */

void slist_print(slist * list, char * (*func) (void *));

/*
 * An example function for the second parameter of the exceeding
 * function.
 * 
 * Example: slist_print(list, slist_print_str);
 * 
 */

char * slist_print_str(void * obj);

#endif  /* SLIST_H */