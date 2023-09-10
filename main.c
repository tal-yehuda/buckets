#include <stdio.h>
#include <stdlib.h>

// You can choose the number of buckets - don't forget to specify capacity.
#define buckets_number 2

const int capacity[buckets_number] = {3, 5};

typedef enum {
    Start, Fill, Empty, Pour,
} command;

struct Node {
    int parent_index;
    int level;
    int bucket_state[buckets_number];
    int target;
    int source;
    command action;
};

struct Node *nodes = NULL;
int len = 0;
int cap = 0;

void add(struct Node node) {
    if (len == cap) {
        cap = (cap + 1) * 2;
        struct Node *temp = (struct Node *) realloc(nodes, cap * sizeof(struct Node));
        if (temp == NULL) exit(1);
        nodes = temp;
    }
    nodes[len++] = node;
}

struct Node fill(struct Node node, int target) {
    node.action = Fill;
    node.target = target;
    node.source = 0;
    node.bucket_state[target] = capacity[target];
    return node;
}

struct Node empty(struct Node node, int target) {
    node.action = Empty;
    node.target = target;
    node.source = 0;
    node.bucket_state[target] = 0;
    return node;
}

struct Node pour(struct Node node, int target, int source) {
    node.action = Pour;
    node.target = target;
    node.source = source;

    int source_state = node.bucket_state[source];
    int target_state = node.bucket_state[target];
    int left = capacity[target] - target_state;

    if (source_state >= left) {
        node.bucket_state[source] -= left;
        node.bucket_state[target] += left;
    } else {
        node.bucket_state[target] += source_state;
        node.bucket_state[source] -= source_state;
    }
    return node;
}

void fill_step(struct Node node) {
    for (int i = 0; i < buckets_number; i++) {
        if (node.bucket_state[i] == capacity[i]) continue;
        add(fill(node, i));
    }
}

void empty_step(struct Node node) {
    for (int i = 0; i < buckets_number; i++) {
        if (node.bucket_state[i] == 0) continue;
        add(empty(node, i));
    }
}

void pour_step(struct Node node) {
    for (int s = 0; s < buckets_number; s++) {
        for (int t = 0; t < buckets_number; t++) {
            if (s == t || node.bucket_state[s] == 0 || node.bucket_state[t] == capacity[t]) continue;
            add(pour(node, t, s));
        }
    }
}

void step(int i) {
    struct Node node = nodes[i];
    node.parent_index = i;
    node.level += 1;

    fill_step(node);
    empty_step(node);
    pour_step(node);
}

int same_buckets(struct Node n, struct Node m) {
    for (int i = 0; i < buckets_number; i++)
        if (n.bucket_state[i] != m.bucket_state[i]) return 0;
    return 1;
}

int is_new(int i) {
    for (int j = 0; j < i; j++) {
        struct Node a = nodes[i];
        struct Node b = nodes[j];
        if (same_buckets(a, b) && a.action == b.action && a.target == b.target && a.source == b.source)
            return 0;
    }
    return 1;
}

int run_level(int level) {
    int run = 0;
    for (int i = 0; i < len; i++) {
        if (nodes[i].level == level && is_new(i)) {
            step(i);
            run += 1;
        }
    }
    return run;
}

void search(void) {
    int level = 0;
    while (run_level(level)) level += 1;
}


void print_action(command a) {
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
    }
}

int check_buckets(struct Node node, int w) {
    for (int n = 0; n < buckets_number; n++)
        if (node.bucket_state[n] == w) return 1;
    return 0;
}

void print_buckets(struct Node node) {
    printf("[ ");
    for (int n = 0; n < buckets_number; n++) printf("%d ", node.bucket_state[n]);
    printf("]");
}

void print_node(struct Node node) {
    print_buckets(node);
    putchar('-');
    print_action(node.action);
    putchar('-');
    printf("(%d %d)", node.target, node.source);
}

int highest(int i, int w) {
    while (nodes[i].action != Start) {
        i = nodes[i].parent_index;
        if (check_buckets(nodes[i], w)) return 0;
    }
    return 1;
}

void print_path(int i){
    while (nodes[i].action != Start) {
        print_node(nodes[i]);
        printf(" <- ");
        i = nodes[i].parent_index;
    }
    print_node(nodes[i]);
}

void print_path_to_bucket(int w) {
    for (int i = 0; i < len; i++) {
        struct Node node = nodes[i];
        if (check_buckets(node, w) && highest(i, w)) {
            print_path(i);
            putchar('\n');
        }
    }
}

void print_nodes(void) {
    for (int i = 0; i < len; i++) {
        print_node(nodes[i]);
        putchar('\n');
    }
}

// You can seed an initial state of buckets.
void seed(void) {
    struct Node n = {
            .bucket_state = {0, 0},
            .action = Start,
    };
    add(n);
}


int main(void) {
    seed();
    search();
    print_path_to_bucket(4);
    print_nodes();
    return 0;
}