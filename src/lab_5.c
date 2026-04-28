#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void print_matrix(double **m, int v, int e) {
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < e; j++) {
            printf("%g", m[i][j]);
            fputs("\t", stdout);
        }
        fputs("\n", stdout);
    }
}


double** to_adj(double **inc, int v, int e) {
    double **adj = (double **)malloc(v * sizeof(double *));
    if (adj == NULL) return NULL;

    for (int i = 0; i < v; i++) {
        adj[i] = (double *)calloc(v, sizeof(double));
        if (adj[i] == NULL) {
            for (int j = 0; j < i; j++) free(adj[j]);
            free(adj);
            return NULL;
        }
    }
    for (int j = 0; j < e; j++) {
        int u_idx = -1, v_idx = -1;
        double weight = 1.0;

        for (int i = 0; i < v; i++) {
            if (inc[i][j] != 0) {
                if (u_idx == -1) u_idx = i;
                else v_idx = i;
                
                if (inc[i][j] > 0 && inc[i][j] != 1.0) weight = inc[i][j];
            }
        }

        if (u_idx != -1 && v_idx != -1) {
            adj[u_idx][v_idx] = weight;
            adj[v_idx][u_idx] = weight;
        }
    }

    return adj;
}


double** floyd(double **adj, int v) {
    double **dist = (double **)malloc(v * sizeof(double *));
    if (dist == NULL) return NULL;
    for (int i = 0; i < v; i++) {
        dist[i] = (double *)malloc(v * sizeof(double));
        if (dist[i] == NULL) {
            for (int j = 0; j < i; j++) free(dist[j]);
            free(dist);
            return NULL;
        }
        for (int j = 0; j < v; j++) {
            if (i == j) dist[i][j] = 0;
            else if (adj[i][j] != 0) dist[i][j] = adj[i][j];
            else dist[i][j] = INFINITY;
        }
    }
    for (int k = 0; k < v; k++) {
        for (int i = 0; i < v; i++) {
            for (int j = 0; j < v; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
    return dist;
}


int main() {
    FILE *f = fopen("tests/graph_input.txt", "r");
    if (!f) return 1;
    int v = 0, e = 0;
    char *buf = (char *) malloc(1024);
    if (buf == NULL) {
        return -1;
    }
    while (fgets(buf, 1024, f)) {
        if (v == 0) {
            char *ptr = buf;
            double val;
            int n;
            while (sscanf(ptr, "%lf%n", &val, &n) == 1) {
                e++;
                ptr += n;
            }
        }
        v++;
    }
    if (v == 0 || e == 0) {
        fclose(f);
        return 1;
    }
    double **matrix = (double **)malloc(v * sizeof(double *));
    if (matrix == NULL) {
        fclose(f);
        return 1;
    }
    for (int i = 0; i < v; i++) {
        matrix[i] = (double *)malloc(e * sizeof(double));
        if (matrix[i] == NULL) {
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            fclose(f);
            return 1;
        }
    }
    rewind(f);
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < e; j++) {
            if (fscanf(f, "%lf", &matrix[i][j]) != 1) break;
        }
    }
    fclose(f);
    print_matrix(matrix, v, e);
    
    double **adj = to_adj(matrix, v, e);
    if (adj == NULL) return -1;
    fputs("\nadj matrix\n\n", stdout);
    print_matrix(adj, v, v);

    double **res = floyd(adj, v);
    if (res == NULL) return -1;
    fputs("\nfloyd worshell matrix\n\n", stdout);
    print_matrix(res, v, v);

    int start, end;
    char *inp_buf = (char *) malloc(1024);
    if (inp_buf == NULL) {
        return -1;
    }
    fputs("\nEnter two vertices: \n", stdout);

    while (fgets(inp_buf, 1024, stdin)) {
        if (inp_buf[0] == '\n' || inp_buf[0] == '\0') break;
        if (sscanf(inp_buf, "%d %d", &start, &end) == 2) {
            if (start >= 0 && start < v && end >= 0 && end < v) {
                if (res[start][end] >= INFINITY) {
                    printf("No path between %d and %d\n", start, end);
                } else {
                    printf("distance: %g\n", res[start][end]);
                }
            } else {
                fputs("Out of range\n", stdout);
            }
        }
    }


    for (int i = 0; i < v; i++) free(matrix[i]);
    free(matrix);
    for (int i = 0; i < v; i++) free(adj[i]);
    free(adj);
    for (int i = 0; i < v; i++) free(res[i]);
    free(res);

    free(buf);
    free(inp_buf);
    return 0;
}
