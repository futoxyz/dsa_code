#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h> 

typedef enum Color: char { 
    BLACK, 
    RED 
} Color;


typedef struct Node {
    int value;
    Color color;
    struct Node *left;
    struct Node *right;
} Node;

int print_tree(const Node *root, int level, HANDLE hConsole) {
    if (root == NULL) {
        return 1;
    }
    print_tree(root->right, level + 4, hConsole);
    for (int i = 0; i < level; i++) {
        fputs(" ", stdout);
    }
    if (root->color == RED) {
        SetConsoleTextAttribute(hConsole, 4);
        printf("%d\n", root->value);
        SetConsoleTextAttribute(hConsole, 7);
    } else printf("%d\n", root->value);
    print_tree(root->left, level + 4, hConsole);
    return 0;
}




int insert(Node **root, const int value) {
    if (*root == NULL) {
        Node *new_node = (Node *) malloc(sizeof(Node));
        if (new_node == NULL) {
            return -1;
        }
        new_node->value = value;
        if (value % 2 == 0) {
            new_node->color = RED;
        } else new_node->color = BLACK;
        new_node->left = NULL;
        new_node->right = NULL;
        *root = new_node;
    }
    if (value < (*root)->value) {
        insert(&(*root)->left, value);
    } else if (value > (*root)->value) {
        insert(&(*root)->right, value);
    }
    return 0;
}

int free_root(Node *root) {
    if (root == NULL) return 1;
    free_root(root->left);
    free_root(root->right);
    free(root);
    return 0;
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    Node *tree = NULL;
    insert(&tree, 5);
    insert(&tree, 8);
    print_tree(tree, 0, hConsole);
    return 0;
}