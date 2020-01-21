.data
str1:		.asciiz "Please select an integer number A from (0~10): "
str2:		.asciiz "A * 2 = "
str3:		.asciiz "******"
str4:		.asciiz "THE END"
Pnewline: 	.asciiz "\n"
.text

_Start:
	# Prompt the user to enter int
	la $a0, str1	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
	           
	
	# Get the int and store in $t0
	li $v0, 5
	syscall
	add $t0, $zero, $v0
  	
  	#if input = 0, goto _Exit
  	beq $t0, $zero, _Exit
  	
  	# If $t0 < 0 || $t0 > 10, goto _OutofRange
  	slt $t1, $t0, $zero
  	bne $t1, $zero, _Start # $t0 < 0 go to _Start, if == 0, already to _Exit, only consider < 0
  	
  	slti $t1, $t0, 11
  	beq $t1, $zero, _Start # $t0 > 10 go to _Start
  	
  	
  	#If input =7, compute input * 2, and print out the answer.
	addi $t1, $zero, 7
	beq $t0, $t1, _special_case
	bne $t0, $t1, _function_others
	
  	#After print out, go back to wait the user to enter number.
	j _Start
	
_special_case: # to print when input is 7
	la $a0, str2	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall
	la $a0, 0($t0)	   # load address of string to print
	li $v0, 1	   # ready to print input value
	syscall            # print
	la $a0, Pnewline   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall	           # print
	j _Start
    
_function_others:
	#setup counter i, store in t2
	addi $t2, $zero, 0
	 
	#Write a Loop for print ******
	Loop:
	
	#Chech the input and the counter
	sub $t1, $t0, $t2 # difference between i and scanf value
	beq $t1, $zero, _Start
	#End the loop and wait the user to enter number.
	

	# print out ******
	la $a0, str3   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall	           # print
	
	 
	
	#print out the counter number
	la $a0, 0($t2)   # load address of string to print
	li $v0, 1	   # ready to print input value
	syscall	           # print	             
	 
	 
	# print out ****** 
	la $a0, str3   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall	           # print          
	
	# print out \n (����)
	la $a0, Pnewline   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall	           # print
		
	
	#counter ++
	addi $t2, $t2, 1
	
	#go back to the Loop
	j Loop                

#Stop the program
_Exit:
	#print out THE END
	la $a0, str4	   # load address of string to print
	li $v0, 4	   # ready to print string
	syscall            # print
