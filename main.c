#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define buckets_number 2
#define c_list Start, Fill, Empty, Pour,

typedef enum {
    c_list
} command;

const int buckets[buckets_number] = {5, 3};

void print_nodes(void);

typedef struct {
    command c;
    int target;
    int source;
} action;


struct Node {
    int parent_index;
    int level;
    int buckets_state[buckets_number];
    action action;
};

struct Node *nodes = NULL;


int node_cursor = 0;
int max = 0;

int add(struct Node node);

int uniq_state(struct Node node);

int add(struct Node node) {
    if (!uniq_state(node)) {
        return 0;
    }
    if (max == 0) {

        max = 10;
        nodes = (struct Node *) malloc(max * sizeof(struct Node));
    } else if (node_cursor == max) {
        max *= 2;
        struct Node *temp = (struct Node *) realloc(nodes, max * sizeof(struct Node));
        if (temp == NULL) {
            fprintf(stderr, "reallocation failed\n");
            exit(1);
        }
        nodes = temp;
    }

    nodes[node_cursor++] = node;
    return 1;
}

int uniq_state(struct Node node) {
    int ok = 1;
    struct Node t;
    for (int i = 0; i < node_cursor; i++) {
        ok = 0;
        for (int j = 0; j < buckets_number; j++) {
            t = nodes[i];
            action a = t.action;
            action na = node.action;
            if (t.buckets_state[j] != node.buckets_state[j]
                || (a.c != na.c && a.target != na.target && a.source != na.source)
                    ) {
                ok = 1;
            }
        }
        if (ok == 0) {
            break;
        }
    }
    return ok;
}

struct Node get(int index);

struct Node get(int index) {
    if (index < node_cursor)
        return nodes[index];
    fprintf(stderr, "index out of bound\n");
    exit(1);
}

struct Node run(struct Node node);

struct Node run(struct Node node) {
    switch (node.action.c) {
        case Start:
            break;
        case Fill:
            node.buckets_state[node.action.target] = buckets[node.action.target];
            break;
        case Empty:
            node.buckets_state[node.action.target] = 0;
            break;
        case Pour: {
            int *src = &node.buckets_state[node.action.source];
            int *trg = &node.buckets_state[node.action.target];
            int left = buckets[node.action.target] - *trg;
            if (*src >= left) {
                *src -= left;
                *trg += left;
            } else {
                *trg += *src;
                *src -= *src;
            }
        }
            break;
    }
    return node;
}

int step(struct Node node, int node_index);

int step(struct Node node, int node_index) {
    // signal that a new level was found
    int ok = 0;
    int temp_ok;

    // next level and parent index
    node.level += 1;
    node.parent_index = node_index;

    // Fill all the buckets that are not full
    node.action.c = Fill;
    for (int i = 0; i < buckets_number; i++) {
        if (node.buckets_state[i] == buckets[i]) {
            continue;
        }
        node.action.target = i;
        temp_ok = add(run(node));
        if (temp_ok == 1) {
            ok = 1;
        }
    }

    // Empty all the buckets that are not empty
    node.action.c = Empty;
    for (int i = 0; i < buckets_number; i++) {
        if (node.buckets_state[i] == 0) {
            continue;
        }
        node.action.target = i;
        temp_ok = add(run(node));
        if (temp_ok == 1) {
            ok = 1;
        }
    }

    // Pour all the buckets that are not empty to the buckets that are not full
    node.action.c = Pour;
    for (int i = 0; i < buckets_number; i++) {
        for (int j = 0; j < buckets_number; j++) {
            if (i == j || node.buckets_state[i] == 0 || node.buckets_state[j] == buckets[j]) {
                continue;
            }
            node.action.source = i;
            node.action.target = j;
            temp_ok = add(run(node));
            if (temp_ok == 1) {
                ok = 1;
            }
        }
    }
    return ok;
}

int search(struct Node node) {
    int current_level = node.level;
    // should continue search (are there more levels to explore)?
    int ok = 1;
    while (ok) {
        ok = 0;
        int r = node_cursor;
        for (int i = 0; i < r; i++) {
            struct Node n = get(i);
            if (n.level != current_level) continue;
            int temp_ok = step(n, i);
            if (temp_ok) {
                ok = 1;
            }

        }
        current_level += 1;
        if (current_level > 10000) break;
    }
    return current_level;
}

void print_nodes(void) {
    struct Node n;
    for (int i = 0; i < node_cursor; i++) {
        n = nodes[i];
//        if (n.action.c != Pour) continue;
        printf("a: %d  l: %d  i: %d  s: [", n.action.c, n.level, n.parent_index);
        for (int j = 0; j < buckets_number; j++) {
            printf("%d,", n.buckets_state[j]);
        }
        printf("]\t");
    }
    printf("\n");
}

void print_action(int a) {
    switch (a) {
        case Start:
            printf("Start");
            break;
        case Fill:
            printf("Fill");
            break;
        case Empty:
            printf("Empty");
            break;
        case Pour:
            printf("Pour");
            break;
        default:
            fprintf(stderr, "No %d action found", a);
            exit(1);
    }
}

//void print_path_to_state(struct Node n) {
//    struct Node t;
//    for (int i = 0; i < node_cursor; i++) {
//        t = get(i);
//        int ok = 0;
//        for (int j = 0; j < buckets_number; j++) {
//            if (n.buckets_state[j] != t.buckets_state[j]) {
//                ok = 1;
//            }
//        }
//        ok = !ok;
//        while (ok) {
//            printf("[%d,%d] ", t.buckets_state[0], t.buckets_state[1]);
//            print_action(t.action.c);
//            if (t.action.c == Start) ok = 0;
//            printf(" <- ");
//            t = get(t.parent_index);
//        }
//    }
//    putchar('\n');
//}

void print_path_to_bucket(int l) {
    struct Node t;
    for (int i = 0; i < node_cursor; i++) {
        t = get(i);
        int ok = 0;
        for (int j = 0; j < buckets_number; j++) {
            if (t.buckets_state[j] == l) {
                ok = 1;
            }
        }

        if (t.action.c != Pour) continue;

        while (ok) {
            printf("[");
            for (int j = 0; j < buckets_number; j++) printf("%d,", t.buckets_state[j]);
            printf("] ");
            print_action(t.action.c);
            if (t.action.c == Start) {
                putchar('\n');
                break;
            }
            printf(" <- ");
            t = get(t.parent_index);
            for (int j = 0; j < buckets_number; j++) {
                if (t.buckets_state[j] == l) {
                    printf(" XXX \n");
                    ok = 0;
                }
            }
        }
    }
}

int main(void) {
    struct Node n = {
            .level = 0,
            .action = {
                    .c = Start,
            },
    };
    for (int i = 0; i < buckets_number; i++) n.buckets_state[i] = 0;
    add(n);
    printf("%d \n", search(n)-1);
    print_nodes();
    print_path_to_bucket(4);
    printf("%d\n", node_cursor);
    return 0;
}


