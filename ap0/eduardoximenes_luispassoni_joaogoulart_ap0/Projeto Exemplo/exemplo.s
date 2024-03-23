; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instru��es do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declara��es EQU - Defines
;<NOME>         EQU <VALOR>
Rdm				EQU 0x20000400
Fib 			EQU	0x20000500
; -------------------------------------------------------------------------------
; �rea de Dados - Declara��es de vari�veis
		AREA  DATA, ALIGN=2
		; Se alguma vari�vel for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a vari�vel <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma vari�vel de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posi��o da RAM		

; -------------------------------------------------------------------------------
; �rea de C�digo - Tudo abaixo da diretiva a seguir ser� armazenado na mem�ria de 
;                  c�digo
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma fun��o do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a fun��o Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma fun��o externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; fun��o <func>

; -------------------------------------------------------------------------------
; Fun��o main()
	
Start  
; Comece o c�digo aqui <======================================================

	;2) Carregar a lista de n�meros aleat�ria
	;{3, 244, 14, 233, 1, 6, 9, 18, 13, 254, 21, 34, 2, 67, 135, 8, 89, 43, 5, 105, 144, 201, 55}
	LDR R0, =Rdm
	
	MOV R1, #3
	STRB R1, [R0], #4 
	MOV R1, #244
	STRB R1, [R0], #4 
	MOV R1, #14
	STRB R1, [R0], #4 
	MOV R1, #233
	STRB R1, [R0], #4 
	MOV R1, #1
	STRB R1, [R0], #4 
	MOV R1, #6
	STRB R1, [R0], #4 
	MOV R1, #9
	STRB R1, [R0], #4 
	MOV R1, #18
	STRB R1, [R0], #4 
	MOV R1, #13
	STRB R1, [R0], #4 
	MOV R1, #254
	STRB R1, [R0], #4 
	MOV R1, #21
	STRB R1, [R0], #4 
	MOV R1, #34
	STRB R1, [R0], #4 
	MOV R1, #2
	STRB R1, [R0], #4 
	MOV R1, #67
	STRB R1, [R0], #4 
	MOV R1, #135
	STRB R1, [R0], #4 
	MOV R1, #8
	STRB R1, [R0], #4 
	MOV R1, #89
	STRB R1, [R0], #4 
	MOV R1, #43
	STRB R1, [R0], #4 
	MOV R1, #5
	STRB R1, [R0], #4 
	MOV R1, #105
	STRB R1, [R0], #4 
	MOV R1, #144
	STRB R1, [R0], #4 
	MOV R1, #201
	STRB R1, [R0], #4 
	MOV R1, #55
	STRB R1, [R0] 
	
	MOV R10, R0 	; POSICAO FINAL
	LDR R0, =Rdm 	; POSICAO INICIAL

	LDRB R1, [R0], #4
	LDRB R2, [R0], #4
	CMP R1, R2
	BHS 
	

	
	


	;3) Fazer uma varredura da lista de n�meros aleat�rios para encontrar quais n�meros s�o pertencentes � s�rie de Fibonacci,
	LDR R0, =Rdm
	LDR R1, =Fib
label
	LDRB R2, [R0], #4 
	CMP 

fib
	
	

	
	NOP



    ALIGN                           ; garante que o fim da se��o est� alinhada 
    END                             ; fim do arquivo
