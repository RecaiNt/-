#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct {
    int id;
    double weight;
    double value;
    double density;
} Item;

int compare(const void *a, const void *b) {
    Item *itemA = (Item*)a;
    Item *itemB = (Item*)b;
    return (itemA->density > itemB->density) ? -1 : 1;
}

void generateItems(Item items[], int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = 1.0 + (double)rand() / RAND_MAX * 99.0;
        items[i].value = 100.0 + (double)rand() / RAND_MAX * 900.0;
        items[i].density = items[i].value / items[i].weight;
    }
}

double greedyKnapsack(Item items[], int n, double capacity, int *selected) {
    qsort(items, n, sizeof(Item), compare);
    
    double totalValue = 0.0;
    double remainingCapacity = capacity;
    int count = 0;
    
    for (int i = 0; i < n && remainingCapacity > 0; i++) {
        if (items[i].weight <= remainingCapacity) {
            selected[++count] = items[i].id;
            totalValue += items[i].value;
            remainingCapacity -= items[i].weight;
        }
    }
    
    selected[0] = count;
    return totalValue;
}

void printResults(int n, double capacity, double totalValue, 
                  double timeUsed, int *selected, Item *items) {
    double totalWeight = 0.0;
    for (int i = 1; i <= selected[0]; i++) {
        for (int j = 0; j < n; j++) {
            if (items[j].id == selected[i]) {
                totalWeight += items[j].weight;
                break;
            }
        }
    }
    
    printf("\n===== 贪心法求解结果 =====\n");
    printf("物品数量: %d\n", n);
    printf("背包容量: %.2f\n", capacity);
    printf("最大总价值: %.2f\n", totalValue);
    printf("执行时间: %.2f ms\n", timeUsed);
    printf("选中物品数量: %d\n", selected[0]);
    printf("------------------------\n");
    printf("总重量: %.2f / %.2f\n", totalWeight, capacity);
    printf("------------------------\n");
}

int main() {
    int n;
    double capacity;
    
    printf("===== 0-1背包问题-贪心法 =====\n");
    printf("请输入物品数量n: ");
    scanf("%d", &n);
    
    printf("请输入背包容量C: ");
    scanf("%lf", &capacity);
    
    Item *items = (Item*)malloc(n * sizeof(Item));
    int *selected = (int*)malloc((n + 1) * sizeof(int));
    
    generateItems(items, n);
    printf("已生成%d个物品，背包容量=%.2f\n", n, capacity);
    
    clock_t start = clock();
    double maxValue = greedyKnapsack(items, n, capacity, selected);
    clock_t end = clock();
    double timeUsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    
    printResults(n, capacity, maxValue, timeUsed, selected, items);
    
    free(items);
    free(selected);
    return 0;
}
