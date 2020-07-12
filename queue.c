#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#include "strnatcmp.h"

#define MIN(a, b)     \
    {                 \
        a < b ? a : b \
    }

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;

    while (q->head) {
        list_ele_t *curh = q->head;
        q->head = q->head->next;
        free(curh->value);
        free(curh);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    newh->value = malloc(strlen(s) + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }

    strncpy(newh->value, s, strlen(s) + 1);
    newh->next = q->head;
    if (!q->tail)
        q->tail = newh;
    q->head = newh;
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    newh->next = NULL;

    newh->value = malloc(strlen(s) + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }

    strncpy(newh->value, s, strlen(s) + 1);
    q->tail->next = newh;
    q->tail = newh;
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    if (sp) {
        strncpy(sp, q->head->value, bufsize);
        // Concatenate terminating character if bufsize is not eqaul to the
        // length of q->head->value
        sp[bufsize - 1] = '\0';
    }

    list_ele_t *oldh = q->head;
    q->head = q->head->next;
    if (!q->head)
        q->tail = NULL;
    free(oldh->value);
    free(oldh);
    q->size -= 1;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q || !q->head)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t *slow = q->head;
    list_ele_t *fast = q->head->next;
    list_ele_t *faster;

    q->tail = slow;
    slow->next = NULL;
    while (fast) {
        faster = fast->next;
        fast->next = slow;
        slow = fast;
        fast = faster;
    }
    q->head = slow;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t *head = NULL;
    list_ele_t **tmp = &head;

    while (l1 && l2) {
        if (strnatcmp(l1->value, l2->value) < 0) {
            *tmp = l1;
            l1 = l1->next;
        } else {
            *tmp = l2;
            l2 = l2->next;
        }
        tmp = &((*tmp)->next);
    }

    if (l1) {
        *tmp = l1;
    }
    if (l2) {
        *tmp = l2;
    }
    return head;
}

list_ele_t *merge_sort_list(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    // merge sort
    list_ele_t *slow = head;
    list_ele_t *fast = head->next;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = merge_sort_list(head);
    list_ele_t *l2 = merge_sort_list(fast);

    // merge sorted l1 and l2
    return merge(l1, l2);
}

void q_sort(queue_t *q)
{
    if (!q || !q->head || !q->head->next)
        return;

    q->head = merge_sort_list(q->head);
    // reset q->tail
    list_ele_t *curh = q->head;
    while (curh->next) {
        curh = curh->next;
    }
    q->tail = curh;
}
