#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>
u_int64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((u_int64_t)hi << 32) | lo;
}
int random_num(){
   srand(rdtsc());
   return (rand() % 100);
}
void merge(int n,int arr[],int i1,int j1,int i2,int j2){
   int aux[n];  
   int i,j,k;
   i=i1;    
   j=i2;    
   k=0;
   while(i<=j1 && j<=j2){
      if(arr[i]<arr[j]){
         aux[k]=arr[i];
         i=i+1;
         k=k+1;
      }
      else{
         aux[k]=arr[j];
         k=k+1;
         j=j+1;
      }
    }
   while(i<=j1){    //Paso los otros elementos al auxiliar
      aux[k]=arr[i];
      k=k+1;
      i=i+1;
   }
   while(j<=j2){   
      aux[k]=arr[j];
      j=j+1;
      k=k+1;
      } 
   for(i=i1,j=0;i<=j2;i++,j++){//Traspaso el auxiliar al arreglo original
      arr[i]=aux[j];//ESTO FUNCIONA YA QUE A ES CREADO COMO PUNTERO Arr UN ESPACIO DE MEM
     //printf("%d%s",arr[i]," ");
   }
    //printf("\n");
}
void Merge_s_normal(int n,int arr[],int izq,int der){
   if(izq<der){
        int mid=(izq+der)/2;
        Merge_s_normal(n,arr,izq,mid); 
        Merge_s_normal(n,arr,mid+1,der);
        merge(n,arr,izq,mid,mid+1,der); 
   }            
}
void Merge_s_para(int n,int arr[],int izq,int der,int threads){
   if(threads == 1){
      Merge_s_normal(n,arr,izq,der);
   }
   else if(threads >1){
      int mid=(der+izq)/2;
      #pragma omp parallel sections
      {
         #pragma omp section
         {
         Merge_s_para(n,arr,izq,mid,threads/2);
         }  
         #pragma omp section
         {
         Merge_s_para(n,arr,mid+1,der,threads - threads / 2);
         }
      }
      merge(n,arr,izq,mid,mid+1,der);
   }   
}
void escribir_arreglo_en(char file_name[],int arr[] ,int n ){
   FILE *flujo = fopen(file_name,"w");
   fprintf(flujo,"inicio\n");
   for (int i=0;i<n;i++){
      fprintf(flujo,"%d%s",arr[i]," ");
   }
   fprintf(flujo,"final\n");
   fclose( flujo);
}
int main(){
   int *arr;
   int *arr2;
   int n=10000;
   int i,num;
   clock_t in ,fn;
   double seg;
   double start, stop;
   arr = (int *)malloc(sizeof(int) * n);
   arr2 = (int *)malloc(sizeof(int) * n);
   for(i=0;i<n;i++){
      num=random_num();
      arr[i]=num;
      arr2[i]=num;
   }
   escribir_arreglo_en("inputs10000.txt",arr,n);
   int threads = 4;
   omp_set_num_threads (threads);
   omp_set_nested(1);
   //primer merge
   start = omp_get_wtime();
   Merge_s_para(n,arr2, 0, n-1,threads);
   stop = omp_get_wtime();
   //segundo merge
   in = clock();
   Merge_s_normal(n,arr,0,n-1);
   fn = clock();
   //entrega de datos
   seg = (double)(fn - in) / CLOCKS_PER_SEC;
   printf("Merge paralelo: %g%s\n",stop-start," segundos.");
   printf("%s%f%s\n","Merge normal:",seg," segundos.");
   escribir_arreglo_en("output_normal10000.txt",arr,n);
   escribir_arreglo_en("output_paralelo10000.txt",arr2,n);
   FILE *flujo = fopen("tiempos.txt","a");
   fprintf(flujo,"%s%d\n","------------ N=",n);
   fprintf(flujo,"%s%f\n","Merge normal",seg);
   fprintf(flujo,"%s%g\n","Merge paralelo:",stop-start);
   fclose( flujo );
   free(arr2);
   free(arr);
   return 0;
}