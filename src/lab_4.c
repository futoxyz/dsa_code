#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define TAB 4

typedef enum Color: char { 
    BLACK, 
    RED 
} Color;

typedef struct Node {
    int value;
    Color color;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

int print_tree(const Node *root, int level, HANDLE hConsole) {
    if (root == NULL) {
        return 1;
    }
    print_tree(root->right, level + TAB, hConsole);
    for (int i = 0; i < level; i++) {
        fputs(" ", stdout);
    }
    if (root->color == RED) {
        SetConsoleTextAttribute(hConsole, 4);
        printf("%d\n", root->value);
        SetConsoleTextAttribute(hConsole, 7);
    } else printf("%d\n", root->value);
    print_tree(root->left, level + TAB, hConsole);
    return 0;
}

void rotate_left(Node **root, Node *x) {
    if (root == NULL || *root == NULL || x == NULL || x->right == NULL) return;
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
    return;
}

void rotate_right(Node **root, Node *y) {
    if (root == NULL || *root == NULL || y == NULL || y->left == NULL) return;
    Node *x = y->left;
    y->left = x->right;
    if (x->right != NULL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL) *root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    x->right = y;
    y->parent = x;
    return;
}

Node* create_node(int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) return NULL;
    node->value = value;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

int fix_insert(Node **root, Node *z) {
    if (root == NULL || *root == NULL || z == NULL) return -1;
    while (z != *root && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node *uncle = z->parent->parent->right;
            
            if (uncle != NULL && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { 
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_right(root, z->parent->parent);
            }
        } else {
            Node *uncle = z->parent->parent->left;
            
            if (uncle != NULL && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_left(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
    return 0;
}

int insert(Node **root, int value) {
    if (root == NULL) return -1;
    Node *new_node = create_node(value);
    if (new_node == NULL) return -2;
    if (*root == NULL) {
        new_node->color = BLACK;
        *root = new_node;
        return 0;
    }
    Node *current = *root;
    Node *parent = NULL;
    while (current != NULL) {
        parent = current;
        if (value < current->value) {
            current = current->left;
        } else if (value > current->value) {
            current = current->right;
        } else {
            free(new_node);
            return -3; 
        }
    }
    new_node->parent = parent;
    if (value < parent->value) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
    fix_insert(root, new_node);
    return 0;
}


int free_root(Node *root) {
    if (root == NULL) return 1;
    free_root(root->left);
    free_root(root->right);
    free(root);
    return 0;
}

int deinit(Node *root) {
    if (root == NULL) {
        return 1;
    }
    deinit(root->left);
    deinit(root->right);
    free(root);
    root = NULL;
    return 0;
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
    
    Node *tree = NULL;
    insert(&tree, 5);
    insert(&tree, 8);
    insert(&tree, 3);
    insert(&tree, 2);
    print_tree(tree, 0, hConsole);
    deinit(tree);
    return 0;
}