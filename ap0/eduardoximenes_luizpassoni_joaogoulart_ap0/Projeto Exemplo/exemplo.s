; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
INITIAL_LIST EQU 0x20000400 
FIBONACCI_LIST EQU 0x20000500

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
	
num_vec SPACE 23
			
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>

; -------------------------------------------------------------------------------
; Função main()
Start  
; Comece o código aqui <======================================================

; 3, 244, 14, 233, 1, 6, 9, 18, 13, 254, 21, 34, 2, 67, 135,  8, 89, 43, 5, 105, 144, 201, 55

	LDR R0, =INITIAL_LIST
	
	MOV R2, #3
	STRB R2, [R0], #1
	MOV R2, #244
	STRB R2, [R0], #1
	MOV R2, #14
	STRB R2, [R0], #1
	MOV R2, #233
	STRB R2, [R0], #1
	MOV R2, #1
	STRB R2, [R0], #1
	MOV R2, #6
	STRB R2, [R0], #1
	MOV R2, #9
	STRB R2, [R0], #1
	MOV R2, #18
	STRB R2, [R0], #1
	MOV R2, #13
	STRB R2, [R0], #1
	MOV R2, #254
	STRB R2, [R0], #1
	MOV R2, #21
	STRB R2, [R0], #1
	MOV R2, #34
	STRB R2, [R0], #1
	MOV R2, #2
	STRB R2, [R0], #1
	MOV R2, #67
	STRB R2, [R0], #1
	MOV R2, #135
	STRB R2, [R0], #1
	MOV R2, #8
	STRB R2, [R0], #1
	MOV R2, #89
	STRB R2, [R0], #1
	MOV R2, #43
	STRB R2, [R0], #1
	MOV R2, #5
	STRB R2, [R0], #1
	MOV R2, #105
	STRB R2, [R0], #1
	MOV R2, #144
	STRB R2, [R0], #1
	MOV R2, #201
	STRB R2, [R0], #1
	MOV R2, #55
	STRB R2, [R0], #1
	
	MOV R6, R0;				Store where the list ends
	LDR R0, =INITIAL_LIST;	Sets R0 pointer to head of list 
	LDR R1, =FIBONACCI_LIST
	MOV R9, #0;				Store fibonacci list size
	
main_loop
	LDRB R3, [R0];		Select next element on the list, loads to R3
	MOV R10, #1;
	MOV R11, #1;		Fibonacci initial values
	CMP R6, R0;			Checks if list is over (TODO), comparing where the last number was stored
	BEQ sorting;		Branch to sorting algorithm
	ADD R0, #1; 		Increase to get the next element on list
	
	
fibonacci
	CMP R10, R3;			Checks if value is in fibonacci list
	BEQ on_list;			Branch if true
	BHI main_loop;			Branch back to next element if fibonacci is bigger than current value
	MOV R7, R11;			Aux register
	MOV R11, R10			
	ADD R10, R10, R7;		Fibonacci iteration
	B fibonacci
	
on_list
	STRB R3, [R1], #1;		Value belongs to fibonacci list, stores it
	ADD R9, R9, #1;			Count total numbers that are in fibonacci
	
	B main_loop;			Branch back to next element
	
sorting
	LDR R8, =FIBONACCI_LIST
	LDR R2, =FIBONACCI_LIST;	R2 will act as a cursor
	ADD R1, R2, #1;				R1 will point to the current key, starting in the 2nd element
	MOV R11, R1

	SUB R9, #1; 			    Increment R9 so it covers the last element 
	
insertion_sort
	LDRB R3, [R1];			Select key element
	LDRB R4, [R2];   		Element pointed by cursor
 		
	B compare	
		
next_element
	MOV R1, R11;			Recovers previous key
	ADD R1, R1, #1; 		Moves key to next element
	MOV R11, R1;			Saves current key
	SUB R2, R1, #1;

    SUB R9, #1;				Decrement R9 until the list is over			
	CMP R9, #0
	IT EQ
		BEQ	finish;
	
	B insertion_sort
	
compare
	CMP R4, R3;         Check R4 > R3
	BLHI swap;          If true swap
	
	SUB R10, R8, R2
	CMP	R10, #0				
	BNE move_cursor;	Branch if cursor (R2) is not pointing to head of list
	
	B next_element;     Go to next element
	
swap
	STRB R4, [R1]
    STRB R3, [R2];          Swap R3 and R4
	
	SUB R10, R2, R8;	
	CMP	R10, #0
	ITTE NE;				If cursor (R2) is not pointing to first element of list
		MOVNE R1, R2;		Then: R1 follows the key element
		SUBNE R2, R1, #1;	Then: Cursor goes to element behind key
		BEQ next_element;   Else: Go to next element
	
	B insertion_sort
	
move_cursor
	SUB R2, R2, #1; 		Moves cursor to previous index
	B insertion_sort;		Branch back to loop
	
finish    
	NOP

    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
