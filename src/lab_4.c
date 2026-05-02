#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define T 2
#define MAX_SIZE (2 * T - 1)


typedef struct Node {
    char keys[MAX_SIZE][7];
    double value[MAX_SIZE];
    struct Node *children[MAX_SIZE + 1];
    int keys_cnt;
    int is_leaf;
} Node;

typedef struct BTree {
    Node *root;
} BTree;



Node *create_node(const int leaf) {
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (new_node == NULL) return NULL;
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < 7; j++) {
            new_node->keys[i][j] = '\0';
        }
        new_node->value[i] = 0.0;
    }
    for (int i = 0; i <= MAX_SIZE; i++) {
        new_node->children[i] = NULL;
    }
    new_node->keys_cnt = 0;
    new_node->is_leaf = leaf;
    return new_node;
}

int split_child(Node *root, int index, Node *old_root) {
    Node *new_child = create_node(old_root->is_leaf);
    if (new_child == NULL) return -1;

    new_child->keys_cnt = T - 1;

    for (int i = 0; i < T - 1; i++) {
        strncpy(new_child->keys[i], old_root->keys[i + T], 6);
        new_child->keys[i][6] = '\0';
        new_child->value[i] = old_root->value[i + T];
    }

    if (!old_root->is_leaf) {
        for (int i = 0; i < T; i++) {
            new_child->children[i] = old_root->children[i + T];
            old_root->children[i + T] = NULL;
        }
    }
    old_root->keys_cnt = T - 1;

    for (int j = root->keys_cnt; j >= index + 1; j--) {
        root->children[j + 1] = root->children[j];
    }
    root->children[index + 1] = new_child;

    for (int j = root->keys_cnt - 1; j >= index; j--) {
        strncpy(root->keys[j + 1], root->keys[j], 6);
        root->keys[j + 1][6] = '\0';
        root->value[j + 1] = root->value[j];
    }

    strncpy(root->keys[index], old_root->keys[1], 6);
    root->keys[index][6] = '\0';
    root->value[index] = old_root->value[1];
    root->keys_cnt++;
    return 0;
}
int delete_node(Node *root, const char *key);
int insert_non_full(Node *root, const char *key, double value) {
    if (root == NULL) return -1;
    int i = root->keys_cnt - 1;

    if (root->is_leaf) {
        while (i >= 0 && strcmp(root->keys[i], key) > 0) {
            strncpy(root->keys[i + 1], root->keys[i], 6);
            root->keys[i + 1][6] = '\0';
            root->value[i + 1] = root->value[i];
            i--;
        }
        strncpy(root->keys[i + 1], key, 6);
        root->keys[i + 1][6] = '\0';
        root->value[i + 1] = value;
        root->keys_cnt++;
    } else {
        while (i >= 0 && strcmp(root->keys[i], key) > 0) {
            i--;
        }
        i++;
        if (root->children[i]->keys_cnt >= MAX_SIZE) {
            split_child(root, i, root->children[i]);
            if (strcmp(root->keys[i], key) < 0) i++;
        }
        insert_non_full(root->children[i], key, value);
    }
    return 0;
}

int insert_node(Node **root, const char *key, const double value) {
    if (*root == NULL) {
        *root = create_node(1);
        if (*root == NULL) return -1;

        int i = (*root)->keys_cnt;
        strncpy((*root)->keys[i], key, 6);
        (*root)->keys[i][6] = '\0';
        (*root)->value[i] = value;
        (*root)->keys_cnt++;
        return 0;
    }
    if ((*root)->keys_cnt == MAX_SIZE) {
        Node *new_root = create_node(0);
        if (new_root == NULL) return -1;

        Node *old_root = *root;
        *root = new_root;
        new_root->children[0] = old_root;

        if (split_child(*root, 0, old_root) != 0) return -1;

        *root = new_root;
    }
    insert_non_full(*root, key, value);
    return 0;
}

void remove_from_leaf(Node *root, int index) {
    if (root == NULL || !root->is_leaf) return;
    for (int i = index + 1; i < root->keys_cnt; i++) {
        strncpy(root->keys[i - 1], root->keys[i], 6);
        root->keys[i - 1][6] = '\0';
        root->value[i - 1] = root->value[i];
    }
    root->keys_cnt--;
}

void merge(Node *root, int index) {
    if (root == NULL) return;
    Node *child = root->children[index];
    Node *sibling = root->children[index + 1];

    strncpy(child->keys[T - 1], root->keys[index], 6);
    child->keys[T - 1][6] = '\0';
    child->value[T - 1] = root->value[index];

    for (int i = 0; i < sibling->keys_cnt; i++) {
        strncpy(child->keys[i + T], sibling->keys[i], 6);
        child->keys[i + T][6] = '\0';
        child->value[i + T] = sibling->value[i];
    }

    if (!child->is_leaf) {
        for (int i = 0; i < sibling->keys_cnt; i++) {
            child->children[i + T] = sibling->children[i];
        }
    }

    for (int i = index + 1; i < root->keys_cnt; i++) {
        strncpy(child->keys[i - 1], child->keys[i], 6);
        child->keys[i - 1][6] = '\0';
        child->value[i - 1] = child->value[i];
    }

    for (int i = index + 2; i <= root->keys_cnt; i++) {
        root->children[i - 1] = root->children[i];
    }

    child->keys_cnt += sibling->keys_cnt + 1;
    root->keys_cnt--;

    free(sibling);
}

void borrow_from_prev(Node *root, int index) {
    if (root == NULL) return;

    Node *child = root->children[index];
    Node *sibling = root->children[index - 1];

    for (int i = child->keys_cnt - 1; i >= 0; --i) {
        strncpy(child->keys[i + 1], child->keys[i], 6);
        child->keys[i + 1][6] = '\0';
        child->value[i + 1] = child->value[i];
    }
    if (!child->is_leaf) {
        for (int i = child->keys_cnt; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    strncpy(child->keys[0], root->keys[index - 1], 6);
    child->keys[0][6] = '\0';
    child->value[0] = root->value[index - 1];
    if (!child->is_leaf) child->children[0] = sibling->children[sibling->keys_cnt];

    strncpy(root->keys[index - 1], sibling->keys[sibling->keys_cnt - 1], 6);
    root->keys[index - 1][6] = '\0';
    root->value[index - 1] = sibling->value[sibling->keys_cnt - 1];

    child->keys_cnt++;
    sibling->keys_cnt--;
}

void borrow_from_next(Node *root, int index) {
    if (root == NULL) return;

    Node *child = root->children[index];
    Node *sibling = root->children[index + 1];

    strncpy(child->keys[child->keys_cnt], root->keys[index], 6);
    child->keys[child->keys_cnt][6] = '\0';
    child->value[child->keys_cnt] = root->value[index];

    if (!child->is_leaf) {
        child->children[child->keys_cnt + 1] = sibling->children[0];
    }

    strncpy(root->keys[index], sibling->keys[0], 6);
    root->keys[index][6] = '\0';
    root->value[index] = sibling->value[0];

    for (int i = 1; i < sibling->keys_cnt; ++i) {
        strncpy(sibling->keys[i - 1], sibling->keys[i], 6);
        sibling->keys[i - 1][6] = '\0';
        sibling->value[i - 1] = sibling->value[i];
    }

    if (!sibling->is_leaf) {
        for (int i = 1; i <= sibling->keys_cnt; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->keys_cnt++;
    sibling->keys_cnt--;
}

void fill(Node *node, int index) {
    if (index != 0 && node->children[index - 1]->keys_cnt >= T)
        borrow_from_prev(node, index);
    else if (index != node->keys_cnt && node->children[index + 1]->keys_cnt >= T)
        borrow_from_next(node, index);
    else {
        if (index != node->keys_cnt) merge(node, index);
        else merge(node, index - 1);
    }
}

void remove_from_non_leaf(Node *root, int index) {
    if (root == NULL) return;

    char k[7];
    strncpy(k, root->keys[index], 6);
    k[6] = '\0';

    if (root->children[index]->keys_cnt >= T) {
        Node *cur = root->children[index];
        while (!cur->is_leaf) cur = cur->children[cur->keys_cnt];

        strncpy(root->keys[index], cur->keys[cur->keys_cnt - 1], 6);
        root->keys[index][6] = '\0';
        root->value[index] = cur->value[cur->keys_cnt - 1];

        delete_node(root->children[index], root->keys[index]);
    } else if (root->children[index + 1]->keys_cnt >= T) {
        Node *cur = root->children[index + 1];
        while (!cur->is_leaf) cur = cur->children[0];

        strncpy(root->keys[index], cur->keys[0], 6);
        root->keys[index][6] = '\0';
        root->value[index] = cur->value[0];

        delete_node(root->children[index + 1], root->keys[index]);
    } else {
        merge(root, index);
        delete_node(root->children[index], k);
    }
}


int delete_node(Node *root, const char *key) {
    int i = 0;
    while (i < root->keys_cnt && strcmp(key, root->keys[i]) > 0) i++;

    if (i < root->keys_cnt && strcmp(key, root->keys[i]) == 0) {
        if (root->is_leaf) {
            remove_from_leaf(root, i);
        } else {
            remove_from_non_leaf(root, i);
        }
    } else {
        if (root->is_leaf) {
            return -1;
        }

        if (root->children[i]->keys_cnt < T) {
            fill(root, i);
        }
        delete_node(root->children[i], key);
    }
    return 0;
}

int delete(Node **root, const char *key) {
    if (*root == NULL) return -1;

    int res = delete_node(*root, key);

    if ((*root)->keys_cnt == 0) {
        Node *old_root = *root;

        if ((*root)->is_leaf) {
            *root = NULL;
        } else {
            *root = (*root)->children[0];
        }

        free(old_root);
    }
    return res;
}

double search(const Node *root, const char *key) {
    if (root == NULL) return 0.0;

    int i = 0;
    while (i < (root)->keys_cnt && strcmp(key, root->keys[i]) > 0) i++;

    if (i < root->keys_cnt && strcmp(key, root->keys[i]) == 0) return root->value[i];

    if (root->is_leaf) return 0.0;

    return search(root->children[i], key);
}

void print(Node *root, int level) {
    if (root == NULL) return;
    for (int i = root->keys_cnt; i >= 0; --i) {
        if (!root->is_leaf && root->children[i] != NULL) {
            print(root->children[i], level + 1);
        }
        if (i > 0) {
            for (int j = 0; j < level; ++j) {
                fputs("    ", stdout); 
            }
            printf("[%s : %.4f]\n", root->keys[i-1], root->value[i-1]);
        }
    }
}



int main() {
    BTree tree = {NULL};
    char line[256];
    FILE *file = fopen("../tests/commands.txt", "r");
    if (!file) {
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        int op;
        char key[64];
        double val;

        if (sscanf(line, "%d", &op) < 1) continue;

        switch (op) {
            case 1:
                if (sscanf(line, "%d %63s %lf", &op, key, &val) == 3) {
                    insert_node(&(tree.root), key, val);
                }
                break;

            case 2:
                if (sscanf(line, "%d %63s", &op, key) == 2) {
                    delete(&(tree.root), key);
                }
                break;

            case 3:
                if (tree.root) {
                    fputs("Tree:\n", stdout);
                    print(tree.root, 0);
                } else {
                    printf("Tree is empty\n");
                }
                fputs("\n", stdout);
                break;
            
            case 4:
                if (sscanf(line, "%d %63s", &op, key) == 2) {
                    double res = search(tree.root, key);
                    if (res != 0.0) printf("Found: %s = %.4f\n", key, res);
                }
                fputs("\n", stdout);
                break;
        }
    }

    fclose(file);
    return 0;
}