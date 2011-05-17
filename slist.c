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
 * File:   slist.c
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

#include "slist.h"

slist * slist_init() {
    slist * temp = malloc(sizeof(slist));
    temp->head = NULL;
    temp->tail = NULL;
    temp->len = 0;
    return temp;
}

slist_node * slist_get(slist * list, int index) {
    if (index >= list->len) {
        // fprintf(stderr, "Out of bound\n");
        return NULL;
    }
    if (index < 0 && abs(index) > list->len) {
        // fprintf(stderr, "Out of bound\n");
        return NULL;
    }
    slist_node * iter = (index >= 0) ? list->head : list->tail;
    while (index > 0) { iter = iter->next; index--; }
    while (index < -1) { iter = iter->prev; index++; }
    return iter;
}

void slist_add(slist * list, void * node, int index) {

    slist_node * temp = malloc(sizeof(slist_node));
    temp->obj = node;
    temp->prev = temp->next = NULL;

    if (list->len == 0) {
        list->head = list->tail = temp;
        list->len = 1;
        return;
    }

    slist_node * iter = slist_get(list, index);
    if (iter == NULL) return;

    if (iter->next == NULL) {
        list->tail = temp;
        iter->next = temp;
        temp->prev = iter;
    }
    else if (iter->prev == NULL) {
        list->head = temp;
        temp->next = iter;
        iter->prev = temp;
    } else {
        if (index < 0) iter = iter->next;
        iter->prev->next = temp;
        temp->prev = iter->prev;
        temp->next = iter;
        iter->prev = temp;
    }

    list->len++;
}

void * slist_remove(slist * list, int index) {

    if (list->len == 0) return NULL;

    slist_node * iter = slist_get(list, index);
    if (iter == NULL) return NULL;

    if (iter->prev == NULL) {
        list->head = iter->next;
        if (list->head != NULL)
            list->head->prev = NULL;
    } else if (iter->next == NULL) {
        list->tail = iter->prev;
        iter->prev->next = NULL;
    } else {
        iter->prev->next = iter->next;
        iter->next->prev = iter->prev;
    }

    list->len--;

    iter->prev = iter->next = NULL;
    return iter->obj;
}

void slist_push(slist * list, void * node) {
    slist_add(list, node, -1);
}

void slist_enqueue(slist * list, void * node) {
    slist_add(list, node, -1);
}

void * slist_pop(slist * list) {
    return slist_remove(list, -1);
}

void * slist_dequeue(slist * list) {
    return slist_remove(list, 0);
}

void slist_print(slist * list, char * (*func) (void *)) {
    slist_node * temp = list->head;
    printf("<");
    while (temp != NULL) {
        printf("%s ", func(temp->obj));
        temp = temp->next;
    }
    printf(">\n");
}

char * slist_print_str(void * obj) {
    return (char *) obj;
}
