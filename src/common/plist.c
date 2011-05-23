/* plist.c - A generic doubly linked list implementation
 *
 * Copyright (c) 2011, Hu Jiakuan <ahqmhjk@gmail.com> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include "plist.h"

struct _plistnode {
    struct _plistnode *prev;
    struct _plistnode *next;
    void *value;
};

struct _plistiter {
    pListNode *next;
    int direction;
};

typedef struct _plist {
    PListNode *head;
    pListNode *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned int len;
};

pList *listCreate(void) {
	pList *list;
	if ((list = malloc(sizeof(pList))) == NULL) 
		return NULL;
	list->head = list->tail = NULL;
	list->len = 0;
	list->dup = NULL;
	list->free = NULL;
	list->match = NULL;
	
	return list;
}
void pListRelease(pList *list) {
	if (list == NULL)
		return;
	unsigned int len;
	pListNode *current, *next;
    current = list->head;
	len = list->len;
	while (len--) {
		next = current->next;
		if (current->free) current->free(current->value);
		free(current);
		current = next;
	}
	free(list);
	current = next = NULL;
}

pList *pListAddNodeHead(pList *list, void *value) {
	pListNode *node = (pListNode*)malloc(sizeof(pListNode));
	node->value = value;
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
	}
	else {
		node->prev = NULL;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
	++(list->len);
	return list;
}

pList *listAddNodeTail(pList *list, void *value) {
	pListNode *node = (pListNode*)malloc(sizeof(pListNode));
	node->value = value;
	if (list->len == 0) {
		list->head = list->tail = node;
		node->prev = node->next = NULL;
	}
	else {
		node->next = NULL;
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}
	++(list->len);
	return list;
}

// Insert a node
pList *listInsertNode(pList *list, pListNode *old_node, void *value) {
	if (list == NULL) return NULL;
	pListNode *current = list->head;
	while (current && current != old_node) current = current->next;
	if (current == NULL) {// Not Found, So insert behind the tail.
		return listAddNodeTail(list, value);
	}
	else {
		pListNode *node = NULL;
		if ((node = (pListNode*)malloc(sizeof(pListNode))) == NULL)
			return NULL;
		node->value = value;
		node->prev = current;
		node->next = current->next;
		if (current->node != NULL)
			current->next->prev = node;
		current->next = node;
		++(list->len);
		return list;
	}
}

pList *listInsertNode(pList *list, int direction, void *value) {
	if (list == NULL) return NULL;
	unsigned int mid = list->len/2;
	if (direction < mid) {
		if (direction < 0) return listAddNodeHead(list, value);
		
	}
	else {
		
	}
}

// Delete a node
void listDelNode(pList *list, pListNode *node);
void listDelNode(pList *list, void* data);
void listDelNode(pList *list, int direction);

#define AL_START_HEAD 0
#define AL_START_TAIL 1
pListIter *listGetIterator(pList *list, int direction) {
   	pListIter *iter;
    
    if ((iter = zmalloc(sizeof(*iter))) == NULL) return NULL;
    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;
    return iter;
}

pListNode *listNext(pListIter *iter) {
   	pListNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}

void listReleaseIterator(pListIter *iter) {
	free(iter);
}

pList *listDup(pList *orig) {
    pList *copy;
    pListIter *iter;
    pListNode *node;

    if ((copy = listCreate()) == NULL)
        return NULL;
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    iter = listGetIterator(orig, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
        void *value;

        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRelease(copy);
                listReleaseIterator(iter);
                return NULL;
            }
        } else
            value = node->value;
        if (listAddNodeTail(copy, value) == NULL) {
            listRelease(copy);
            listReleaseIterator(iter);
            return NULL;
        }
    }
    listReleaseIterator(iter);
    return copy;
}

pListNode *listSearchKey(pList *list, void *key) {
   	pListIter *iter;
    pListNode *node;

    iter = listGetIterator(list, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, key)) {
                listReleaseIterator(iter);
                return node;
            }
        } else {
            if (key == node->value) {
                listReleaseIterator(iter);
                return node;
            }
        }
    }
    listReleaseIterator(iter);
    return NULL;
}

pListNode *listIndex(pList *list, int index) {
   	pListNode *n;

    if (index < 0) {
        index = (-index)-1;
        n = list->tail;
        while(index-- && n) n = n->prev;
    } else {
        n = list->head;
        while(index-- && n) n = n->next;
    }
    return n;
}

void listRewind(PList *list, pListIter *li) {
	li->next = list->head;
    li->direction = AL_START_HEAD;
}
void listRewindTail(pList *list, pListIter *li) {
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}

