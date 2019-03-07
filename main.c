//MAIN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 3000


    FILE *fp; 
    FILE *fo;  

int main(int argc, char *argv[]){
    char direccion[100];
    getcwd(direccion, sizeof(direccion))  ; //direccion del archivo en cuestion  tambien en la plantilla de nasm
    if (argc== 1){
        printf("Error al encontrar ruta de archivo \n");
        return 1;
    };
    strcat( direccion,"/");
    strcat( direccion,argv[1]);
    fp = fopen(direccion, "r");
    fo = fopen("output.txt","w");
    if(fp == NULL){
        fprintf(stderr,"Error al encontrar el archivo");
        exit(1);
    }
    system_goal();
    fclose(fp);
    fclose(fo);
    ensamblerGenerator();
    ejecutarArchivo();
    return 0;
}

//Proceso para generar ensamblador


// cambiarle ADD -> add ->Add dependiendo de lo que tenga gabo

//Archivos para generar codigo
 FILE *ft; //Archivo de codigo intermedio
FILE *Temp; // Archivo de codigo ensamblador

int cantidadPrints  =  0 ;

// int main()
// {
//     ensamblerGenerator();
//     ejecutarArchivo();
//     return 0;
// }



void ensamblerGenerator(){
    char str[MAXCHAR];
//Manejo de archivos
    fp = fopen("output.txt", "r");
    if(fp == NULL){
        fprintf(stderr,"Error al abrir archivo \n");
        exit(1);
    }

    Temp=fopen("file.asm","w");
    if(Temp == NULL){
        fprintf(stderr,"Error al abrir archivo \n ");
        exit(1);
    }
    //printf("Direccion procesada  \n ");

 //Proceso inicial
    char * pch;
    //Variable para los declares
    char declares [MAXCHAR];
    strcpy(declares, "");
    //Variable para codigo general
    char generalCode [MAXCHAR];
    strcpy(generalCode,"");


    while (fgets(str, MAXCHAR, fp) != NULL){
        pch = strtok (str," ,.");
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Declare")){
            int flag = 0 ;
            pch = strtok (NULL, " ,.-&");
            char  * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                    flag = 1;
                    pch = strtok (NULL, " ,.-");
                    numeroVariable = pch;
            }
            pch = strtok (NULL, " ,.-");
            char  * tipoVariable  = pch;
            strcat(declares,nombreVariable);
            if (flag ==1 ){
                strcat(declares,numeroVariable);
            }
            strcat(declares," resb ");
            strcat(declares,"2048 \n");
        }
/*------------------------------------------ ------------------------------*/
        if (!strcmp(pch, "Halt")){
            strcat(generalCode,"\tmov  ebx, 0 \n");
            strcat(generalCode,"\tmov  eax, 1 \n");
            strcat(generalCode,"\tint  80h\n");
            strcat(generalCode,"\tret \n");
        }
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Store")){
            char variablePrimera [MAXCHAR];
            char variableSegunda [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, "&\n,");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            pch = strtok (NULL, "\n,&");
            nombreVariable  = pch;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variableSegunda, nombreVariable);
            if (flag ==1 ){
                strcat(variableSegunda,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tmov eax,  ");

            if (isdigit(variablePrimera[0])) strcat(generalCode , variablePrimera);
            else{
               if(isdigit(variablePrimera[1])) {
                    strcat(generalCode , variablePrimera);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variablePrimera);
                   strcat(generalCode , "]");
               }
            }
             strcat(generalCode , "\n\tmov ");
            if (isdigit(variableSegunda[0])) strcat(generalCode , variableSegunda);
            else{
               strcat(generalCode , "[");
               strcat(generalCode , variableSegunda);
               strcat(generalCode , "]");
            }
            strcat(generalCode , " , eax \n");
        }
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Sub")){

            char variablePrimera [MAXCHAR];
            char variableSegunda [MAXCHAR];
            char variableTercera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, ",&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            pch = strtok (NULL, ",&");
            nombreVariable  = pch;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",");
                numeroVariable = pch;
            }
            strcpy(variableSegunda, nombreVariable);
            if (flag ==1 ){
                strcat(variableSegunda,numeroVariable);
                flag = 0;
            }

            pch = strtok (NULL, ",&\n");
            nombreVariable = pch;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",\n");
                numeroVariable = pch;
            }
            strcpy(variableTercera, nombreVariable);
            if (flag ==1 ){
                strcat(variableTercera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tmov eax ,");
             if (isdigit(variablePrimera[0])){
                strcat(generalCode , variablePrimera);
                strcat(generalCode , "\n");
            }else{
               if(isdigit(variablePrimera[1])) {
                    strcat(generalCode , variablePrimera);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variablePrimera);
                   strcat(generalCode , "]");
               }
            }
            strcat(generalCode , "\n");
            strcat(generalCode , "\tmov ebx ,");
            if (isdigit(variableSegunda[0])){
                strcat(generalCode , variableSegunda);
                strcat(generalCode , "\n");
            }  else{
               if(isdigit(variableSegunda[1])) {
                    strcat(generalCode , variableSegunda);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variableSegunda);
                   strcat(generalCode , "]");
               }
            }
            strcat(generalCode , "\n");
            strcat(generalCode , "\tsub eax ,ebx \n");
            strcat(generalCode , "\tmov ");
            if (isdigit(variableTercera[0])) {
                strcat(generalCode , variableTercera);
                
            }else{
               if(isdigit(variableTercera[1])) {
                    strcat(generalCode , variableTercera);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variableTercera);
                   strcat(generalCode , "]");
               }
               strcat(generalCode , ", eax \n");
            }

        }
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Add")){
            char variablePrimera [MAXCHAR];
            char variableSegunda [MAXCHAR];
            char variableTercera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, ",&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            pch = strtok (NULL, ",&");
            nombreVariable  = pch;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, ",");
                numeroVariable = pch;
            }
            strcpy(variableSegunda, nombreVariable);
            if (flag ==1 ){
                strcat(variableSegunda,numeroVariable);
                flag = 0;
            }
            pch = strtok (NULL, ",&\n");
            nombreVariable = pch;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variableTercera, nombreVariable);
            if (flag ==1 ){
                strcat(variableTercera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tmov eax ,");
             if (isdigit(variablePrimera[0])){
                strcat(generalCode , variablePrimera);
                strcat(generalCode , "\n");
            }else{
               if(isdigit(variablePrimera[1])) {
                    strcat(generalCode , variablePrimera);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variablePrimera);
                   strcat(generalCode , "]");
               }
            }
            strcat(generalCode , "\n");
            strcat(generalCode , "\tmov ebx ,");
            if (isdigit(variableSegunda[0])){
                strcat(generalCode , variableSegunda);
                strcat(generalCode , "\n");
            }  else{
               if(isdigit(variableSegunda[1])) {
                    strcat(generalCode , variableSegunda);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variableSegunda);
                   strcat(generalCode , "]");
               }
            }
            strcat(generalCode , "\n");
            strcat(generalCode , "\tadd eax ,ebx \n");
            strcat(generalCode , "\tmov ");
            if (isdigit(variableTercera[0])) {
                strcat(generalCode , variableTercera);
                
            }else{
               if(isdigit(variableTercera[1])) {
                    strcat(generalCode , variableTercera);
               }
               else{
                   strcat(generalCode , "[");
                   strcat(generalCode , variableTercera);
                   strcat(generalCode , "]");
               }
               strcat(generalCode , ", eax \n");
            }
        }
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Read")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, ",&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }

            char variableSegunda [MAXCHAR];
            strcpy(variableSegunda,"");
            if (isdigit(variablePrimera[0])) strcat(variableSegunda , variablePrimera);
            else{
               strcat(variableSegunda , "[");
               strcat(variableSegunda , variablePrimera);
               strcat(variableSegunda, "] ");
            }

            strcat(generalCode , "\tmov ebp, 0\n");
            strcat(generalCode , "\tmov eax, 3\n");
            strcat(generalCode , "\tmov ebx, 0\n");
            strcat(generalCode ,"\tmov ecx, ");
            strcat(generalCode ,variablePrimera);
            strcat(generalCode , "\n\tmov edx, 2048 \n");
            strcat(generalCode , "\tint 80h \n");
            strcat(generalCode ,"\tmov eax, ecx \n");
            strcat(generalCode ,"\tcall atoi\n");
            strcat(generalCode, "\tcmp ebp, 0 \n");

            char strNumPrints[8];
            sprintf(strNumPrints, "%d", cantidadPrints);

            strcat(generalCode ,"\tje guardarVar");
            strcat(generalCode ,strNumPrints);
            strcat(generalCode ,"\n\tneg eax\n");
            strcat(generalCode ,"guardarVar");
            strcat(generalCode ,strNumPrints);
            strcat(generalCode ,":\n");
            strcat(generalCode ,"\tmov ");
            strcat(generalCode ,variableSegunda);
            strcat(generalCode ,", eax\n ");
            cantidadPrints++;
        }
/*-------------------------------------------------------------------------*/
        if (!strcmp(pch, "Write")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, ",&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "TEMP")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            char strNumPrints[8];
            sprintf(strNumPrints, "%d", cantidadPrints);
            strcat(generalCode , "\tmov eax, [");
            strcat(generalCode , variablePrimera);
            strcat(generalCode , "]\n");
            strcat(generalCode , "\tcmp eax, 0\n");
            strcat(generalCode , "\tjge .printRes");
            strcat(generalCode , strNumPrints);
            strcat(generalCode , "\n\tpush eax\n");
            strcat(generalCode , "\tmov eax, 4\n");
            strcat(generalCode , "\tmov ebx, 1\n");
            strcat(generalCode , "\tmov ecx, signoMenos\n");
            strcat(generalCode , "\tmov edx, lenMenos\n");
            strcat(generalCode , "\tint 80h\n");
            strcat(generalCode , "\tpop eax\n");
            strcat(generalCode , "\tneg eax\n");
            strcat(generalCode , "\t.printRes");
            strcat(generalCode , strNumPrints);
            strcat(generalCode , ":\n\t\tcall iprintLF \n");
            cantidadPrints++;
        }
        if (!strcmp(pch, "Eti")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, " \n,&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "TEMP")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , variablePrimera );
            strcat(generalCode , ":\n");
        }
        if (!strcmp(pch, "Jmp")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, ",&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "TEMP")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tjmp ");
            strcat(generalCode , variablePrimera );
            strcat(generalCode , "\n");
        }
        if (!strcmp(pch, "Jz")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, "\n&,");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "TEMP")){
                flag = 1;
                pch = strtok (NULL, "\n,");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tjz ");
            strcat(generalCode , variablePrimera );
            strcat(generalCode , "\n");
        }

        if (!strcmp(pch, "Skip")){
            char variablePrimera [MAXCHAR];
            int flag = 0;
            pch = strtok (NULL, "\n,&");
            char * nombreVariable  = pch;
            char *numeroVariable;
            if (!strcmp(pch, "Temp")){
                flag = 1;
                pch = strtok (NULL, " \n");
                numeroVariable = pch;
            }
            strcpy(variablePrimera, nombreVariable);
            if (flag ==1 ){
                strcat(variablePrimera,numeroVariable);
                flag = 0;
            }
            strcat(generalCode , "\tmov eax,  ");
            if (isdigit(variablePrimera[0])){
                strcat(generalCode , variablePrimera);
            }else{
                if(isdigit(variablePrimera[1])){
                    strcat(generalCode , variablePrimera);
                }
                else{
                    strcat(generalCode , "[");
                   strcat(generalCode , variablePrimera);
                   strcat(generalCode , "] ");  
                }
            }
            strcat(generalCode,"\n");
            strcat(generalCode,"\tcmp eax, 0\n");
        }

        //pch = strtok (NULL, " ,.-");
    }
    //Plantilla de ensablador
    //functions.asm
    //Plantilla de ensablador
    //functions.asm
    fprintf(Temp, "\n ; Archivo para ensamblador de NASM Linux  \n \n");
    fputc('%', Temp );
    fprintf(Temp, "include '/bib.asm'  \n");
    fprintf(Temp, "section .bss \n");
    fprintf(Temp, declares);
    fprintf(Temp, "\n\nsection .data \n");
    fprintf(Temp, "signoMenos db 45\n");
    fprintf(Temp, "lenMenos equ $-signoMenos\n\n");
    fprintf(Temp, "section .data \n");
    fprintf(Temp, "\n\n");
    fprintf(Temp, "section .text \n \t global _start  \n\n_start: \n ");
    fprintf(Temp, generalCode);
    fprintf(Temp, "\n\n");


    //--------------------------------------
    fprintf(Temp, "checkNegativo:\n");
    fprintf(Temp, "\tcmp eax, 0\n");
    fprintf(Temp, "\tjge .return\n");
    fprintf(Temp, "\tpush eax\n");
    fprintf(Temp, "\tmov eax, 4\n");
    fprintf(Temp, "\tmov ebx, 1 \n");
    fprintf(Temp, "\tcmp ecx, signoMenos\n");
    fprintf(Temp, "\tmov edx,1\n");
    fprintf(Temp, "\tint 80h\n");
    fprintf(Temp, "\tpop eax\n");
    fprintf(Temp, "\tneg eax\n");
    fprintf(Temp, "\t.return:\n");
    fprintf(Temp, "\t\tret\n");

    fclose(fp);
    fclose(Temp);
}

void ejecutarArchivo(){
	int flag=0;
   	printf("\nProcesando...\n");
   	printf("\nEjecutando... \n\n");
/* COMPILAR PARA 32
nasm -f elf32 myprog.asm
ld -melf_i386 -o myprog myprog.o

COMPILAR PARA 64 EN RUNNER DE NASM
*/
   char FILENAME[] = "file"; //-->PONER NOMBRE DEL ARCHIVO ACÁ
   char str[350];
   strcpy(str, "nasm -f elf32 ");
   strcat(str, FILENAME);
   strcat(str, ".asm");
   strcat(str, " && ld -melf_i386 -o ");
   strcat(str, FILENAME);
   strcat(str, " ");
   strcat(str, FILENAME);
   strcat(str, ".o");
   strcat(str, " && ./");
   strcat(str, FILENAME);

   flag = system(str);
   printf("\n");
   if (!flag){
     printf("Proceso terminado exitosamente.\n");
   }
   else {printf("Error en ejecucion de archivo\n");}

}