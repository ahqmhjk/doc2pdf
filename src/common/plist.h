/* plist.h - A generic doubly linked list implementation
 *
 * Copyright (c) 2011, Hu Jiakuan <ahqmhjk@gmail.com>
 * All rights reserved.
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


#ifndef _PLIST_H
#define _PLIST_H 1


struct _plistnode;
typedef struct _plistnode pListNode;

struct _plist;
typedef struct _plist pList;

struct _plistiter;
typedef struct _plistiter pListIter;

#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
pList *listCreate(void);
void pListRelease(pList *list);
pList *pListAddNodeHead(pList *list, void *value);
pList *listAddNodeTail(pList *list, void *value);

pList *listInsertNode(pList *list, pListNode *old_node, void *value);
pList *listInsertNode(pList *list, int direction, void *value);

void listDelNode(pList *list, pListNode *node);
void listDelNode(pList *list, void* data);
void listDelNode(pList *list, int direction);


pListIter *listGetIterator(pList *list, int direction);
pListNode *listNext(pListIter *iter);
void listReleaseIterator(pListIter *iter);
pList *listDup(pList *orig);
pListNode *listSearchKey(pList *list, void *key);
pListNode *listIndex(pList *list, int index);
void listRewind(PList *list, pListIter *li);
void listRewindTail(pList *list, pListIter *li);

#endif
