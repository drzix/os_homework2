#include <stdlib.h>
#include "htable.h"

/**
* bucket_find_right_pos - val 보다 작은 값 중 가장 큰 노드를 리턴
*
* @bkt: 리스트가 있는 버킷
* @val: 기준이 되는 value
*/
struct list_node *bucket_find_right_pos(struct bucket *bkt, value_t val)
{
        struct item_struct *now;
        struct list_node *ret = &bkt->bkt_list;

        for_each_list_node(bkt->bkt_list) {
                now = container_of(node, struct item_struct, bkt_node_entry);

                if (now->val < val)
                        ret = node;
                else
                        break;
        }

        return ret;
}

/**
* bucket_insert - 해당 버킷 리스트에 새 데이터 삽입 혹은 갱신
*
* @bkt: 리스트가 있는 버킷
* @key: 검색할 key
* @val: value
*
* 기존 key가 있으면 갱신 후 적절한 위치에 다시 삽입
* 새로운 key라면 새로 할당 후 적절한 위치에 삽입
*/
int bucket_insert(struct bucket *bkt, key_t key, value_t val)
{
        int code;
        struct item_struct *found;
        struct list_node *prev;

        found = bucket_search(bkt, key);

        if (found) {
                found->val = val;
                list_delete(&found->bkt_node_entry);
                code = UPDATED;
        } else {
                found = malloc(sizeof(struct item_struct));
                found->key = key;
                found->val = val;
                code = INSERTED;
        }
        
        prev = bucket_find_right_pos(bkt, val);
        list_insert(&found->bkt_node_entry, prev);

        return code;
}

/**
* htable_insert - 해시테이블에 키 밸류 삽입 혹은 갱신
*
* key에 해당하는 버킷을 검색 후 해당 버킷리스트의 상호배제에 따라
* 삽입 혹은 갱신
*/
int htable_insert(struct htable *table, key_t key, value_t val)
{
        int code;
        int idx = get_hash_idx(key);
        struct bucket *target_bkt = &table->bkt[idx];

        htable_lock(&target_bkt->bkt_lock);
        code = bucket_insert(target_bkt, key, val);
        htable_unlock(&target_bkt->bkt_lock);

        return code;
}

/**
* bucket_search - 버킷에 key가 일치하는 데이터가 있나 확인
*
* 순회를 하면서 pos를 증가시켜 리스트의 어느 위치에 있는지
* item_struct의 bkt_last_pos에 기록한다
* item_struct의 포인터를 리턴하지만 리스트에서의 위치도 알 수 있기 위함임
*/
struct item_struct *bucket_search(struct bucket *bkt, key_t key)
{
        struct item_struct *ret;
        int pos = 0;

        for_each_list_node(bkt->bkt_list) {
                ret = container_of(node, struct item_struct, bkt_node_entry);

                if (ret->key == key) {
                        ret->bkt_last_pos = pos;
                        goto out;
                }

                pos++;
        }

        ret = NULL;
out:
        return ret;
}

/**
* htable_search - key, val를 이용하여 해시테이블을 검색 후 res에 검색 결과를 기록
*
* @res: 버킷 인덱스와 데이터의 위치 검색 결과를 기록할 객체
*/
int htable_search(struct htable_search_result *res, struct htable *table, key_t key, value_t val)
{
        int err;
        struct item_struct *found;

        res->bkt_idx = get_hash_idx(key);
        found = bucket_search(&table->bkt[res->bkt_idx], key);

        if (found && found->val == val) {
                res->bkt_list_pos = found->bkt_last_pos;
                err = 0;
        } else {
                err = -ENODATA;
        }

        return err;
}

/**
* htable_delete - key에 해당하는 데이터를 해시테이블에서 삭제
*/
int htable_delete(struct htable *table, key_t key)
{
        struct item_struct *found;
        int err;
        int idx = get_hash_idx(key);

        found = bucket_search(&table->bkt[idx], key);

        if (found) {
                list_delete(&found->bkt_node_entry);
                free(found);
                err = 0;
        } else {
                err = -ENODATA;
        }

        return err;
}
