#include <stdio.h>
#include <string.h>
#include <stdlib.h>           /*import*/
#include "compression.h"
#include "utils.h"

/* Your code here... */

static int convert_int(const char* str,int n)   /*get the DEC number of it*/
{
    long int ans = strtol(str,NULL,2);
    
    if(str[0] == '0' || n == 5){return ans;}     /*assess*/
    else
    {
        if(n == 12){ans = (0xfffff000 | ans);}
        else if(n == 20){ans = (0xfff00000 | ans);}      /*assess*/
    }
    return ans;
}

static char bechar(int a){      /*convert 1,0 to char*/
    if (a==1)
    {
        return '1';
    }
    return '0';                /*get char*/
    }
    
/*for c.addi,c.slli,c.srli,c.srai,c.andi,c.lw,c.li*/
void branch1(char* str,char* outstr){
    /*create temp*/
    int i;
    char subr1[50],subr2[50],subr3[50],subr4[50],subr5[50],shamt5[50],s5;
    /*initial temp*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr2,'\0',sizeof(subr2));
    memset(subr3,'\0',sizeof(subr3));
    memset(subr4,'\0',sizeof(subr4));        /*initial temp*/
    memset(subr5,'\0',sizeof(subr5));
    memset(shamt5,'\0',sizeof(shamt5));
    strncpy(subr1,str,12);          /* get immediate(20bit - 31bit)*/
    strncpy(subr2,str + 12,5);      /* get rs1(15bit - 19bit)*/
    strncpy(subr3,str + 20,5);      /* get rd(7bit - 11bit)*/
    strncpy(subr4,str + 17,3);      /* get func3(12bit - 14bit)*/
    strncpy(shamt5,str + 7,5);      /* get shamt(20bit - 24bit)*/
    s5 = subr1[6];
    if(strcmp(subr4,"000") == 0)    
    {
        if(strcmp(subr2,subr3) == 0)          /*for c.addi*/
        {
            outstr[15]='0';
            outstr[14]='0';
            outstr[13]='0';
            /*funct*/
            outstr[12]=subr1[6];
            /*imm*/
            for ( i = 0; i < 5; i++)
            {
                outstr[11-i]=subr3[i];
            }
            /*rd*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i]=subr1[7+i];
            }
            /*imm*/
        }
        else if(strcmp(subr2,"00000") == 0)   /*for c.li*/
        {
            outstr[15]='0';
            outstr[14]='1';
            outstr[13]='0';
            /*funct*/
            outstr[12]=subr1[6];
            /*imm*/
            for ( i = 0; i < 5; i++)
            {
                outstr[11-i]=subr3[i];
            }
            /*rd*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i]=subr1[7+i];
            }
            /*imm*/
        }
        outstr[1]='0';
        outstr[0]='1';
        /*opcode*/
    }
                
    else if(strcmp(subr4,"111") == 0)    /*for c.andi*/
        {
            outstr[15]='1';
            outstr[14]='0';
            outstr[13]='0';
            /*funct3*/
            outstr[12]=subr1[6];
            /*imm*/
            outstr[11]='1';
            outstr[10]='0';
            /*funct2*/
            for ( i = 0; i < 3; i++)
            {
                outstr[9-i]=subr3[2+i];
            }
            /*rd*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i]=subr1[7+i]; 
            }
            /*imm*/
            outstr[1]='0';
            outstr[0]='1';
            /*opcode*/
        }
                
        else if(strcmp(subr4,"010") == 0)    /*for c.lw*/
        {
            outstr[15]='0';
            outstr[14]='1';
            outstr[13]='0';
            /*funct3*/
            outstr[12] = subr1[6];
            outstr[11] = subr1[7];
            outstr[10] = subr1[8];
            /*imm*/
            outstr[9] = subr2[2];
            outstr[8] = subr2[3];
            outstr[7] = subr2[4];
            /*rs1*/
            outstr[6] = subr1[9];
            outstr[5] = subr1[5];
            /*imm*/
            outstr[4] = subr3[2];
            outstr[3] = subr3[3];
            outstr[2] = subr3[4];
            /*rd*/
            outstr[1]='0';
            outstr[0]='0';
            /*opcode*/
        }
        else if(strcmp(subr4,"001") == 0)   /*for c.slli*/
        {
            outstr[15]='0';
            outstr[14]='0';
            outstr[13]='0';
            /*funct3*/
            outstr[12]=s5;
            /*shamt5*/
            for ( i = 0; i < 5; i++)
            {
                outstr[11-i]=subr3[i];
            }
            /*rd*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i] = shamt5[i];
            }
            /*imm*/
            outstr[1]='1';
            outstr[0]='0';
        }
                
        else if(strcmp(subr4,"101") == 0)   /*for c.srli,c.srai*/
        {
            outstr[15] = '1';
            outstr[14] = '0';
            outstr[13] = '0';
            /*funct3*/
            outstr[12] = s5;
            /*shamt5*/
            outstr[11]='0';
           if (str[1] == '0')     /*c.srli*/
           {
                outstr[10] = '0';
           }else{               /*c.srai*/
                outstr[10] = '1';
           }
            /*funct2*/
            outstr[9]=subr3[2];
            outstr[8]=subr3[3];
            outstr[7]=subr3[4];
            /*rd*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i] = shamt5[i];
            }
            /*imm*/
            outstr[1]='0';
            outstr[0]='1';
            /*opcode*/
        }
}

/* c.lui*/
void branch2(char* str,char* outstr){
    /*create temp*/
    int i;
    char subr1[50],subr3[50];
    /*initial temp*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr3,'\0',sizeof(subr3));
    strncpy(subr1,str,20);      /* get immediate(12bit - 31bit)*/
    strncpy(subr3,str + 20,5);  /* get rd(7bit - 11bit)*/
    outstr[15]='0';
    outstr[14]='1';
    outstr[13]='1';
    /*funct3*/
    outstr[12]=subr1[14];
    /*imm*/
    for ( i = 0; i < 5; i++)
    {
        outstr[11-i]=subr3[i];
    }
    /*rd*/
    for ( i = 0; i < 5; i++)
    {
        outstr[6-i]=subr1[15+i];
    }
    /*imm*/
    outstr[1]='0';
    outstr[0]='1';
}


/* for c.jal c.j*/
void branch3(char* str,char* outstr,int jb[50][50],int po){
    /*create temp*/
    int dum,target,numbyte;
    char subr1[50],subr3[50];
    /*initial temp*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr3,'\0',sizeof(subr3));
    strncpy(subr3,str + 20,5);  /* get rd(7bit - 11bit)*/
    memcpy(subr1,str,1);
    /*copy imm[20]*/
    memcpy(subr1+10,str+1,10);
    /*copy imm[10:1]*/
    memcpy(subr1+9,str+11,1);
    /*copy imm[11]*/
    memcpy(subr1+1,str+12,8);
    /*copy imm[19:12]*/
    dum=convert_int(subr1,20);
    dum=dum/2;
    /*number of jump instruction*/
    target=po+dum;
    /*target instruction*/
    numbyte=jb[po][target];
    numbyte=numbyte/2;
    if (target<po){numbyte=-numbyte;}
    
    /*get byte*/
    if (strcmp(subr3,"00000") == 0)       /*c.j*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='1';
    }else{                      /*c.jal*/
        outstr[15]='0';
        outstr[14]='0';
        outstr[13]='1';
    }
    /*funct3*/
    outstr[12]=bechar((numbyte>>10)&1);
    outstr[11]=bechar((numbyte>>3)&1);
    outstr[10]=bechar((numbyte>>8)&1);
    /*imm*/
    outstr[9]=bechar((numbyte>>7)&1);
    outstr[8]=bechar((numbyte>>9)&1);
    outstr[7]=bechar((numbyte>>5)&1);
    /*imm*/
    outstr[6]=bechar((numbyte>>6)&1);
    outstr[5]=bechar((numbyte>>2)&1);
    outstr[4]=bechar((numbyte>>1)&1);
    /*imm*/
    outstr[3]=bechar((numbyte>>0)&1);
    outstr[2]=bechar((numbyte>>4)&1);
    /*imm*/
    outstr[1]='0';
    outstr[0]='1';
}
