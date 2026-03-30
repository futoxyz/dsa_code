#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct tree_node {
    char *data;
    struct tree_node *left;
    struct tree_node *right;
} tree_node;

int print_tree(const tree_node *root, int level) {
    if (root == NULL) {
        return 1;
    }
    print_tree(root->right, level + 4);

    for (int i = 0; i < level; i++) {
        fputs(" ", stdout);
    }
    fputs("[", stdout);
    fputs(root->data, stdout);
    fputs("]\n", stdout);

    print_tree(root->left, level + 4);
    return 0;
}


typedef struct stack_node {
    char *data;
    struct stack_node *next;
} stack_node;

int is_empty(stack_node **top) {
    return *top == NULL;
}

int push(stack_node **top, char *value) {
    stack_node *new_node = malloc(sizeof(stack_node));
    if (new_node == NULL) return -1;
    new_node->data = value;
    new_node->next = *top;
    *top = new_node;
    return 0;
}

char *pop(stack_node **top) {
    if (is_empty(top)) return NULL;
    stack_node *temp = *top;
    *top = (*top)->next;
    char *result = temp->data;
    free(temp);
    return result;
}

char *peek(stack_node **top) {
    if (is_empty(top)) return NULL;
    return (*top)->data;
}


typedef struct stack_tree {
    tree_node *node;
    struct stack_tree *next;
} stack_tree;

int is_empty_stack(stack_tree **top) {
    return *top == NULL;
}

int push_node(stack_tree **top, tree_node *tree_node) {
    stack_tree *new_stack_node_node = malloc(sizeof(stack_tree));
    if (new_stack_node_node == NULL) return -1;
    new_stack_node_node->node = tree_node;
    new_stack_node_node->next = *top;
    *top = new_stack_node_node;
    return 0;
}

tree_node *pop_node(stack_tree **top) {
    if (is_empty_stack(top)) return NULL;
    stack_tree *temp = *top;
    *top = (*top)->next;
    tree_node *res = temp->node;
    free(temp);
    return res;
}

tree_node *create_tree_node(const char *val) {
    tree_node *node = malloc(sizeof(tree_node));
    if (!node) return NULL;
    node->data = strdup(val);
    node->left = node->right = NULL;
    return node;
}


char *get_string(const char *src, size_t *start, size_t size) {
    while (*start < size && isspace((unsigned char) src[*start])) {
        (*start)++;
    }
    if (*start >= size) return NULL;
    int capacity = 8;
    char *result = (char *) malloc(capacity * sizeof(char));
    if (result == NULL) return NULL;
    size_t res_size = 0;
    if (isdigit(src[*start])) {
        while (*start < size && isdigit((unsigned char) src[*start])) {
            if (res_size + 2 > capacity) {
                capacity *= 2;
                char *buf = (char *) realloc(result, capacity * sizeof(char));
                if (buf == NULL) {
                    free(result);
                    return NULL;
                }
                result = buf;
            }
            result[res_size++] = src[(*start)++];
        }
    } else {
        result[res_size++] = src[(*start)++];
    }
    result[res_size] = '\0';
    return result;
}


int digit_check(const char *src) {
    if (src == NULL || *src == '\0') return 0;
    size_t len = strlen(src);
    for (size_t i = 0; i < len; i++) {
        unsigned char cur = src[i];
        if (!isdigit(cur)) {
            return 0;
        }
    }
    return 1;
}

int get_priority(const char *op) {
    if (strcmp(op, "(") == 0) return 0;
    if (strcmp(op, ")") == 0) return 1;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 2;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 3;
    return -1;
}

int poliz(char **dst, const char *src, size_t n) {
    if (src == NULL || n == 0) return -1;
    stack_node *stack_node = NULL;
    size_t dst_size = 0;
    size_t i = 0;

    char *cur;
    while ((cur = get_string(src, &i, n)) != NULL) {
        if (digit_check(cur)) {
            dst[dst_size++] = cur;
        } else if (strcmp(cur, "(") == 0) {
            push(&stack_node, cur);
        } else if (strcmp(cur, ")") == 0) {
            while (!is_empty(&stack_node) && strcmp(peek(&stack_node), "(") != 0) {
                dst[dst_size++] = pop(&stack_node);
            }
            if (!is_empty(&stack_node)) {
                char *open_bracket = pop(&stack_node);
                free(open_bracket);
            }
            free(cur);
        } else {
            int p_cur = get_priority(cur);
            while (!is_empty(&stack_node) && get_priority(peek(&stack_node)) >= p_cur) {
                dst[dst_size++] = pop(&stack_node);
            }
            push(&stack_node, cur);
        }
    }

    while (!is_empty(&stack_node)) {
        char *current = pop(&stack_node);
        if (strcmp(current, "(") == 0) {
            free(current);
        } else {
            dst[dst_size++] = current;
        }
    }

    return (int) dst_size;
}

int deinit_root(tree_node *root) {
    if (root == NULL) return 1;
    deinit_root(root->left);
    deinit_root(root->right);
    free(root->data);
    free(root);
    return 0;
}


int build_tree(tree_node **root, char **src, size_t size) {
    if (src == NULL) return -1;
    stack_tree *stack_node = NULL;
    for (int i = 0; i < size; i++) {
        tree_node *new_node = create_tree_node(src[i]);
        if (new_node == NULL) return -1;
        if (!digit_check(src[i])) {
            new_node->right = pop_node(&stack_node);
            new_node->left = pop_node(&stack_node);
            if (new_node->right == NULL || new_node->left == NULL) {
                deinit_root(new_node);
                return -1;
            }
        }
        push_node(&stack_node, new_node);
    }
    *root = pop_node(&stack_node);
    return 1;
}


int deinit_expr(char **expr, int size) {
    if (expr == NULL) return 1;
    if (size <= 0) return -1;
    for (int i = 0; i < size; i++) {
        free(expr[i]);
    }
    free(expr);
    return 0;
}


int simplification(tree_node *root) {
    if (root == NULL) return 1;

    if (strcmp(root->data, "/") == 0 && root->left && root->right) {
        tree_node *L = root->left;
        tree_node *R = root->right;

        if (strcmp(L->data, "*") == 0 && strcmp(R->data, "2") == 0) {
            tree_node *LL = L->left;
            tree_node *LR = L->right;

            if (LL && strcmp(LL->data, "4") == 0) {
                free(LL->data);
                LL->data = strdup("2");

                free(root->data);
                root->data = strdup("*");

                root->left = LL;
                root->right = LR;

                free(L->data);
                free(L);
                free(R->data);
                free(R);

                return 0;
            }
        }
    }
    return -1;
}



int main() {
    tree_node *root = NULL;
    char *buf = (char *) malloc(1024);
    if (buf == NULL) {
        return -1;
    }
    if (fgets(buf, 1024, stdin) == NULL || buf[0] == '\n' || buf[0] == '\0') {
        return 0;
    }
    buf[strcspn(buf, "\n")] = 0;
    char **expr = (char **) malloc(1024 * sizeof(char *));
    if (expr == NULL) {
        free(buf);
        return -1;
    }
    int dst_size = poliz(expr, buf, strlen(buf));
    if (dst_size <= 0) {
        free(buf);
        return -1;
    }
    build_tree(&root, expr, dst_size);
    simplification(root);
    print_tree(root, 0);

    deinit_root(root);
    deinit_expr(expr, dst_size);
    free(buf);
    return 0;
}