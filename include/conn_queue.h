#ifndef CONN_QUEUE_H
#define CONN_QUEUE_H

#include <pthread.h>
#include <stddef.h>

// Represents an accepted client connection
typedef struct {
    int file_descriptor;
    unsigned long connection_id;
} connection_t;

// Synchronized thread-safe bounded queue for connections (Producer-Consumer)
typedef struct {
    connection_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    int shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} conn_queue_t;

/**
 * Initializes the connection queue with a given capacity.
 * Returns 0 on success, -1 on failure.
 */
int conn_queue_init(conn_queue_t *q, size_t capacity);

/**
 * Releases memory and destroys synchronization primitives.
 */
void conn_queue_destroy(conn_queue_t *q);

/**
 * Pushes a connection into the queue (Producer).
 * Blocks if the queue is full.
 * Returns 0 on success, -1 if the queue is shutting down.
 */
int conn_queue_push(conn_queue_t *q, connection_t conn);

/**
 * Pops a connection from the queue (Consumer).
 * Blocks if the queue is empty.
 * Returns 0 on success, -1 if the queue is empty and shutting down.
 */
int conn_queue_pop(conn_queue_t *q, connection_t *out_conn);

/**
 * Signals the queue to wake up all blocked threads and shut down.
 */
void conn_queue_shutdown(conn_queue_t *q);

/**
 * Returns the current number of elements in the queue.
 */
size_t conn_queue_size(conn_queue_t *q);

#endif // CONN_QUEUE_H
