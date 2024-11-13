#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

// Binary tree node
typedef struct node {
    int data;
    struct node *left;
    struct node *right;
    struct node *below_left;
    struct node *below_right;
} node;

typedef struct tree {
    node *root;
    int levels; // Number of levels in the tree
} tree;

tree *create_tree() {
    tree *new_tree = (tree *)malloc(sizeof(tree));
    assert(new_tree != NULL);
    new_tree->root = NULL;
    new_tree->levels = 0;
    return new_tree;
}

node *create_node(int data, node *nleft, node *nright) {
    node *new_node = (node *)malloc(sizeof(node));
    assert(new_node != NULL);
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->below_left = NULL;
    new_node->below_right = NULL;
    return new_node;
}

int countlines(FILE *fp) {
    int count = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    return count;
}

int* parseline(char *line, int size) {
    int *data = (int *)malloc(size * sizeof(int));
    assert(data != NULL);
    int i = 0;
    char *token = strtok(line, " ");
    while (token != NULL && i < size) {
        data[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
    }
    return data;
}

void insert_from_file(tree *t, FILE *file) {
    int nb_levels = countlines(file);
    rewind(file);
    
    // Calculate max line length:
    // (level × 3 digits per number) + (level-1 spaces) + 2 line ending chars + 1 null
    size_t max_line_len = (nb_levels * 3) + (nb_levels - 1) + 3;
    char *line = malloc(max_line_len);
    if (!line) {
        fprintf(stderr, "Failed to allocate line buffer\n");
        return;
    }

    // Read the first line and create the root
    if (fgets(line, max_line_len, file) != NULL) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;
        int *data = parseline(line, 1);
        t->root = create_node(data[0], NULL, NULL);
        t->levels++;
        // Check if the root was created successfully
        assert(t->root != NULL);
        free(data);
    } else {
        fprintf(stderr, "Error reading the first line\n");
        free(line); // Free the buffer before returning
        return;
    }
    // Define a current node
    node *current = t->root;
    // Read the rest of the file and insert the nodes line by line
    while (fgets(line, max_line_len, file) && t->levels <= nb_levels) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;
        // Parse the line
        int *data = parseline(line, t->levels + 1);
        // Initialize the first node.
        node *nfirst = create_node(data[0], NULL, NULL);
        // Array to store the nodes in the new level.
        node *nodes[t->levels + 1];
        nodes[0] = nfirst;
        // Create all the nodes in the current and link them to each other with right and left.
        for (int i = 1; i < t->levels + 1; i++) {
            node *nnew = create_node(data[i], nodes[i - 1], NULL);
            nnew->left = nodes[i - 1];
            nodes[i - 1]->right = nnew;
            nodes[i] = nnew;
        }
        // Now, we iterate of the neighbours of the current node to insert the nodes of the new level.
        int counter = 0;
        while (current != NULL) {
            current->below_left = nodes[counter];
            current->below_right = nodes[counter + 1];
            current = current->right;
            counter++;
        }
        // Set current to the first node of the new level.
        current = nfirst;

        // Free the allocated memory for data
        free(data);

        t->levels++;
    }

    free(line); // Don't forget to free the buffer
}

void print_tree(tree *t) {
    if (t == NULL || t->root == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    node *current = t->root;
    printf("Root: %d\n", current->data);
    current = current->below_left;

    while (current != NULL) {
        printf("Current node: %d\n", current->data);

        if (current->left != NULL) {
            printf("\tLeft: %d\n", current->left->data);
        } else {
            printf("\tLeft: NULL\n");
        }

        if (current->right != NULL) {
            printf("\tRight: %d\n", current->right->data);
        } else {
            printf("\tRight: NULL\n");
        }

        if (current->below_left != NULL) {
            printf("\tBelow left: %d\n", current->below_left->data);
        } else {
            printf("\tBelow left: NULL\n");
        }

        if (current->below_right != NULL) {
            printf("\tBelow right: %d\n", current->below_right->data);
        } else {
            printf("\tBelow right: NULL\n");
        }

        if (current->right == NULL) {
            while (current->left != NULL) {
                current = current->left;
            }
            current = current->below_left;
        } else {
            current = current->right;
        }
    }
}

void delete_tree(tree *t) {
    assert(t != NULL);
    if (t->root == NULL) {
        free(t);
        return;
    }

    node *current = t->root;
    node *next_level = current->below_left;

    while (current != NULL) {
        node *temp = current;
        while (temp != NULL) {
            node *next = temp->right;
            free(temp);
            temp = next;
        }
        current = next_level;
        if (current != NULL) {
            next_level = current->below_left;
        }
    }

    free(t);
}

void print_tree_like_file(tree *t) {
    if (t == NULL || t->root == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    node *current = t->root;
    printf("%d\n", current->data);
    current = current->below_left;

    while (current != NULL) {
        printf("%d", current->data);

        while (current->right != NULL) {
            printf(" %d", current->right->data);
            current = current->right;
        }

        printf("\n");

        // Set current to the most left node of the current level
        while (current->left != NULL) {
            current = current->left;
        }

        if (current->below_left != NULL) {
            current = current->below_left;
        } else {
            break;
        }
    }
}

/// Solve the problem

// Hash table structure for memoization
typedef struct {
    node *node_ptr;
    int sum;
} cache_entry;

typedef struct {
    cache_entry *entries;
    int size;
    int count;
} cache;

// Create and initialize cache
cache* create_cache(int size) {
    cache *c = malloc(sizeof(cache));
    c->entries = calloc(size, sizeof(cache_entry));
    c->size = size;
    c->count = 0;
    return c;
}

// Hash function for node pointers
int hash(node *n, int size) {
    return ((uintptr_t)n) % size;
}

// Get cached value
int get_cached(cache *c, node *n) {
    int h = hash(n, c->size);
    while (c->entries[h].node_ptr != NULL) {
        if (c->entries[h].node_ptr == n) {
            return c->entries[h].sum;
        }
        h = (h + 1) % c->size;
    }
    return -1;
}

// Store value in cache
void store_cache(cache *c, node *n, int sum) {
    if (c->count >= c->size * 0.75) return; // Avoid overflow
    
    int h = hash(n, c->size);
    while (c->entries[h].node_ptr != NULL) {
        h = (h + 1) % c->size;
    }
    c->entries[h].node_ptr = n;
    c->entries[h].sum = sum;
    c->count++;
}

// Modified max_path_sum with memoization
int max_path_sum_memo(node *current, cache *c) {
    if (current == NULL) return 0;
    
    // Check cache first
    int cached = get_cached(c, current);
    if (cached != -1) return cached;
    
    // Leaf node
    if (current->below_left == NULL && current->below_right == NULL) {
        store_cache(c, current, current->data);
        return current->data;
    }
    
    int left_sum = max_path_sum_memo(current->below_left, c);
    int right_sum = max_path_sum_memo(current->below_right, c);
    
    int result = current->data + (left_sum > right_sum ? left_sum : right_sum);
    store_cache(c, current, result);
    return result;
}

// Modified solve function
int solve(tree *t) {
    if (t == NULL || t->root == NULL) return 0;
    
    cache *c = create_cache(t->levels * t->levels); // Adjust size based on tree
    int result = max_path_sum_memo(t->root, c);
    free(c->entries);
    free(c);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    tree *t = create_tree();
    insert_from_file(t, file);
    // print_tree(t);
    print_tree_like_file(t);
    int result = solve(t);
    printf("Result: %d\n", result);
    delete_tree(t);

    fclose(file);
    return 0;
}