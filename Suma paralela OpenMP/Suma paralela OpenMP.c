#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define ARRAY_SIZE 100000000  // 100 millones de elementos

int main() {
    int num_threads[] = {1, 2, 4, 8, 16};
    int num_configs = sizeof(num_threads) / sizeof(num_threads[0]);
    

    int *array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        return 1;
    }
    

    printf("Inicializando arreglo de %d elementos...\n", ARRAY_SIZE);
    srand(time(NULL));
    for (long i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100;
    }
    
    printf("\nResultados de rendimiento:\n");
    printf("========================================\n");
    printf("Hilos\tTiempo (s)\tSpeedup\t\tEficiencia\n");
    printf("========================================\n");
    
    double tiempos[5]; 
    

    for (int t = 0; t < num_configs; t++) {
        int n_threads = num_threads[t];
        long long suma = 0;
        double tiempo_inicio, tiempo_fin;
        
        omp_set_num_threads(n_threads);
        
        tiempo_inicio = omp_get_wtime();
        
        #pragma omp parallel for reduction(+:suma)
        for (long i = 0; i < ARRAY_SIZE; i++) {
            suma += array[i];
        }
        
        tiempo_fin = omp_get_wtime();
        double tiempo_ejecucion = tiempo_fin - tiempo_inicio;
        tiempos[t] = tiempo_ejecucion;
        
        if (t == 0) {
            printf("%d\t%.6f\t%.2f\t\t%.3f\n", 
                   n_threads, tiempo_ejecucion, 1.00, 1.000);
        }
    }
    
    double T1 = tiempos[0];
    for (int t = 0; t < num_configs; t++) {
        int n_threads = num_threads[t];
        double speedup = T1 / tiempos[t];
        double eficiencia = speedup / n_threads;
        
        printf("%d\t%.6f\t%.4f\t\t%.4f\n", 
               n_threads, tiempos[t], speedup, eficiencia);
    }
    
    printf("========================================\n");
    
    printf("\nAnálisis de Ley de Amdahl:\n");
    printf("========================================\n");
    

    double f_estimada = 0.999;  
    
    printf("Fracción paralelizable estimada (f): %.3f (99.9%%)\n", f_estimada);
    printf("\nSpeedup teórico según Ley de Amdahl:\n");
    printf("Hilos\tSpeedup Teórico\tSpeedup Experimental\tDiferencia\n");
    printf("----------------------------------------\n");
    
    for (int t = 0; t < num_configs; t++) {
        int p = num_threads[t];
        double speedup_teorico = 1.0 / ((1.0 - f_estimada) + (f_estimada / p));
        double speedup_exp = T1 / tiempos[t];
        double diferencia = speedup_teorico - speedup_exp;
        
        printf("%d\t%.4f\t\t%.4f\t\t%.4f\n", 
               p, speedup_teorico, speedup_exp, diferencia);
    }
    
    printf("========================================\n");
    
    printf("\nAnálisis de Escalabilidad:\n");
    printf("========================================\n");
    
    for (int t = 0; t < num_configs; t++) {
        int p = num_threads[t];
        double speedup = T1 / tiempos[t];
        double eficiencia = speedup / p;
        
        printf("p=%d:\n", p);
        printf("  Speedup: %.4f\n", speedup);
        printf("  Eficiencia: %.4f (%.2f%%)\n", eficiencia, eficiencia * 100);
        
        if (eficiencia > 0.8) {
            printf("  → Excelente escalabilidad\n");
        } else if (eficiencia > 0.6) {
            printf("  → Buena escalabilidad\n");
        } else if (eficiencia > 0.4) {
            printf("  → Escalabilidad moderada\n");
        } else {
            printf("  → Baja escalabilidad\n");
        }
        printf("\n");
    }
    
    free(array);
    
    return 0;
}