#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>

int *vector = NULL;

int leer_datos(char *filename, int **v){
    int totalnumeros =0, temp;
    FILE *file = fopen(filename, "r");

    if(!file){
        printf("Error al abrir\n"); 
        exit(1);
    }
    fscanf(file, "%d", &totalnumeros);
    
    (*v) = (int *)calloc(totalnumeros, sizeof(int));
    
    if(!(*v)){
        printf("Error memoria\n"); 
        exit(2);
    }
    
    for(int i=0; i<totalnumeros; i++){
        fscanf(file, "%d", &temp);
        (*v)[i] = temp;
    } 
    return totalnumeros;
}

int numeroDeValoresImpares(int *v, int totalnumeros){
    int contador = 0;
    for(int i=0; i<totalnumeros; i++){
        if(v[i] % 2 != 0){
            contador+=1;
        }
    }
    return contador;
}

int numeroDeValoresPares(int *v, int totalnumeros){
    int contador = 0;
    for(int i=0; i<totalnumeros; i++){
        if(v[i] % 2 == 0){
            contador+=1;
        }
    }
    return contador;
}

float promedioDeValores(int *v, int totalnumeros){
    float suma = 0;
    for(int i=0; i<totalnumeros; i++){
        suma += v[i];
    }
    return (suma/(float)totalnumeros);
}

int numeroMasRepetido(int *v,int totalnumeros){
    int mayorNumero = 0;
    for(int i=0; i < totalnumeros; i++){
        if(v[i]>mayorNumero){
            mayorNumero = v[i];
        }
    }
    int *arrayRepetidos = (int *)calloc(mayorNumero,sizeof(int));   
    for(int i=0; i<totalnumeros;i++){
        arrayRepetidos[v[i]]++;
    }
    int mayorRep=0, mayorNum=0;
    for(int i=0;i<mayorNumero;i++){
        if(arrayRepetidos[i]>mayorRep){
            mayorRep = arrayRepetidos[i];
            mayorNum = i;
        }
    }
    return mayorNum;
}

int main(int argc, char **argv){
    pid_t child;
    int childs[4];
    int tube[4][2];
    int tube1[4][2];
    int i;
    
    for(int j=0; j<4; j++){
        pipe(tube[j]);
        pipe(tube1[j]);
    }

    for(i=0; i<4; i++){
        child = fork();
        if(!child){
            childs[i] = child;
            break;
        }
    }
    int tn = leer_datos("./datos.txt", &vector);
    float buffer;
    int bufferInt;
    
    if(i==4){
        for(int j=0; j<4; j++){
            close(tube[j][1]);
            close(tube1[j][0]);
        }   
        for(int j=0; j<4; j++){
            write(tube1[j][1],vector,sizeof(vector));
        }
        for(int j=0; j<4;j++){wait(NULL);}
        for(int j=0; j<4; j++){
            switch(j){
                case 0: 
                    read(tube[j][0],&bufferInt,sizeof(int));
                    printf("la cantidad de numeros impares es: %d\n",bufferInt);
                    break;
                case 1: 
                    read(tube[j][0],&bufferInt,sizeof(int));
                    printf("la cantidad de numeros pares es: %d\n",bufferInt);
                    break;
                case 2: 
                    read(tube[j][0],&buffer,sizeof(float));
                    printf("el promedio es: %f\n",buffer);
                    break;
                case 3: 
                    read(tube[j][0],&bufferInt,sizeof(int));
                    printf("el numero mas repetido es: %d\n",bufferInt);
                    break;
            }
        }
    }else{
        for(int j=0; j<4; j++){
            if(i==j){
                close(tube[j][0]);
                close(tube1[j][1]);
            }else{
                close(tube[j][0]);
                close(tube[j][1]);
                close(tube1[j][0]);
                close(tube1[j][1]);
            }
        }
        read(tube[i][0],&buffer,sizeof(float));
        if(i==0){//numeros impare
            int cantidadImpares = numeroDeValoresImpares(vector,tn);
            // printf("el mensaje enviado fue %d por %d\n",cantidadImpares,i);
            write(tube[i][1],&cantidadImpares,sizeof(int));
        }else if(i==1){//numeros pares
            int cantidadPares = numeroDeValoresPares(vector,tn);
            // printf("el mensaje enviado fue %d por %d\n",cantidadPares,i);
            write(tube[i][1],&cantidadPares,sizeof(int));
        }else if(i==2){//promedio
            float promedio = promedioDeValores(vector,tn);
            // printf("el mensaje enviado fue %f por %d\n",promedio,i);
            write(tube[i][1],&promedio,sizeof(float));
        }else if(i==3){//numeros repetidos
            int numeroRep = numeroMasRepetido(vector,tn);
            // printf("el mensaje enviado fue %d por %d\n",numeroRep,i);
            write(tube[i][1],&numeroRep,sizeof(int));
        }
    }


    return 0;
}