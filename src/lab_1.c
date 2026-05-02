#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


typedef struct tree_node {
    int value;
    struct tree_node *left;
    struct tree_node *right;
} tree_node;


int insert(tree_node **root, const int value) {
    if (*root == NULL) {
        tree_node *new_node = (tree_node *) malloc(sizeof(tree_node));
        if (new_node == NULL) {
            return -1;
        }
        new_node->value = value;
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

int delete(tree_node **root, const int value) {
    if (*root == NULL) {
        return -1;
    }
    tree_node *temp = *root;
    if (value < (*root)->value) {
        delete(&(*root)->left, value);
    } else if (value > (*root)->value) {
        delete(&(*root)->right, value);
    } else {
        if ((*root)->left == NULL && (*root)->right == NULL) {
            *root = NULL;
            free(temp);
            return 0;
        }
        if ((*root)->left == NULL && (*root)->right != NULL) {
            *root = (*root)->right;
            free(temp);
            return 0;
        }
        if ((*root)->right == NULL && (*root)->left != NULL) {
            *root = (*root)->left;
            free(temp);
            return 0;
        }
        if ((*root)->left != NULL && (*root)->right != NULL) {
            const tree_node *curr = (*root)->right;
            while (curr->left != NULL) {
                curr = curr->left;
            }
            (*root)->value = curr->value;
            delete(&(*root)->right, curr->value);
            return 0;
        }
    }
    return 0;
}

int print(const tree_node *root, int level) {
    if (root == NULL) {
        return 1;
    }
    print(root->right, level + 4);
    for (int i = 0; i < level; i++) {
        fputs(" ", stdout);
    }
    printf("%d", root->value);
    fputs("\n", stdout);
    print(root->left, level + 4);
    return 0;
}

void max_leaf_depth(const tree_node *root, int current_depth, int *max_depth, int *deepest_value) {
    if (root == NULL) {
        return;
    }
    if (root->left == NULL && root->right == NULL) {
        if (current_depth > *max_depth) {
            *max_depth = current_depth;
            *deepest_value = root->value;
        }
        return;
    }
    max_leaf_depth(root->left, current_depth + 1, max_depth, deepest_value);
    max_leaf_depth(root->right, current_depth + 1, max_depth, deepest_value);
}

int leaf_depth_value(const tree_node *root) {
    if (root == NULL) {
        return -1;
    }
    int max_depth = 0;
    int leaf_value = root->value;
    max_leaf_depth(root, 1, &max_depth, &leaf_value);
    return leaf_value;
}

void deinit(tree_node *root) {
    if (root == NULL) return;
    deinit(root->left);
    deinit(root->right);
    free(root);
}


int main() {
    tree_node *tree = NULL;
    insert(&tree, 5);
    insert(&tree, 10);
    insert(&tree, 3);
    insert(&tree, 2);
    print(tree, 0);
    printf("\ndeepest leaf value: %d", leaf_depth_value(tree));
    deinit(tree);
    return 0;
}