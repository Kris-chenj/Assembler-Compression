/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you 
    should define and implement your own functions in translator.c, compression.c, 
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>      /*import*/

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"    /*import*/

/*check if file can be correctly opened */
static int open_files(FILE** input, FILE** output, const char* input_name, const char* output_name){ 
    *input = fopen(input_name, "r");
    if (!*input){ /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output){ /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE** input, FILE** output){
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit() {
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}

static int convert_int(const char* str,int n)   /*get the DEC number of it*/
{
    long int ans = strtol(str,NULL,2);
    
    if(str[0] == '0' || n == 5){return ans;}   /*get it directly*/
    else                                        
    {
        if(n == 12){ans = (0xfffff000 | ans);}
        else if(n == 20){ans = (0xfff00000 | ans);}
    }
    return ans;                                 /*get it signed*/
}

/*Run the translator */
int translate(const char*in, const char*out)
{
    char inputstr[50][50];       /* input*/
    char outputstr[50][50];      /* output*/
    int jump_byte[50][50];       /* respersent the byte number sum between each instruction*/ 
    int whether_comp[50];        /* 1 represent compressible ,0 respresent cannot compress*/
    int innumber=0;              /* repersent number of instructions*/
    char x[50];
    int i=0,j,k;
    int dum,regirs1,regird;      /*creat temp*/
    char temp[50],sw_imm[50],s5;
    char subr1[50],subr2[50],subr3[50],subr4[50],subr5[50];        /*creat temp*/
    FILE *input, *output;
    int err = 0;
    if (in){    /* correct input file name */
        if(open_files(&input, &output, in, out) != 0)
            exit(1);
        /* ... */
        for ( i = 0; i < 50; i++){
        memset(inputstr[i],'\0',sizeof(inputstr[i]));
        memset(outputstr[i],'\0',sizeof(outputstr[i]));}
        /*initail the array*/
        while (~fscanf(input,"%s",inputstr[innumber]))
        {
            innumber++;
        }
        /* here code*/
        for ( i = 0; i < 50; i++)
        {
            whether_comp[i] = 0;
        }
                                           /*initilize*/
        for(i = 0;i < innumber;++i)
        {
            memset(temp,'\0',sizeof(temp));
            memset(subr1,'\0',sizeof(subr1));
            memset(subr2,'\0',sizeof(subr2));         /*initilize*/
            memset(subr3,'\0',sizeof(subr3));
            memset(subr4,'\0',sizeof(subr4));
            memset(subr5,'\0',sizeof(subr5));         /*initilize*/
            memset(sw_imm,'\0',sizeof(sw_imm));
            strncpy(temp,inputstr[i] + 25,7);
            if(strcmp(temp,"0000011") == 0)                        /*for lw*/
            {
                strncpy(subr1,inputstr[i],12);      /* get immediate(20bit - 31bit)*/
                strncpy(subr2,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr3,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                strncpy(subr4,inputstr[i] + 17,3);  /* get func3(12bit - 14bit)*/
                if(strcmp(subr4,"010") == 0)        /*for lw*/
                {
                    dum = convert_int(subr1,12);
                    regirs1 = convert_int(subr2,5);
                    regird = convert_int(subr3,5);  
                    if(dum >= 0 && dum % 4 == 0 && dum < 128 && regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15){whether_comp[i] = 1;}     /*for lw operation*/
                }
            }
            else if(strcmp(temp,"0010011") == 0)                   /*for addi,slli,srli,srai,andi*/
            {
                strncpy(subr1,inputstr[i],12);      /* get immediate(20bit - 31bit)*/
                strncpy(subr2,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr3,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                strncpy(subr4,inputstr[i] + 17,3);  /* get func3(12bit - 14bit)*/

                if(strcmp(subr4,"000") == 0)    /*for addi*/
                {
                    dum = convert_int(subr1,12);
                    if(strcmp(subr2,"00000") == 0 && strcmp(subr3,"00000") != 0 && dum >= -32 && dum < 32){whether_comp[i] = 1;}/*sb*/
                    else if(dum != 0 && strcmp(subr2,subr3) == 0 && strcmp(subr2,"00000") != 0 && dum >= -32 && dum < 32){whether_comp[i] = 1;}   /*for addi*/
                }
                
                else if(strcmp(subr4,"111") == 0)    /*for andi*/
                {
                    dum = convert_int(subr1,12);
                    regird = convert_int(subr2,5);
                    if(strcmp(subr2,subr3) == 0 && regird >= 8 && regird <= 15 && dum >= -32 && dum < 32){whether_comp[i] = 1;}      /*for andi*/
                }
                
                else if(strcmp(subr4,"001") == 0)   /*for slli*/
                {
                    s5 = subr1[6];
                    if(strcmp(subr2,subr3) == 0 && strcmp(subr3,"00000") != 0 && s5 == '0'){whether_comp[i] = 1;}
                }
                
                else if(strcmp(subr4,"101") == 0)   /*for srli,srai*/
                {
                    s5 = subr1[6];
                    regird = convert_int(subr3,5);
                    if(strcmp(subr2,subr3) == 0 && regird >= 8 && regird <= 15 && s5 == '0'){whether_comp[i] = 1;}
                }
            }
            else if(strcmp(temp,"0110111") == 0)              /* lui*/
            {
                strncpy(subr1,inputstr[i],20);      /* get immediate(12bit - 31bit)*/
                strncpy(subr3,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                dum = convert_int(subr1,20);
                if(strcmp(subr1,"00000000000000000000")!=0 && strcmp(subr3,"00000")!=0 && strcmp(subr3,"00010")!=0 && dum >= -32 && dum < 32){whether_comp[i] = 1;}
            }

            else if(strcmp(temp,"1101111") == 0)              /* for jal*/
            {
                strncpy(subr3,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                if(strcmp(subr3,"00000") == 0 || strcmp(subr3,"00001") == 0){whether_comp[i] = 1;}
            }

            else if(strcmp(temp,"1100111") == 0)              /* for jalr*/
            {
                strncpy(subr1,inputstr[i],12);      /* get immediate(20bit - 31bit)*/
                strncpy(subr2,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr3,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                strncpy(subr4,inputstr[i] + 17,3);  /* get func3(12bit - 14bit)*/
                if(strcmp(subr4,"000") == 0)
                {
                    dum = convert_int(subr1,12);
                    if(dum == 0 && strcmp(subr2,"00000") != 0 && strcmp(subr3,"00000") == 0){whether_comp[i] = 1;} /*snzj*/
                    else if(dum == 0 && strcmp(subr2,"00000") != 0 && strcmp(subr3,"00001") == 0){whether_comp[i] = 1;}       /* for jalr*/
                }
            }

            else if(strcmp(temp,"0110011") == 0)              /* for add,and,or,xor,sub*/
            {
                strncpy(subr1,inputstr[i],7);       /* get funct7(25bit - 31bit)*/
                strncpy(subr2,inputstr[i] + 17,3);  /* get funct3(12bit - 14bit)*/
                strncpy(subr3,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr4,inputstr[i] + 7,5);   /* get rs2(20bit - 24bit)*/
                strncpy(subr5,inputstr[i] + 20,5);  /* get rd(7bit - 11bit)*/
                if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"000") == 0)  /*for add*/
                {
                    if(strcmp(subr4,"00000")!=0 && strcmp(subr5,"00000")!=0 && strcmp(subr3,subr5) == 0){whether_comp[i] = 1;}
                    else if(strcmp(subr4,"00000")!=0 && strcmp(subr5,"00000")!=0 && strcmp(subr3,"00000") == 0){whether_comp[i] = 1;}
                }
                else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"111") == 0)  /*for and*/
                {
                    regird = convert_int(subr5,5);
                    regirs1 = convert_int(subr4,5);
                    if(strcmp(subr3,subr5) == 0 && regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15){whether_comp[i] = 1;}/*for and*/
                }
                else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"110") == 0)  /*for or*/
                {
                    regird = convert_int(subr5,5);
                    regirs1 = convert_int(subr4,5);
                    if(strcmp(subr3,subr5) == 0 && regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15){whether_comp[i] = 1;}/*for or*/
                }
                else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"100") == 0)  /*for xor*/
                {
                    regird = convert_int(subr5,5);
                    regirs1 = convert_int(subr4,5);
                    if(strcmp(subr3,subr5) == 0 && regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15){whether_comp[i] = 1;}/*for xor*/
                }
                else if(strcmp(subr1,"0100000") == 0 && strcmp(subr2,"000") == 0)  /*for sub*/
                {
                    regird = convert_int(subr5,5);
                    regirs1 = convert_int(subr4,5);
                    if(strcmp(subr3,subr5) == 0 && regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15){whether_comp[i] = 1;}/*for sub*/
                }
            }
            else if(strcmp(temp,"0100011") == 0)              /*for sw*/
            {
                for(j = 0;j <= 6;++j){sw_imm[j] = inputstr[i][j];}       /* get contious imm to sw_imm*/
                k = 7;
                for(j = 20;j <= 24;++j){sw_imm[k] = inputstr[i][j];++k;}
                strncpy(subr2,inputstr[i] + 17,3);  /* get funct3(12bit - 14bit)*/
                strncpy(subr3,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr4,inputstr[i] + 7,5);   /* get rs2(20bit - 24bit)*/

                if(strcmp(subr2,"010") == 0)
                {
                    regirs1 = convert_int(subr3,5);
                    regird = convert_int(subr4,5);
                    dum = convert_int(sw_imm,12);
                    if(regirs1 >= 8 && regirs1 <= 15 && regird >= 8 && regird <= 15 && dum >= 0 && dum % 4 == 0 && dum < 128){whether_comp[i] = 1;}       /*for sw*/
                }
            }
            else if(strcmp(temp,"1100011") == 0)              /*for beq,bne*/
            {
                strncpy(subr2,inputstr[i] + 17,3);  /* get funct3(12bit - 14bit)*/
                strncpy(subr3,inputstr[i] + 12,5);  /* get rs1(15bit - 19bit)*/
                strncpy(subr4,inputstr[i] + 7,5);   /* get rs2(20bit - 24bit)*/
                if(strcmp(subr2,"000") == 0 || strcmp(subr2,"001") == 0)
                {
                    regirs1 = convert_int(subr3,5);
                    if(strcmp(subr4,"00000") == 0 && regirs1 >= 8 && regirs1 <= 15){whether_comp[i] = 1;}             /*for beq,bne*/
                }
            }
        }   /*go next*/

        for ( i = 0; i < innumber; i++)
        {
            for ( j = 0; j < innumber; j++)
            {                                    /*get circle*/
                jump_byte[i][j]=0;
                if (i<=j)
                {
                    for ( k = i; k < j; k++)              /*if jump back*/
                {                                
                    if (whether_comp[k] == 1)
                    {
                        jump_byte[i][j]=jump_byte[i][j]+2;
                    }else{                                    /*if jump */
                        jump_byte[i][j]=jump_byte[i][j]+4;
                    }}
                }else{
                    for ( k = j; k < i; k++)               /*store value*/
                    {
                        if (whether_comp[k] == 1)
                    {
                        jump_byte[i][j]=jump_byte[i][j]+2;            /*update value*/
                    }else{
                        jump_byte[i][j]=jump_byte[i][j]+4;}}
                }
            }                /*end*/
        }
        /* initialize the jump_byte gap*/


        for (i = 0; i < innumber; i++)                             /* go to compress*/
        {
            memset(outputstr[i],'\0',sizeof(outputstr[i]));
            if(whether_comp[i] == 0)
            {
                branch9(inputstr[i],outputstr[i],jump_byte,i);     /*discompressible*/
            }
            else
            {
                memset(temp,'\0',sizeof(temp));               /* initialize*/
                strncpy(temp,inputstr[i] + 25,7);
                if(strcmp(temp,"0000011") == 0)                   /*for lw*/
                {
                    branch8(inputstr[i],outputstr[i]);
                }
                else if(strcmp(temp,"0010011") == 0)                   /*for addi,slli,srli,srai,andi*/
                {
                    branch1(inputstr[i],outputstr[i]);
                }
                else if(strcmp(temp,"0110111") == 0)             /* lui*/
                {
                    branch2(inputstr[i],outputstr[i]);
                }

                else if(strcmp(temp,"1101111") == 0)              /* for jal*/
                {
                    branch3(inputstr[i],outputstr[i],jump_byte,i);
                }

                else if(strcmp(temp,"1100111") == 0)              /* for jalr*/
                {
                branch4(inputstr[i],outputstr[i]);
                }

                else if(strcmp(temp,"0110011") == 0)              /* for add,and,or,xor,sub*/
                {
                    branch5(inputstr[i],outputstr[i]);
                }
                else if(strcmp(temp,"0100011") == 0)              /*for sw*/
                {
                    branch6(inputstr[i],outputstr[i]);
                }
                else if(strcmp(temp,"1100011") == 0)              /*for beq,bne*/
                {
                    branch7(inputstr[i],outputstr[i],jump_byte,i);
                }
            }
        }


        for ( i = 0; i < innumber; i++)              /*restore*/
        {
            if (whether_comp[i] == 1)
            {
                for ( j = 0; j < 16; j++)            /*reverse*/
                {
                    x[j] = outputstr[i][15-j]; 
                }
                for ( j = 0; j < 16; j++)           /*restore*/
                {
                    outputstr[i][j] = x[j];
                }
            }                                   /*get value*/
        }
        /* code*/
        for (i = 0; i < innumber; i++)
        {
            if (whether_comp[i]==0)     /*cannot compress*/
            {
                outputstr[i][32]=0;
                fprintf(output,"%s\n",outputstr[i]);
            }else{                      /*can compress*/
                outputstr[i][16]=0;
                fprintf(output,"%s\n",outputstr[i]);
            }
        }
        
        /* write correct result to the output file */
        /* ... */
        close_files(&input, &output);
    }
    return err;
}

/* main func */
int main(int argc, char **argv){
    char* input_fname, *output_fname;
    int err;

    if (argc != 3)                 /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}



/*
git add .
git commit -m {message}
git push origin {branchname}

git fetch --all
git pull origin {branchname}
git checkout {branch}
*/