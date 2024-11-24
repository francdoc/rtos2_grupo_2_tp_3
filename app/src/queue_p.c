/**
 * queue.c
 * 
 * 
 */
#include "queue_p.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

#define QUEUE_P_LENGTH 10



// Function to create a new node
node_t* new_node(int d, int p)
{
    node_t* temp = (node_t*)pvPortMalloc(sizeof(node_t));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;

    return temp;
}

// Return the value at head
int peek(queue_p_t* queue) {
    node_t *node = (node_t*)queue->head;
    return node->data; 
}

void queue_create(queue_p_t **queue) {
    if(queue) {
        *queue = pvPortMalloc(sizeof(queue_p_t));
        (*queue)->head = NULL;
        (*queue)->tail = NULL;
        (*queue)->current_length = 0;
        (*queue)->queue_mutex = xSemaphoreCreateMutex();
        // Register to logging
        vQueueAddToRegistry((*queue)->queue_mutex, "Mutex Handle");
    }
}

void queue_destroy(queue_p_t **queue) {
    if(queue && *queue) {
        node_t *next = (*queue)->head->next;
        node_t *current = (*queue)->head;

        while (!next)
        {
            vPortFree(current);
            current = next;
            next = current->next;
        }
        //TODO: mutex destroy
        vPortFree(*queue);
        *queue = NULL;
    }
}

// Removes the element with the
// highest priority form the list
void pop(queue_p_t* queue)
{
    node_t* temp = queue->head;
    (queue->head) = (queue->head)->next;
    vPortFree(temp);
}

// Function to push according to priority
void push(queue_p_t* queue, int d, int p)
{
    node_t* start = (queue->head);

    // Create new node_t
    node_t* temp = new_node(d, p);

    // Special Case: The head of list has
    // lesser priority than new node
    if ((queue->head)->priority < p) {

        // Insert New node_t before head
        temp->next = queue->head;
        (queue->head) = temp;
    }
    else {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL
               && start->next->priority > p) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check is list is empty
int is_empty(queue_p_t* queue) { return (queue->head) == NULL; }

// Driver code
/*
int main()
{

    // Create a Priority Queue
    // 7->4->5->6
    node_t* pq = new_node(4, 1);
    push(&pq, 5, 2);
    push(&pq, 6, 3);
    push(&pq, 7, 0);

    while (!is_empty(&pq)) {
        pop(&pq);
    }
    return 0;
}
*/