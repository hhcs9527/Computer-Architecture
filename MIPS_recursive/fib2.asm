.data
str1:		.asciiz "input a: "
str2:		.asciiz "input b: "
str3:		.asciiz "ans: "
Pnewline: 	.asciiz "\n"
.text

Main:
	# declare a,b,c,d = 0
	add $s0, $zero, $zero
	add $s1, $zero, $zero
	add $s2, $zero, $zero
	
	# Prompt the user to enter int
	la $a0, str1	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
	       
	# Get the a and store in $s0
	li $v0, 5
	syscall
	add $s0, $zero, $v0
	
	move $a0, $v0
	jal fib
	move $s2, $v0 	   # assign c = fib(a)

	# Prompt the user to enter int
	la $a0, str3	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print

	# Get the a and store in $s0
	li $v0, 1
	move $a0, $s2
	syscall
	add $s0, $zero, $v0
	
	j Exit


fib:	
	# Get the a and store in $s0
	addi $sp, $sp, -8
	sw $ra, 4($sp)	
	sw $a0, 0($sp)
	
	slti $t0, $a0, 1
	beq $t0, $zero, L1
	addi $v0, $zero, 1    # x <= 2
	addi $sp, $sp, 8
	jr $ra
	
L1:
	addi $a0, $a0, -1
	jal fib
	lw $a0, 0($sp)
	lw $ra, 4($sp)
	addi $sp, $sp, 8
	mul $v0, $a0, $v0	
	jr $ra


Exit:
	
	       
