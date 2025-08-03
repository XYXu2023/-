#include <stdio.h>
#include <stdlib.h>

struct Edge {
    int city1, city2;//两个顶点
    float cost;//权重
};

// 打印邻接矩阵
void printAdjacencyMatrix(int n, float adjMatrix[n][n]) {
    printf("\nAdjacency Matrix:\n    ");
    for (int i = 0; i < n; i++) printf("%-8d", i); //打印城市编号
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%-4d", i);  // 打印城市编号
        for (int j = 0; j < n; j++) {
            if (i == j) {
                printf("%-8.2f", 0.00);  
            } else {
                printf("%-8.2f", adjMatrix[i][j]);
            }
        }
        printf("\n");
    }
}

//并查集查找父亲结点
int find(int parent[], int i) {
    if (parent[i] == i)
        return i;
    return find(parent, parent[i]);
}

void kruskal(struct Edge graph[], int n, int edge_count) {
    //按边权排序
    for (int i = 0; i < edge_count - 1; i++) {
        for (int j = 0; j < edge_count - i - 1; j++) {
            if (graph[j].cost > graph[j + 1].cost) {
                struct Edge temp = graph[j];
                graph[j] = graph[j + 1];
                graph[j + 1] = temp;
            }
        }
    }

    int *parent = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        parent[i] = i; //事先把每个点的父亲初始化为它自己

    printf("\nMinimum Cost Connection Plan:\n");
    float total_cost = 0;

    for (int i = 0; i < edge_count; i++) {
        int root1 = find(parent, graph[i].city1);
        int root2 = find(parent, graph[i].city2);

        if (root1 != root2) {  
            printf("City %d -- City %d : Cost %.2f\n", 
                   graph[i].city1, graph[i].city2, graph[i].cost);
            total_cost += graph[i].cost;
            parent[root1] = root2;  
        }
    }
    printf("Total Cost: %.2f\n", total_cost);
    free(parent);
}

int main() {
    int n;
    printf("Enter number of cities: ");
    scanf("%d", &n);

    // 初始化邻接矩阵
    float adjMatrix[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            adjMatrix[i][j] = 0;
        }
    }

    
    int max_edges = n * (n - 1) / 2;
    struct Edge *graph = (struct Edge *)malloc(max_edges * sizeof(struct Edge));

    printf("Enter connection costs (format: city1 city2 cost), enter -1 -1 -1 to finish:\n");
    int edge_count = 0;
    while (1) {
        int city1, city2;
        float cost;
        scanf("%d %d %f", &city1, &city2, &cost);

        if (city1 == -1 && city2 == -1 && cost == -1)
            break;

        if (city1 < 0 || city2 < 0 || city1 >= n || city2 >= n) {
            printf("Invalid input! City IDs must be 0 to %d\n", n - 1);
            continue;
        }

        
        graph[edge_count].city1 = city1;
        graph[edge_count].city2 = city2;
        graph[edge_count].cost = cost;
        
        
        adjMatrix[city1][city2] = cost;
        adjMatrix[city2][city1] = cost;
        
        edge_count++;
    }

    
    printAdjacencyMatrix(n, adjMatrix);

    kruskal(graph, n, edge_count);
    free(graph);
    return 0;
}

