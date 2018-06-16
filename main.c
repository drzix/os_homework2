#include <stdio.h>
#include <time.h>

#include "htable.h"

#define NR_THREAD 4

struct thread_args {
        struct htable *table;
        int fileno;
};

static double TimeSpecToSeconds(struct timespec *ts);
static void read_input_into_htable(struct htable *);
static void *_read_input_into_htable(void *);
static void do_command(struct htable *);

int main(int argc, char **argv)
{
        struct htable my_ht;
        init_htable(&my_ht);

        read_input_into_htable(&my_ht);
        do_command(&my_ht);

        return 0;
}

static double TimeSpecToSeconds(struct timespec *ts)
{
        return (double)ts->tv_sec + (double)ts->tv_nsec / (double)1e9;
}

static void read_input_into_htable(struct htable *table)
{
        int i;
        pthread_t th[NR_THREAD];
        struct thread_args th_arg[NR_THREAD];
        void *th_ret;
        struct timespec begin, end;
        double elapsedSec;

        clock_gettime(CLOCK_MONOTONIC, &begin);

        for (i = 0; i < NR_THREAD; i++) {
                th_arg[i].table = table;
                th_arg[i].fileno = i;
                pthread_create(th + i, NULL, &_read_input_into_htable, th_arg + i);
        }

        for (i = 0; i < NR_THREAD; i++)
                pthread_join(th[i], &th_ret);
                
        clock_gettime(CLOCK_MONOTONIC, &end);

        elapsedSec = TimeSpecToSeconds(&end) - TimeSpecToSeconds(&begin);

        printf("%lf\n", elapsedSec);
}

static void *_read_input_into_htable(void *t_args)
{
        char path[20];
        FILE *f_ptr;
        key_t key;
        value_t val;
        struct thread_args *arg = t_args;

        sprintf(path, "input%d.txt", arg->fileno);
        f_ptr = fopen(path, "r");

        while (!feof(f_ptr)) {
                fscanf(f_ptr, "%d %d", &key, &val);
                htable_insert(arg->table, key, val);
        }

        return NULL;
}

static void do_command(struct htable *table)
{
        int foo, bar;
        struct htable_search_result result;
        
        while (1) {
                scanf("%d", &foo);

                switch (foo) {
                case 1:
                        scanf("%d %d", &foo, &bar);
                        foo = htable_insert(table, foo, bar);
                        printf("%d\n", foo);
                        break;
                case 2:
                        scanf("%d %d", &foo, &bar);
                        if (!htable_search(&result, table, foo, bar))
                                printf("%d %d\n", result.bkt_idx, result.bkt_list_pos);
                        else
                                printf("-1\n");
                        break;
                case 3:
                        scanf("%d", &foo);
                        foo = htable_delete(table, foo);
                        printf("%d\n", foo);
                        break;
                default:
                        return;
                }
        }
}
