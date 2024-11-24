/**
 * queue_p.h
 * 
 */

#ifndef QUEUE_P_H
#define QUEUE_P_H

// Node
typedef struct node {
    int data;

    // Lower values indicate
    // higher priority
    int priority;

    struct node* next;

} node_t;

typedef struct queue_p
{
    node_t *head;
    node_t *tail;
    uint32_t current_length;
    SemaphoreHandle_t queue_mutex;
} queue_p_t;



#endif // QUEUE_P_H
