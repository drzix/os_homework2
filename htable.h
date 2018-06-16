#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <pthread.h>
#include "list.h"

#define NR_BUCKET 10007

#define ENODATA 1

#define INSERTED 0
#define UPDATED 1

struct bucket {
        struct list_node bkt_list;
        pthread_mutex_t bkt_lock;
};

struct htable {
        struct bucket bkt[NR_BUCKET];
};

static inline void init_htable(struct htable *table)
{
        int i;
        for (i = 0; i < NR_BUCKET; i++) {
                list_init(&table->bkt[i].bkt_list);
                pthread_mutex_init(&table->bkt[i].bkt_lock, NULL); 
        }
}

typedef int key_t;
typedef int value_t;

static inline int get_hash_idx(key_t key)
{
        return key % NR_BUCKET;
}

struct item_struct {
        key_t key;
        value_t val;
        struct list_node bkt_node_entry;
        int bkt_last_pos; // updated when only searched
};

struct htable_search_result {
        int bkt_idx;
        int bkt_list_pos;
};

extern struct list_node *bucket_find_right_pos(struct bucket *, value_t);
extern int bucket_insert(struct bucket *, key_t, value_t);
extern struct item_struct *bucket_search(struct bucket *, key_t);

extern int htable_insert(struct htable *, key_t, value_t);
extern int htable_search(struct htable_search_result *, struct htable *, key_t, value_t);
extern int htable_delete(struct htable *, key_t);

static inline int htable_lock(pthread_mutex_t *mtx)
{
        return pthread_mutex_lock(mtx);
}

static inline int htable_unlock(pthread_mutex_t *mtx)
{
        return pthread_mutex_unlock(mtx);
}

#endif
