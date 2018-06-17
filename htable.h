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

/**
* init_htable - 해시테이블 초기화
*
* 버킷마다의 리스트를 초기화해주고
* 버킷마다의 뮤텍스를 초기화해준다
*/
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

        /* 버킷 리스트 노드  */
        struct list_node bkt_node_entry;

        /* 검색되는 시점에 버킷리스트에서의 위치 */
        int bkt_last_pos;
};

struct htable_search_result {
        /* 검색된 데이터의 버킷 인덱스 */
        int bkt_idx;

        /* 검색된 데이터의 버킷리스트에서의 위치 */
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
