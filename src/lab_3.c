#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct tree_node {
    char *data;
    struct tree_node *left;
    struct tree_node *right;
} tree_node;

typedef struct stack_node {
    char *data;
    struct stack_node *next;
} stack_node;

typedef struct stack {
    stack_node *top;
} stack;


typedef struct stack_tree_node {
    tree_node *node;
    struct stack_tree_node *next;
} stack_tree_node;

typedef struct tree_stack {
    stack_tree_node *top;
} tree_stack;


int print_tree(const tree_node *root, int level) {
    if (root == NULL) return 1;
    print_tree(root->right, level + 4);
    for (int i = 0; i < level; i++) {
        fputs(" ", stdout);
    }
    fputs(root->data, stdout);
    fputs("\n", stdout);
    print_tree(root->left, level + 4);
    return 0;
}

void init_stack(stack *s) { s->top = NULL; }


int is_empty(stack *s) { return s->top == NULL; }


int push(stack *s, char *value) {
    stack_node *new_node = malloc(sizeof(stack_node));
    if (!new_node) return -1;
    new_node->data = value;
    new_node->next = s->top;
    s->top = new_node;
    return 0;
}

char *pop(stack *s) {
    if (is_empty(s)) return NULL;
    stack_node *temp = s->top;
    s->top = s->top->next;
    char *result = temp->data;
    free(temp);
    return result;
}

char *peek(stack *s) {
    return is_empty(s) ? NULL : s->top->data;
}


void init_tree_stack(tree_stack *s) { 
    s->top = NULL; 
}


int is_empty_tree(tree_stack *s) { 
    return s->top == NULL; 
}


int push_node(tree_stack *s, tree_node *t_node) {
    stack_tree_node *new_node = malloc(sizeof(stack_tree_node));
    if (!new_node) return -1;
    new_node->node = t_node;
    new_node->next = s->top;
    s->top = new_node;
    return 0;
}

tree_node *pop_node(tree_stack *s) {
    if (is_empty_tree(s)) return NULL;
    stack_tree_node *temp = s->top;
    s->top = s->top->next;
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
    while (*start < size && isspace((unsigned char)src[*start])) (*start)++;
    if (*start >= size) return NULL;
    
    int capacity = 8;
    char *result = malloc(capacity);
    size_t res_size = 0;

    if (isdigit(src[*start])) {
        while (*start < size && isdigit((unsigned char)src[*start])) {
            if (res_size + 2 > capacity) {
                capacity *= 2;
                result = realloc(result, capacity);
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
    if (!src || !*src) return 0;
    for (int i = 0; src[i]; i++) if (!isdigit(src[i])) return 0;
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
    stack s;
    init_stack(&s);
    size_t dst_size = 0, i = 0;
    char *cur;

    while ((cur = get_string(src, &i, n)) != NULL) {
        if (digit_check(cur)) {
            dst[dst_size++] = cur;
        } else if (strcmp(cur, "(") == 0) {
            push(&s, cur);
        } else if (strcmp(cur, ")") == 0) {
            while (!is_empty(&s) && strcmp(peek(&s), "(") != 0) {
                dst[dst_size++] = pop(&s);
            }
            free(pop(&s));
            free(cur);
        } else {
            while (!is_empty(&s) && get_priority(peek(&s)) >= get_priority(cur)) {
                dst[dst_size++] = pop(&s);
            }
            push(&s, cur);
        }
    }
    while (!is_empty(&s)) dst[dst_size++] = pop(&s);
    return (int)dst_size;
}

int build_tree(tree_node **root, char **src, size_t size) {
    tree_stack s;
    init_tree_stack(&s);
    for (int i = 0; i < size; i++) {
        tree_node *new_node = create_tree_node(src[i]);
        if (!digit_check(src[i])) {
            new_node->right = pop_node(&s);
            new_node->left = pop_node(&s);
        }
        push_node(&s, new_node);
    }
    *root = pop_node(&s);
    return 1;
}


void deinit_root(tree_node *root) {
    if (root == NULL) return;
    deinit_root(root->left);
    deinit_root(root->right);
    free(root->data);
    free(root);
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
    fputs("\n", stdout);
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
    for (int i = 0; i < dst_size; i++) {
        free(expr[i]);
    }
    free(expr);
    free(buf);
    return 0;
}