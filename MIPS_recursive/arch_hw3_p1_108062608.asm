.data
str1:		.asciiz "input a: "
str2:		.asciiz "input b: "
str3:		.asciiz "input c: "
str4:		.asciiz "result = "
str5:		.asciiz "going to end "
Pnewline: 	.asciiz "\n"
.text

Main:
	# declare a,b,c,d = 0
	add $s0, $zero, $zero
	add $s1, $zero, $zero
	add $s2, $zero, $zero
	add $s3, $zero, $zero
	
	# Prompt the user to enter int
	la $a0, str1	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
	       
	# Get the a and store in $s0
	li $v0, 5
	syscall
	add $s0, $zero, $v0
	
	
	# Prompt the user to enter int
	la $a0, str2	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
	           
	
	# Get the b and store in $s1
	li $v0, 5
	syscall
	add $s1, $zero, $v0
	
	
	# Prompt the user to enter int
	la $a0, str3	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
	           
	           
	# Get the c and store in $s2
	li $v0, 5
	syscall
	add $s2, $zero, $v0
	           	
	
	# Do abs_sub, give argument
	jal abs_sub
	move $s3, $v0
	
	# Prompt the user to enter int
	la $a0, str4	   # load address of string to print
	li $v0, 4	   # ready to print string / also set v0 t0 4
	syscall            # print
	
	# print the result

	#move $s3, $t3
	li $v0, 1
	move $a0, $s3
	syscall 
	
	j Exit    
	
abs_sub: # int abs_sub(int x, int y) 
	addi $sp, $sp, -4
	sw $ra, 0($sp)  
	
	jal mad
	move $t3, $v0          # get madd(a,c)

# find the large
	slt $t0, $t3, $s1      # large = (x >= y) ? x = b : y = madd(a,c); 
	beq $t0, $zero, assign_Large_abs
	add $t0, $zero, $s1    
	j keep_sub

assign_Large_abs :
	add $t0, $zero, $t3
	
# find the small
keep_sub :	
	slt $t1, $s1, $t3       # small = (x <= y) ? x : y;
	beq $t1, $zero, assign_Small_abs
	add $t1, $zero, $s1 
	j final

assign_Small_abs :
	add $t1, $zero, $t3
	
final:
	sub $t4, $t0, $t1	#return large - small
	move $v0, $t4
	
	lw $ra, 0($sp)  
	addi $sp, $sp, 4
	jr $ra
		
			
				
						
mad: # int madd(int x, int y), x = a, y = c
	
	move $t3, $zero  # ans = 0
	
# find the large
	slt $t0, $s2, $s0  # large = (x >= y) ? x = s0 : y = s2; use $t0
	beq $t0, $zero, assign_Large_mad
	move $t0, $s0 
	j keep_mad
	
assign_Large_mad :
	move $t0, $s2 


# find the small	
keep_mad :	
	slt $t1, $s0, $s2  	# small = (x <= y) ? x : y;
	beq $t1, $zero, assign_Small_mad
	move $t1, $s0   
	j Accumulate
		
assign_Small_mad :
	move $t1, $s2
	
	addi $t4, $t1, -1	# while (large(t0) >= small(t1)), so must one less more
Accumulate:
	slt $t2, $t4, $t0       # while (large(t0) >= small(t1))
	beq $t2, $zero, Return
	add $t3, $t3, $t1       # ans = ans + small
	addi, $t0, $t0, -1      # large = large - 1
	j Accumulate

Return:
	move $v0, $t3	
	jr $ra


Exit:
	
