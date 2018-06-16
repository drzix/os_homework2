#include <stdlib.h>
#include "htable.h"

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
