# A demonstration of some simple MIPS instructions
# used to test QtSPIM

	# Declare main as a global function
	.globl main 

	# All program code is placed after the
	# .text assembler directive
	.text 		

# The label 'main' represents the starting point
main:
	li $t2, 25		# Load immediate value (25) 
	lw $t3, value		# Load the word stored in value (see bottom)
	add $t4, $t2, $t3	# Add
	sub $t5, $t2, $t3	# Subtract
	sw $t5, Z		#Store the answer in Z (declared at the bottom)  

	# Exit the program by means of a syscall.
	# There are many syscalls - pick the desired one
	# by placing its code in $v0. The code for exit is "10"
	li $v0, 10 # Sets $v0 to "10" to select exit syscall
	syscall # Exit

	# All memory structures are placed after the
	# .data assembler directive
	.data

	# The .word assembler directive reserves space
	# in memory for a single 4-byte word (or multiple 4-byte words)
	# and assigns that memory location an initial value
	# (or a comma separated list of initial values)
value:	.word 12
Z:	.word 0
