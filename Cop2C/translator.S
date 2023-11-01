#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.

# main 
.text
main:
    # load number of lines, store in a4
    la a4,lines_of_rvc_codes
    lw a4,0(a4)

    # load specficial data, store head pointer in s0
    la s0,rvc_codes

    # set gap
    li s1,0

    # record iteration number
    li s2,0

    # create space
    li a0,0x3CC
    mv a1,a4
    li a2,4
    li a6,2
    ecall

    # let space head to a5
    mv a5,a0

temporary:
    # assess which instrution is 16 or 32
    # iteration number
    li t4,0
    # s0(head pointer)
    mv t5,s0
    # gap s1
    li t6,0
    # temporary space head s10
    mv s10,a5

init_loop:
    # assess whether stop
    beq a4,t4,outloop

    # update t5
    add t5,t5,t6

    # load one line data , store in t0
    lhu t0,0(t5)

    # assess whether its 32b or 16b
    li t2,3
    li t1,0b00000000000000000000000000000011
    and t1,t0,t1

    # if its 32bit
    beq t1,t2,if_32

    # if its 16bit
    li t6,2
    addi t4,t4,1
    addi s10,s10,4

    # end_loop
    j init_loop


outloop:
    # assess whether stop
    beq a4,s2,Exit

    # update s0
    add s0,s0,s1

    # load one line data , store in t0
    lhu t0,0(s0)

    # assess whether its 32b or 16b
    li t2,3
    li t1,0b00000000000000000000000000000011
    and t1,t0,t1

    # if its 32bit
    beq t1,t2,Get_32

    # if its 16bit(store in t0,last 16 bit work)
    li t2,1                                   # last OPCODE is 01
    li t3,2                                   # last OPCODE is 10

    li s3,0b00000000000000001110000000000000  
    and s3,t0,s3                              # get front 3bit  in s3

    li s4,0b00000000000000001111000000000000  
    and s4,t0,s4                              # get front 4bit  in s4

    li s5,0b00000000000000000000000001100000  
    and s5,t0,s5                              # get mid 2bit(CA fun2)  in s5

    li s6,0b00000000000000001111110000000000  
    and s6,t0,s6                              # get front 6bit  in s6

    li s7,0b00000000000000000000110000000000 
    and s7,t0,s7                              # get mid 2bit(CB_type2 fun2)  in s7

    li s8,0b00000000000000000000000001111100  
    and s8,t0,s8                              # get mid 5bit(CR Rs2)  in s8

assess_op00:
    beq t1,x0,OPCode_00                       # if last OPCODE is 00
assess_op01:
    beq t1,t2,OPCode_01                       # if last OPCODE is 01
assess_op10:
    beq t1,t3,OPCode_10                       # if last OPCODE is 10


End_once_loop_by16_expand:
    # print origin instruction
    jal ra,PRINT_32

    # update gap s1
    li s1,2

    # update iterate number s2
    addi s2,s2,1
    
    # restart outloop
    j outloop




Get_32:
    # reget entire 32b instruction
    lw a3,0(s0)

    # assess whether regulate the jump byte
    li t1,0b00000000000000000000000001111111
    li t2,0b00000000000000000111000000000000
    and t1,a3,t1                              # get last seven bit to t1
    and t2,a3,t2                              
    srli t2,t2,12                             # get 12-14 bit to t2(fun3)
    li t6,0b00000000000000000000000001101111  # jal opcode
    li t5,0b00000000000000000000000001100011  # beq,bne opcode
    li t4,1                                   # beq,bne function 3

    # store ra
    addi sp,sp,4
    sw ra,0(sp)

    # assess whether go to change
    beq t1,t6,j_change_jump                   # suit to jal
    beq t1,t5,whe_jump_passed_opcode_same     # beq,bne opcode suit

Exit_assess:
    #restore ra
    lw ra,0(sp)
    #release stack
    addi sp,sp,-4
    
    # print origin instruction
    jal ra,PRINT_32

    # update gap s1
    li s1,4

    # update iterate number s2
    addi s2,s2,1
    
    # restart outloop
    j outloop



PRINT_32:
    # print result store in a3
    li t5,0
    li t6,32
    li t4,0b10000000000000000000000000000000

    # print number
    li a0,1
    # set move number
    li t2,31

loop_32:
    # assess
    beq t5,t6,jout_32
    # get each number
    and a1,a3,t4
    srl a1,a1,t2
    ecall
    # update move number
    addi t2,t2,-1
    # update t4
    srli t4,t4,1
    # update t5
    addi t5,t5,1
    j loop_32

jout_32:
    # set and print \n
    li a0,11
    li a1,10
    ecall
    jr ra

whe_jump_passed_opcode_same:
    beq t2,x0,b_change_jump                    # suit to beq
    beq t2,t4,b_change_jump                    # suit to bne
    j Exit_assess                              # jump back



j_change_jump:
    # get imm to t4
    li t4,0b01111111111000000000000000000000
    and t4,a3,t4
    # set [10:1]
    srli t4,t4,20
    # get[11]
    li t5,0b00000000000100000000000000000000
    and t5,a3,t5
    # set [11]
    srli t5,t5,9
    or t4,t4,t5
    # get [19:12]
    li t5,0b00000000000011111111000000000000
    and t5,a3,t5
    # set [19:12]
    or t4,t4,t5
    # get [20]
    li t5,0b10000000000000000000000000000000
    and t5,a3,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,31
    # set [20]
    srli t5,t5,11
    or t4,t4,t5

    # set others
    li t5,0b00000000000000000000111111111111
    and a3,a3,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,posi_j

    # else nega_change
    # sign extend
    li t5,0b11111111111000000000000000000000
    or t4,t4,t5

    addi s11,s2,-1
    j nega_j

b_change_jump:
    # get imm to t4
    li t4,0b00000000000000000000111100000000
    and t4,a3,t4
    # set [4:1]
    srli t4,t4,7
    # get [10:5]
    li t5,0b01111110000000000000000000000000
    and t5,a3,t5
    # set [10:5]
    srli t5,t5,20
    or t4,t4,t5
    # get[11]
    li t5,0b00000000000000000000000010000000
    and t5,a3,t5
    # set [11]
    slli t5,t5,4
    or t4,t4,t5
    # get [12]
    li t5,0b10000000000000000000000000000000
    and t5,a3,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,31
    # set [12]
    srli t5,t5,19
    or t4,t4,t5

    # set others
    li t5,0b00000001111111111111000001111111
    and a3,a3,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,posi_b

    # else nega_change
    # sign extend
    li t5,0b11111111111111111110000000000000
    or t4,t4,t5

    addi s11,s2,-1
    j nega_b

posi_j:
    beq t4,x0,w_j
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,j_32_d2
    # if 1
    j j_32_d4

nega_j:
    beq t4,x0,w_j_n
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,j_32_i2
    # if 1
    j j_32_i4

posi_b:
    beq t4,x0,w_b
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,b_32_d2
    # if 1
    j b_32_d4

nega_b:
    beq t4,x0,w_b_n
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,b_32_i2
    # if 1
    j b_32_i4

j_32_d4:
    # update 
    addi a7,a7,1
    addi t4,t4,-4
    # u
    addi s11,s11,1
    # l
    j posi_j
j_32_d2:
    # update 
    addi a7,a7,1
    addi t4,t4,-2
    # u
    addi s11,s11,1
    # l
    j posi_j
j_32_i4:
    # update 
    addi a7,a7,1
    addi t4,t4,4
    # u
    addi s11,s11,-1
    # l
    j nega_j
j_32_i2:
    # update 
    addi a7,a7,1
    addi t4,t4,2
    # u
    addi s11,s11,-1
    # l
    j nega_j

b_32_d4:
    # update 
    addi a7,a7,1
    addi t4,t4,-4
    # u
    addi s11,s11,1
    # l
    j posi_b
b_32_d2:
    # update 
    addi a7,a7,1
    addi t4,t4,-2
    # u
    addi s11,s11,1
    # l
    j posi_b
b_32_i4:
    # update 
    addi a7,a7,1
    addi t4,t4,4
    # u
    addi s11,s11,-1
    # l
    j nega_b
b_32_i2:
    # update 
    addi a7,a7,1
    addi t4,t4,2
    # u
    addi s11,s11,-1
    # l
    j nega_b

w_j:
    # get new off
    li t4,4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000011111111110
    and t4,t4,t5
    # [10:1]
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    slli t4,t4,9
    or a3,a3,t4
    # [19:12]
    li t4,0b00000000000011111111000000000000
    and t4,t4,t5
    or a3,a3,t4
    # [20]
    li t4,0b00000000000100000000000000000000
    and t4,t4,t5
    slli t4,t4,11
    or a3,a3,t4

    # jump back
    j Exit_assess

w_j_n:
    # get new off
    li t4,-4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000011111111110
    and t4,t4,t5
    # [10:1]
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    slli t4,t4,9
    or a3,a3,t4
    # [19:12]
    li t4,0b00000000000011111111000000000000
    and t4,t4,t5
    or a3,a3,t4
    # [20]
    li t4,0b00000000000100000000000000000000
    and t4,t4,t5
    slli t4,t4,11
    or a3,a3,t4

    # jump back
    j Exit_assess

w_b:
    # get new off
    li t4,4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000000000011110
    and t4,t4,t5
    # [4:1]
    slli t4,t4,7
    or a3,a3,t4
    # [10:5]
    li t4,0b00000000000000000000011111100000
    and t4,t4,t5
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    srli t4,t4,4
    or a3,a3,t4
    # [12]
    li t4,0b00000000000000000001000000000000
    and t4,t4,t5
    slli t4,t4,19
    or a3,a3,t4

    # print
    j Exit_assess

w_b_n:
    # get new off
    li t4,-4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000000000011110
    and t4,t4,t5
    # [4:1]
    slli t4,t4,7
    or a3,a3,t4
    # [10:5]
    li t4,0b00000000000000000000011111100000
    and t4,t4,t5
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    srli t4,t4,4
    or a3,a3,t4
    # [12]
    li t4,0b00000000000000000001000000000000
    and t4,t4,t5
    slli t4,t4,19
    or a3,a3,t4

    # print
    j Exit_assess

OPCode_00:
    # assess type

    li t6,0b00000000000000000100000000000000
    beq t6,s3,C_lw        # if type is c.lw

    li t6,0b00000000000000001100000000000000
    beq t6,s3,C_sw        # if type is c.sw

    # if not ,jump back
    j assess_op01


OPCode_01:
    # assess type

    beq x0,s3,C_addi                # if type is c.addi

    li t6,0b00000000000000000100000000000000
    beq t6,s3,C_li                  # if type is c.li

    li t6,0b00000000000000000110000000000000
    beq t6,s3,C_lui                 # if type is c.lui

    li t6,0b00000000000000001100000000000000
    beq t6,s3,C_beqz                # if type is c.beqz

    li t6,0b00000000000000001110000000000000
    beq t6,s3,C_bnez                # if type is c.bnez

    li t6,0b00000000000000001010000000000000
    beq t6,s3,C_j                   # if type is c.j

    li t6,0b00000000000000000010000000000000
    beq t6,s3,C_jal                 # if type is c.jal

    li t6,0b00000000000000001000000000000000
    beq t6,s3,C_srli_srai_andi      # if type is c.srli or c.srai or c.andi

    li t6,0b00000000000000001000110000000000
    beq t6,s6,C_and_or_xor_sub      # if type is c.and or c.or or c.xor or c.sub

    # if not ,jump back
    j assess_op10

OPCode_10:
    # assess type

    beq x0,s3,C_slli                # if type is c.slli

    li t6,0b00000000000000001000000000000000
    beq t6,s4,C_mv_or_C_jr          # if type is C.mv or C.jr

    li t6,0b00000000000000001001000000000000
    beq t6,s4,C_add_or_C_jalr       # if type is C.add or C.jalr

C_mv_or_C_jr:
    # assess type

    beq s8,x0,C_jr                  # if type is C.jr
    jal C_mv                        # if type is C.mv

C_add_or_C_jalr:
    # assess type

    beq s8,x0,C_jalr                # if type is C.jalr
    jal C_add                       # if type is C.add

C_srli_srai_andi:
    # assess type

    beq s7,x0,C_srli                # if type is C.srli

    li t6,0b00000000000000000000010000000000
    beq s7,t6,C_srai                # if type is C.srai

    li t6,0b00000000000000000000100000000000
    beq s7,t6,C_andi                # if type is C.andi

    # is "11" possible? may wrong*******

C_and_or_xor_sub:
    # assess type

    beq s5,x0,C_sub                 # if type is C.sub

    li t6,0b00000000000000000000000001100000
    beq s5,t6,C_and                 # if type is C.and

    li t6,0b00000000000000000000000001000000
    beq s5,t6,C_or                  # if type is C.or

    li t6,0b00000000000000000000000000100000
    beq s5,t6,C_xor                 # if type is C.xor





# Expand 16 bit instrutions ,and store answer to a3!!!

# R type
C_add:
    # initilize a3
    li a3,0b00000000000000000000000000110011
    # get rs2
    li t4,0b00000000000000000000000001111100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rs1/rd
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand

C_mv:
    # initilize a3
    li a3,0b00000000000000000000000000110011
    # get rs2
    li t4,0b00000000000000000000000001111100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rd
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand

C_and:
    # initilize a3
    li a3,0b00000000100001000111010000110011
    # get rs2'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rd'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand

C_or:
    # initilize a3
    li a3,0b00000000100001000110010000110011
    # get rs2'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rd'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand

C_sub:
    # initilize a3
    li a3,0b01000000100001000000010000110011
    # get rs2'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rd'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand

C_xor:
    # initilize a3
    li a3,0b00000000100001000100010000110011
    # get rs2'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    slli t4,t4,18
    # set rs2
    or a3,a3,t4
    # get rd'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5

    # end and print
    j End_once_loop_by16_expand


# I type
C_addi:
    # initilize a3
    li a3,0b00000000000000000000000000010011
    # get rd/rs1
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get rs1
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    # get nzimm[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set nzimm[4:0]
    or a3,a3,t5
    # sign extend
    li t4,0b00000000000000000001000000000000
    # get nzimm[17]
    and t4,t0,t4
    bne t4,x0,front7_sign_extend
 
    # end and print
    j End_once_loop_by16_expand
    
C_andi:
    # initilize a3
    li a3,0b00000000000001000111010000010011
    # get rd'/rs1'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get rs1
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    # get imm[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set imm[4:0]
    or a3,a3,t5
    # sign extend
    li t4,0b00000000000000000001000000000000
    # get imm[5]
    and t4,t0,t4
    bne t4,x0,front7_sign_extend
 
    # end and print
    j End_once_loop_by16_expand
    
C_li:
    # initilize a3
    li a3,0b00000000000000000000000000010011
    # get rd
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get nzimm[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set nzimm[4:0]
    or a3,a3,t5
    # sign extend
    li t4,0b00000000000000000001000000000000
    # get imm5
    and t4,t0,t4
    bne t4,x0,front7_sign_extend
 
    # end and print
    j End_once_loop_by16_expand
    
C_jr:
    # initilize a3
    li a3,0b00000000000000000000000001100111
    # get rs1
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    
    # end and print
    j End_once_loop_by16_expand
    
C_jalr:
    # initilize a3
    li a3,0b00000000000000000000000011100111
    # get rs1
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
 
    # end and print
    j End_once_loop_by16_expand
    
C_slli:
    # initilize a3
    li a3,0b00000000000000000001000000010011
    # get rd/rs1
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get rs1
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    # get shamt[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set shamt[4:0]
    or a3,a3,t5
    # end and print
    j End_once_loop_by16_expand
    
C_srli:
    # initilize a3
    li a3,0b00000000000001000101010000010011
    # get rd'/rs1'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get rs1
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    # get shamt[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set shamt[4:0]
    or a3,a3,t5
 
    # end and print
    j End_once_loop_by16_expand
    
C_srai:
    # initilize a3
    li a3,0b01000000000001000101010000010011
    # get rd'/rs1'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get rs1
    slli t5,t5,8
    # set rs1
    or a3,a3,t5
    # get shamt[4:0]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,18
    # set shamt[4:0]
    or a3,a3,t5
 
    # end and print
    j End_once_loop_by16_expand
    
C_lw:
    # initilize a3
    li a3,0b00000000000001000010010000000011
    # get rd'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    # set rd
    or a3,a3,t4
    # get rs1'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rs1
    slli t5,t5,8
    or a3,a3,t5

    # get offset[2]
    li t4,0b00000000000000000000000001000000
    and t4,t0,t4
    # set offset[2]
    slli t4,t4,16
    or a3,a3,t4

    # get offset[5:3]
    li t4,0b00000000000000000001110000000000
    and t4,t0,t4
    # set offset[5:3]
    slli t4,t4,13
    or a3,a3,t4

    # get offset[6]
    li t4,0b00000000000000000000000000100000
    and t4,t0,t4
    # set offset[6]
    slli t4,t4,21
    or a3,a3,t4
    
    # end and print
    j End_once_loop_by16_expand
    

# S type
C_sw:
    # initilize a3
    li a3,0b00000000100001000010000000100011
    # get rs2'
    li t4,0b00000000000000000000000000011100
    and t4,t0,t4
    # set rs2
    slli t4,t4,18
    or a3,a3,t4
    # get rs1'
    li t5,0b00000000000000000000001110000000
    and t5,t0,t5
    # set rs1
    slli t5,t5,8
    or a3,a3,t5

    # get offset[2]
    li t4,0b00000000000000000000000001000000
    and t4,t0,t4
    # set offset[2]
    slli t4,t4,3
    or a3,a3,t4

    # get offset[4:3]
    li t4,0b00000000000000000000110000000000
    and t4,t0,t4
    # set offset[4:3]
    or a3,a3,t4

    # get offset[5]
    li t4,0b00000000000000000001000000000000
    and t4,t0,t4
    # set offset[5]
    slli t4,t4,13
    or a3,a3,t4

    # get offset[6]
    li t4,0b00000000000000000000000000100000
    and t4,t0,t4
    # set offset[6]
    slli t4,t4,21
    or a3,a3,t4
 
    # end and print
    j End_once_loop_by16_expand
    

# SB type
C_beqz:
    # initilize a3
    li a3,0b00000000000001000000000001100011
    # get rs1'
    li t4,0b00000000000000000000001110000000
    and t4,t0,t4
    slli t4,t4,8
    # set rs1
    or a3,a3,t4

    # get imm to t4
    li t4,0b00000000000000000000000000011000
    and t4,t0,t4
    # set [2:1]
    srli t4,t4,2
    # get [4:3]
    li t5,0b00000000000000000000110000000000
    and t5,t0,t5
    # set [4:3]
    srli t5,t5,7
    or t4,t4,t5
    # get[5]
    li t5,0b00000000000000000000000000000100
    and t5,t0,t5
    # set [5]
    slli t5,t5,3
    or t4,t4,t5
    # get [7:6]
    li t5,0b00000000000000000000000001100000
    and t5,t0,t5
    # set [7:6]
    srli t5,t5,1
    or t4,t4,t5
    # get[8]
    li t5,0b00000000000000000001000000000000
    and t5,t0,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,12
    # set [8]
    srli t5,t5,4
    or t4,t4,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,posi_change

    # else nega_change
    # sign extend
    li t5,0b11111111111111111111111000000000
    or t4,t4,t5

    addi s11,s2,-1
    j nega_change
    
C_bnez:
    # initilize a3
    li a3,0b00000000000001000001000001100011
    # get rs1'
    li t4,0b00000000000000000000001110000000
    and t4,t0,t4
    slli t4,t4,8
    # set rs1
    or a3,a3,t4

    # get imm to t4
    li t4,0b00000000000000000000000000011000
    and t4,t0,t4
    # set [2:1]
    srli t4,t4,2
    # get [4:3]
    li t5,0b00000000000000000000110000000000
    and t5,t0,t5
    # set [4:3]
    srli t5,t5,7
    or t4,t4,t5
    # get[5]
    li t5,0b00000000000000000000000000000100
    and t5,t0,t5
    # set [5]
    slli t5,t5,3
    or t4,t4,t5
    # get [7:6]
    li t5,0b00000000000000000000000001100000
    and t5,t0,t5
    # set [7:6]
    srli t5,t5,1
    or t4,t4,t5
    # get[8]
    li t5,0b00000000000000000001000000000000
    and t5,t0,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,12
    # set [8]
    srli t5,t5,4
    or t4,t4,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,posi_change

    # else nega_change
    # sign extend
    li t5,0b11111111111111111111111000000000
    or t4,t4,t5

    addi s11,s2,-1
    j nega_change

# U type
C_lui:
    # initilize a3
    li a3,0b00000000000000000000000000110111
    # get rd
    li t5,0b00000000000000000000111110000000
    and t5,t0,t5
    # set rd
    or a3,a3,t5
    # get nzimm[16:12]
    li t5,0b00000000000000000000000001111100
    and t5,t0,t5
    slli t5,t5,10
    # set nzimm[16:12]
    or a3,a3,t5
    # sign extend
    li t4,0b00000000000000000001000000000000
    # get nzimm[17]
    and t4,t0,t4
    bne t4,x0,lui_sign_extend
    # end and print
    j End_once_loop_by16_expand
    

# UJ type
C_j:
    # initilize a3
    li a3,0b00000000000000000000000001101111
         
    # get imm[11:1] to t4
    # get [3:1]
    li t4,0b00000000000000000000000000111000
    and t4,t0,t4
    # set [3:1]
    srli t4,t4,2
    # get [4]
    li t5,0b00000000000000000000100000000000
    and t5,t0,t5
    # set [4]
    srli t5,t5,7
    or t4,t4,t5
    # get [5]
    li t5,0b00000000000000000000000000000100
    and t5,t0,t5
    # set [5]
    slli t5,t5,3
    or t4,t4,t5
    # get [6]
    li t5,0b00000000000000000000000010000000
    and t5,t0,t5
    # set [6]
    srli t5,t5,1
    or t4,t4,t5
    # get [7]
    li t5,0b00000000000000000000000001000000
    and t5,t0,t5
    # set [7]
    slli t5,t5,1
    or t4,t4,t5
    # get [9:8]
    li t5,0b00000000000000000000011000000000
    and t5,t0,t5
    # set [9:8]
    srli t5,t5,1
    or t4,t4,t5
    # get [10]
    li t5,0b00000000000000000000000100000000
    and t5,t0,t5
    # set [10]
    slli t5,t5,2
    or t4,t4,t5
    # get [11]
    li t5,0b00000000000000000001000000000000
    and t5,t0,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,12
    # set [11]
    srli t5,t5,1
    or t4,t4,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,j_posi_change

    # else nega_change
    # sign extend
    li t5,0b11111111111111111111000000000000
    or t4,t4,t5

    addi s11,s2,-1
    j j_nega_change
    
C_jal:
    # initilize a3
    li a3,0b00000000000000000000000011101111

    # get imm[11:1] to t4
    # get [3:1]
    li t4,0b00000000000000000000000000111000
    and t4,t0,t4
    # set [3:1]
    srli t4,t4,2
    # get [4]
    li t5,0b00000000000000000000100000000000
    and t5,t0,t5
    # set [4]
    srli t5,t5,7
    or t4,t4,t5
    # get [5]
    li t5,0b00000000000000000000000000000100
    and t5,t0,t5
    # set [5]
    slli t5,t5,3
    or t4,t4,t5
    # get [6]
    li t5,0b00000000000000000000000010000000
    and t5,t0,t5
    # set [6]
    srli t5,t5,1
    or t4,t4,t5
    # get [7]
    li t5,0b00000000000000000000000001000000
    and t5,t0,t5
    # set [7]
    slli t5,t5,1
    or t4,t4,t5
    # get [9:8]
    li t5,0b00000000000000000000011000000000
    and t5,t0,t5
    # set [9:8]
    srli t5,t5,1
    or t4,t4,t5
    # get [10]
    li t5,0b00000000000000000000000100000000
    and t5,t0,t5
    # set [10]
    slli t5,t5,2
    or t4,t4,t5
    # get [11]
    li t5,0b00000000000000000001000000000000
    and t5,t0,t5
    # store sign_bit
    mv t6,t5
    srli t6,t6,12
    # set [11]
    srli t5,t5,1
    or t4,t4,t5

    # initilize num of mid_instru
    mv a7,x0

    # SET ORIGIN
    mv s11,s2
    # assess positive off or negative off
    beq t6,x0,j_posi_change

    # else nega_change
    # sign extend
    li t5,0b11111111111111111111000000000000
    or t4,t4,t5

    addi s11,s2,-1
    j j_nega_change

lui_sign_extend:
    # sign extend
    li t5,0b11111111111111100000000000000000
    # set in
    or a3,a3,t5
    # end and print
    j End_once_loop_by16_expand

front7_sign_extend:
    # sign extend
    li t5,0b11111110000000000000000000000000
    # set in
    or a3,a3,t5
    # end and print
    j End_once_loop_by16_expand

if_32:
    # put 1 in space
    li s9,1
    sw s9,0(s10)
    # update
    li t6,4
    addi t4,t4,1
    addi s10,s10,4
    # end_loop
    j init_loop

posi_change:
    beq t4,x0,write_back_sb
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,decrease_2
    # if 1
    j decrease_4

nega_change:
    beq t4,x0,write_back_sb_n
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,increase_2
    # if 1
    j increase_4

j_posi_change:
    beq t4,x0,write_back_j
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,j_decrease_2
    # if 1
    j j_decrease_4

j_nega_change:
    beq t4,x0,write_back_j_n
    # get space offset
    li t5,4
    mul s10,s11,t5
    add s10,a5,s10
    lw t5,0(s10)
    beq t5,x0,j_increase_2
    # if 1
    j j_increase_4

write_back_sb:
    # get new off
    li t4,4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000000000011110
    and t4,t4,t5
    # [4:1]
    slli t4,t4,7
    or a3,a3,t4
    # [10:5]
    li t4,0b00000000000000000000011111100000
    and t4,t4,t5
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    srli t4,t4,4
    or a3,a3,t4
    # [12]
    li t4,0b00000000000000000001000000000000
    and t4,t4,t5
    slli t4,t4,19
    or a3,a3,t4
    # print
    j End_once_loop_by16_expand

write_back_sb_n:
    # get new off
    li t4,-4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000000000011110
    and t4,t4,t5
    # [4:1]
    slli t4,t4,7
    or a3,a3,t4
    # [10:5]
    li t4,0b00000000000000000000011111100000
    and t4,t4,t5
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    srli t4,t4,4
    or a3,a3,t4
    # [12]
    li t4,0b00000000000000000001000000000000
    and t4,t4,t5
    slli t4,t4,19
    or a3,a3,t4
    # print
    j End_once_loop_by16_expand

write_back_j:
    # get new off
    li t4,4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000011111111110
    and t4,t4,t5
    # [10:1]
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    slli t4,t4,9
    or a3,a3,t4
    # [19:12]
    li t4,0b00000000000011111111000000000000
    and t4,t4,t5
    or a3,a3,t4
    # [20]
    li t4,0b00000000000100000000000000000000
    and t4,t4,t5
    slli t4,t4,11
    or a3,a3,t4
    # print
    j End_once_loop_by16_expand

write_back_j_n:
    # get new off
    li t4,-4
    mul t5,a7,t4
    # set in
    li t4,0b00000000000000000000011111111110
    and t4,t4,t5
    # [10:1]
    slli t4,t4,20
    or a3,a3,t4
    # [11]
    li t4,0b00000000000000000000100000000000
    and t4,t4,t5
    slli t4,t4,9
    or a3,a3,t4
    # [19:12]
    li t4,0b00000000000011111111000000000000
    and t4,t4,t5
    or a3,a3,t4
    # [20]
    li t4,0b00000000000100000000000000000000
    and t4,t4,t5
    slli t4,t4,11
    or a3,a3,t4
    # print
    j End_once_loop_by16_expand

decrease_2:
    # update 
    addi a7,a7,1
    addi t4,t4,-2
    # u
    addi s11,s11,1
    # l
    j posi_change

decrease_4:
    # update 
    addi a7,a7,1
    addi t4,t4,-4
    # u
    addi s11,s11,1
    # l
    j posi_change

increase_2:
    # update 
    addi a7,a7,1
    addi t4,t4,2
    # u
    addi s11,s11,-1
    # l
    j nega_change

increase_4:
    # update 
    addi a7,a7,1
    addi t4,t4,4
    # u
    addi s11,s11,-1
    # l
    j nega_change


j_decrease_2:
    # update 
    addi a7,a7,1
    addi t4,t4,-2
    # u
    addi s11,s11,1
    # l
    j j_posi_change

j_decrease_4:
    # update 
    addi a7,a7,1
    addi t4,t4,-4
    # u
    addi s11,s11,1
    # l
    j j_posi_change

j_increase_2:
    # update 
    addi a7,a7,1
    addi t4,t4,2
    # u
    addi s11,s11,-1
    # l
    j j_nega_change

j_increase_4:
    # update 
    addi a7,a7,1
    addi t4,t4,4
    # u
    addi s11,s11,-1
    # l
    j j_nega_change


Exit:
    # free
    li a0,0x3CC
    li a6,4
    mv a1,a5
    ecall
    # Exit with error code 0
    add	   a1, x0, x0
    addi   a0, x0, 17
    ecall