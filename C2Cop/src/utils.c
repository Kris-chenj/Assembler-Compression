#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"                /*import*/

/* Your code here... */

static int convert_int(const char* str,int n)   /*get the DEC number of it*/
{
    long int ans = strtol(str,NULL,2);
    
    if(str[0] == '0' || n == 5){return ans;}     /*assess*/
    else
    {
        if(n == 12){ans = (0xfffff000 | ans);}
        else if(n == 20){ans = (0xfff00000 | ans);}     /*assess*/
    }
    return ans;
}

static char bechar(int a){      /*convert 1,0 to char*/
    if (a==1)
    {
        return '1';
    }
    return '0';             /*get char*/
    }
    
/* for c.jalr,c.jr*/
void branch4(char* str,char* outstr){
    int i;
    char subr2[50],subr3[50];        /*initilize*/
    memset(subr2,'\0',sizeof(subr2));
    memset(subr3,'\0',sizeof(subr3));
    strncpy(subr2,str + 12,5);  /* get rs1(15bit - 19bit)*/
    strncpy(subr3,str + 20,5);  /* get rd(7bit - 11bit)*/
    if(strcmp(subr3,"00000") == 0)        /*for c.jr*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';
        outstr[12]='0';         /*func4*/
        for ( i = 0; i < 5; i++)
        {
            outstr[11-i]=subr2[i];
        }                       /*rd/rs1*/
        outstr[6]='0';
        outstr[5]='0';
        outstr[4]='0';
        outstr[3]='0';         
        outstr[2]='0';          /*func4*/
        outstr[1]='1';
        outstr[0]='0';          /*opcode*/
    }
    else if(strcmp(subr3,"00001") == 0)   /*for c.jalr*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';
        outstr[12]='1';         /*func4*/
        for ( i = 0; i < 5; i++)
        {
            outstr[11-i]=subr2[i];
        }                       /*rd/rs1*/
        outstr[6]='0';
        outstr[5]='0';
        outstr[4]='0';
        outstr[3]='0';         
        outstr[2]='0';          /*func4*/
        outstr[1]='1';
        outstr[0]='0';          /*opcode*/
    }
}


/* for c.add,c.mv,c.and,c.or,c.xor,c.sub*/
void branch5(char* str,char* outstr){
    int i;
    char subr1[50],subr2[50],subr3[50],subr4[50],subr5[50];   /*intilize*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr2,'\0',sizeof(subr2));
    memset(subr3,'\0',sizeof(subr3));   /*intilize*/
    memset(subr4,'\0',sizeof(subr4));
    memset(subr5,'\0',sizeof(subr5));
    strncpy(subr1,str,7);       /* get funct7(25bit - 31bit)*/
    strncpy(subr2,str + 17,3);  /* get funct3(12bit - 14bit)*/
    strncpy(subr3,str + 12,5);  /* get rs1(15bit - 19bit)*/
    strncpy(subr4,str + 7,5);   /* get rs2(20bit - 24bit)*/
    strncpy(subr5,str + 20,5);  /* get rd(7bit - 11bit)*/
    if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"000") == 0)  /*for c.add c.mv*/
    {
        if(strcmp(subr3,subr5) == 0)                    /*for c.add*/
        {
            outstr[15]='1';
            outstr[14]='0';
            outstr[13]='0';
            outstr[12]='1';         /*func4*/
            for ( i = 0; i < 5; i++)
            {
                outstr[11-i]=subr3[i];
            }                       /*rd/rs1*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i]=subr4[i];
            }                       /*rs2*/
            outstr[1]='1';
            outstr[0]='0';          /*opcode*/
        }
        else if(strcmp(subr3,"00000") == 0)             /*for c.mv*/
        {
            outstr[15]='1';
            outstr[14]='0';
            outstr[13]='0';
            outstr[12]='0';         /*func4*/
            for ( i = 0; i < 5; i++)
            {
                outstr[11-i]=subr5[i];
            }                       /*rd/rs1*/
            for ( i = 0; i < 5; i++)
            {
                outstr[6-i]=subr4[i];
            }                       /*rs2*/
            outstr[1]='1';
            outstr[0]='0';          /*opcode*/
        }
    }
    else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"111") == 0)  /*for c.and*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';             /*func6*/
        outstr[12]='0';         
        outstr[11]='1';
        outstr[10]='1';             /*func6*/
        for ( i = 0; i < 3; i++)
        {
            outstr[9-i]=subr5[i+2];
        }                           /*rd'/rs1'*/
        outstr[6]='1';
        outstr[5]='1';              /*func2*/
        for ( i = 0; i < 3; i++)
        {
            outstr[4-i]=subr4[i+2];
        }                           /*rs2'*/
        outstr[1]='0';
        outstr[0]='1';              /*opcode*/
    }
    else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"110") == 0)  /*for or*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';              /*func6*/
        outstr[12]='0';         
        outstr[11]='1';
        outstr[10]='1';             /*func6*/
        for ( i = 0; i < 3; i++)
        {
            outstr[9-i]=subr5[i+2];
        }                           /*rd'/rs1'*/
        outstr[6]='1';
        outstr[5]='0';              /*func2*/
        for ( i = 0; i < 3; i++)
        {
            outstr[4-i]=subr4[i+2];
        }                           /*rs2'*/
        outstr[1]='0';
        outstr[0]='1';              /*opcode*/
    }
    else if(strcmp(subr1,"0000000") == 0 && strcmp(subr2,"100") == 0)  /*for xor*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';              /*func6*/
        outstr[12]='0';         
        outstr[11]='1';
        outstr[10]='1';             /*func6*/
        for ( i = 0; i < 3; i++)
        {
            outstr[9-i]=subr5[i+2];
        }                           /*rd'/rs1'*/
        outstr[6]='0';
        outstr[5]='1';              /*func2*/
        for ( i = 0; i < 3; i++)
        {
            outstr[4-i]=subr4[i+2];
        }                           /*rs2'*/
        outstr[1]='0';
        outstr[0]='1';              /*opcode*/
    }
    else if(strcmp(subr1,"0100000") == 0 && strcmp(subr2,"000") == 0)  /*for sub*/
    {
        outstr[15]='1';
        outstr[14]='0';
        outstr[13]='0';             /*func6*/
        outstr[12]='0';         
        outstr[11]='1';
        outstr[10]='1';             /*func6*/
        for ( i = 0; i < 3; i++)
        {
            outstr[9-i]=subr5[i+2];
        }                           /*rd'/rs1'*/
        outstr[6]='0';
        outstr[5]='0';              /*func2*/
        for ( i = 0; i < 3; i++)
        {
            outstr[4-i]=subr4[i+2];
        }                           /*rs2'*/
        outstr[1]='0';
        outstr[0]='1';              /*opcode*/
    }
}


/*for c.sw*/
void branch6(char* str,char* outstr){
    char subr1[50],subr2[50],subr3[50],subr5[50];
    /*initial temp*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr2,'\0',sizeof(subr2));
    memset(subr3,'\0',sizeof(subr3));    /*initial temp*/
    memset(subr5,'\0',sizeof(subr5));
    strncpy(subr1,str,7);           /* get immediate[11:5]*/
    strncpy(subr2,str + 12,5);      /* get rs1(15bit - 19bit)*/
    strncpy(subr3,str + 20,5);      /* get immediate[4:0]*/
    strncpy(subr5,str + 7,5);       /* get rs2(20bit - 24bit)*/

    outstr[15]='1';
    outstr[14]='1';
    outstr[13]='0';                 /*funct3*/

    outstr[12]=subr1[6];
    outstr[11]=subr3[0];
    outstr[10]=subr3[1];            /*imm[5:3]*/

    outstr[9]=subr2[2];
    outstr[8]=subr2[3];
    outstr[7]=subr2[4];             /*rs1'*/

    outstr[6]=subr3[2];
    outstr[5]=subr1[5];             /*imm[2|6]*/

    outstr[4]=subr5[2];
    outstr[3]=subr5[3];
    outstr[2]=subr5[4];             /*rs2'*/

    outstr[1]='0';
    outstr[0]='0';                  /*opcode*/
}

/*for c.beqz,c.bnez*/
void branch7(char* str,char* outstr,int jb[50][50],int po){
    int dum,target,numbyte;
    char subr1[50],subr2[50],subr3[50],subr4[50],subr5[50];
    memset(subr1,'\0',sizeof(subr1));
    memset(subr2,'\0',sizeof(subr2));    /*initial temp*/
    memset(subr3,'\0',sizeof(subr3));
    memset(subr4,'\0',sizeof(subr4));
    memset(subr5,'\0',sizeof(subr5));
    strncpy(subr2,str + 17,3);  /* get funct3(12bit - 14bit)*/
    strncpy(subr3,str + 12,5);  /* get rs1(15bit - 19bit)*/
    strncpy(subr4,str + 7,5);   /* get rs2(20bit - 24bit)*/
    memcpy(subr1,str,1);
    /*copy imm[12]*/
    memcpy(subr1+2,str+1,6);
    /*copy imm[10:5]*/
    memcpy(subr1+8,str+20,4);
    /*copy imm[4:1]*/
    memcpy(subr1+1,str+24,1);
    /*copy imm[11]*/
    dum=convert_int(subr1,12);
    dum=dum/2;
    /*number of jump instruction*/
    target=po+dum;
    /*target instruction*/
    numbyte=jb[po][target];
    numbyte=numbyte/2;
    if (target<po){numbyte=-numbyte;}
    /*get byte*/
    if(strcmp(subr2,"000") == 0)          /* for c.beqz*/
    {
        outstr[15]='1';
        outstr[14]='1';
        outstr[13]='0';                 /*funct3*/
    }
    else if(strcmp(subr2,"001") == 0)     /* for c.bnez*/
    {
        outstr[15]='1';
        outstr[14]='1';
        outstr[13]='1';                 /*funct3*/
    }
    outstr[12]=bechar((numbyte>>7)&1);
    outstr[11]=bechar((numbyte>>3)&1);
    outstr[10]=bechar((numbyte>>2)&1);  /*IMM*/

    outstr[9]=subr3[2];
    outstr[8]=subr3[3];
    outstr[7]=subr3[4];                 /*rs1'*/

    outstr[6]=bechar((numbyte>>6)&1);
    outstr[5]=bechar((numbyte>>5)&1);   /*IMM*/
    outstr[4]=bechar((numbyte>>1)&1);  
    outstr[3]=bechar(numbyte&1);
    outstr[2]=bechar((numbyte>>4)&1);  /*IMM*/

    outstr[1]='0';
    outstr[0]='1';                      /*opcode*/
}
/*for lw*/
void branch8(char* str,char* outstr){
    /*create temp*/
    char subr1[50],subr2[50],subr3[50];
    /*initial temp*/
    memset(subr1,'\0',sizeof(subr1));
    memset(subr2,'\0',sizeof(subr2));
    memset(subr3,'\0',sizeof(subr3));
    strncpy(subr1,str,12);          /* get immediate(20bit - 31bit)*/
    strncpy(subr2,str + 12,5);      /* get rs1(15bit - 19bit)*/
    strncpy(subr3,str + 20,5);      /* get rd(7bit - 11bit)*/
    outstr[15]='0';
    outstr[14]='1';
    outstr[13]='0';
    /*funct3*/
    outstr[12]=subr1[6];
    outstr[11]=subr1[7];
    outstr[10]=subr1[8];
    /*imm*/
    outstr[9]=bechar(((convert_int(subr2,5)-8)>>2)&1);
    outstr[8]=bechar(((convert_int(subr2,5)-8)>>1)&1);
    outstr[7]=bechar((convert_int(subr2,5)-8)&1);
    /*rs1*/
    outstr[6]=subr1[9];
    outstr[5]=subr1[5];
    /*imm*/
    outstr[4]=bechar(((convert_int(subr3,5)-8)>>2)&1);
    outstr[3]=bechar(((convert_int(subr3,5)-8)>>1)&1);
    outstr[2]=bechar((convert_int(subr3,5)-8)&1);
    /*rd*/
    outstr[1]='0';
    outstr[0]='0';
    /*opcode*/
}

void branch9(char* str,char* outstr,int jb[50][50],int po)
{
    int i = 0;
    int dum,target,numbyte;          /*initial temp*/
    char temp[50],subr1[50];
    memset(temp,'\0',sizeof(temp));
    memset(subr1,'\0',sizeof(subr1));    /*initial temp*/
    strncpy(temp,str + 25,7);
    for ( i = 0; i < 32; i++)            /*restore*/
    {
        outstr[i] = str[i];
    }
    if(strcmp(temp,"1101111") == 0)              /* for jal*/
    {
        memset(subr1,'\0',sizeof(subr1));
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
        outstr[0] = bechar((numbyte>>19)&1);    /*set bit*/
        for(i = 0;i<10;++i)
        {
            outstr[1+i] = bechar((numbyte>>(9-i))&1);
        }
        outstr[11] = bechar((numbyte>>10)&1);   /*set bit*/
        outstr[12] = bechar((numbyte>>18)&1);
        outstr[13] = bechar((numbyte>>17)&1);
        outstr[14] = bechar((numbyte>>16)&1);   /*set bit*/
        outstr[15] = bechar((numbyte>>15)&1);
        outstr[16] = bechar((numbyte>>14)&1);
        outstr[17] = bechar((numbyte>>13)&1);   
        outstr[18] = bechar((numbyte>>12)&1);   /*set bit*/
        outstr[19] = bechar((numbyte>>11)&1);
    }

    else if(strcmp(temp,"1100011") == 0)              /*for beq,bne*/
    {
        memcpy(subr1,str,1);
        /*copy imm[12]*/
        memcpy(subr1+2,str+1,6);
        /*copy imm[10:5]*/
        memcpy(subr1+8,str+20,4);
        /*copy imm[4:1]*/
        memcpy(subr1+1,str+24,1);
        /*copy imm[11]*/
        dum=convert_int(subr1,12);
        dum=dum/2;
        /*number of jump instruction*/
        target=po+dum;
        /*target instruction*/
        numbyte=jb[po][target];
        numbyte=numbyte/2;
        if (target<po){numbyte=-numbyte;}
        /*get byte*/
        outstr[0] = bechar((numbyte>>11)&1);
        for ( i = 0; i < 6; i++)
        {
            outstr[1+i] = bechar((numbyte>>(9-i))&1);    /*set bit*/
        }
        
        outstr[20] = bechar((numbyte>>3)&1);
        outstr[21] = bechar((numbyte>>2)&1);             /*set bit*/
        outstr[22] = bechar((numbyte>>1)&1);
        outstr[23] = bechar((numbyte>>0)&1);
        outstr[24] = bechar((numbyte>>10)&1);            /*set bit*/
    }
}