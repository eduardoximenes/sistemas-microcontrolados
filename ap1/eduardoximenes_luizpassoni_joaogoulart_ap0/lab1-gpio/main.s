; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 24/08/2020
; Este programa espera o usu�rio apertar a chave USR_SW1.
; Caso o usu�rio pressione a chave, o LED1 piscar� a cada 0,5 segundo.

; -------------------------------------------------------------------------------
        THUMB                        ; Instru��es do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declara��es EQU - Defines
;<NOME>         EQU <VALOR>
; ========================

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

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms			
		IMPORT  GPIO_Init
			
        IMPORT  PortJ_Input			; Porta J - SW1 e SW2
		IMPORT  PortQ_Output		; Porta Q - Q0:Q3 - Leds e Display 
		IMPORT  PortA_Output		; Porta A - A4:Q7 - Leds e Display 
		IMPORT  PortB_Output		; Porta B - B4:B5 - Transistor dos displays
		IMPORT  PortP_Output		; Porta P - P5 	  - Transistor dos leds

; Mapeamento dos 7 segmentos (0 a 9)
MAPEAMENTO_7SEG DCB	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F

; Mapeamento dos padrões dos leds 
MAPEAMENTO_LED DCB	0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81

; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init
	BL GPIO_Init                 ;Chama a subrotina que inicializa os GPIO

	MOV R3, #0
	MOV R5, #0					;Tick do contador

	MOV R7, #0					;Contador display   -> 0 a 99
	MOV R12, #0					;Contador leds 		-> 0 a 7
	
	MOV R8, #1 					;Registrador com o valor do passo
	MOV R9, #1					;Registrador com o sinal do incremento (1 - Crescente, 0 - Decrescente) 
	
MainLoop
	BL WriteDS1
	BL WriteDS2
	BL WriteLED

	CMP R5, #100
	IT EQ						;Verifica o tick do contador, atualiza a cada 50 	
		BLEQ UpdateCounter
	
	CMP R5, #100
	ITT EQ
		MOVEQ R5, #0
		BLEQ UpdateLED

	ADD R5, #1

Check_None
	BL PortJ_Input
	CMP	R0, #2_00000011			;Verifica se pelo menos uma chave está pressionada
	BNE CheckSW1			 	;Se o teste viu que tem pelo menos alguma chave pressionada pula

	B MainLoop

CheckSW1
	CMP R0, #2_00000010			; Verifica se apenas SW1 está pressionada
	BNE	CheckSW2				; Testa SW2
	
	CMP R8, #9
	ITE EQ
		MOVEQ R8, #1
		ADDNE R8, #1
	
	MOV R0, #300
	BL SysTick_Wait1ms	

CheckSW2
	CMP R0, #2_00000001			; Verifica se apenas SW2 está pressionada
	BNE MainLoop				; Se as duas ou nenhuma das chaves está pressionada, 
	
	CMP R9, #0
	ITE EQ						; Change flag - invert counter increaser signal
		MOVEQ R9, #1			; Increasing
		MOVNE R9, #0			; Decreasing

	

	B MainLoop

UpdateCounter
	PUSH {LR}

	CMP R9, #1
	BEQ IncreaseCounter
	BNE DecreaseCounter

	POP {LR}
	BX LR

IncreaseCounter					;If units over 9, downward reset and increase tens by 1
	ADD R7, R7, R8
	
	CMP R7, #99
	IT HI
		MOVHI R7, #0

	MOV R4, #10

	UDIV R6, R7, R4
	MLS R4, R6, R4, R7
		
	BX LR

DecreaseCounter					;If units under 0, upward reset and decrease tens by 1
	SUB R7, R7, R8

	CMP R7, #0
	IT LT
		MOVLT R7, #99

	MOV R4, #10

	UDIV R6, R7, R4
	MLS R4, R6, R4, R7

	BX LR

;--------------------------------------------------------------------------------
; Função UpdateLED
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
UpdateLED
	CMP R12, #7
	ITE EQ
		MOVEQ R12, #0
		ADDNE R12, #1

	BX LR	

WriteDS1							; Recebe o dígito das dezenas
	PUSH {LR}					; Guarda o endereço de retorno
	
	LDR  R11, =MAPEAMENTO_7SEG	; Desloca escolhendo o respectivo número das dezenas
	LDRB R10, [R11, R6]
	
	AND R0, R10, #2_11110000	; Atualiza DSDP:DSE (PA7:PA4)
	BL PortA_Output
	
	AND R0, R10, #2_00001111	; Atualiza DSD:DSA (PQ3:PQ0)
	BL PortQ_Output
	
	MOV R0, #2_010000			; Ativa o transistor do DS1 (PB4)
	BL PortB_Output
 	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	MOV R0, #2_000000			; Desativa o transistor do DS1 (PB4)
	BL PortB_Output
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	POP {LR}
	BX LR						; Retoma

WriteDS2								; Recebe o dígito das unidades
	PUSH {LR}					; Guarda o endereço de retorno
	
	LDR  R11, =MAPEAMENTO_7SEG	; Desloca escolhendo o respectivo número das unidades
	LDRB R10, [R11, R4]
	
	AND R0, R10, #2_11110000	; Atualiza DSDP:DSE (PA7:PA4)
	BL PortA_Output
	
	AND R0, R10, #2_00001111	; Atualiza DSD:DSA (PQ3:PQ0)
	BL PortQ_Output

	MOV R0, #2_100000			; Ativa o transistor do DS2 (PB5)
	BL PortB_Output
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	MOV R0, #2_000000			; Desativa o transistor do DS2 (PB5)
	BL PortB_Output
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	POP {LR}
	BX LR						; Retoma

;--------------------------------------------------------------------------------
; Função Pisca_LED
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
WriteLED
	PUSH {LR}

	LDR  R11, =MAPEAMENTO_LED	; Desloca escolhendo o respectivo número das unidades
	LDRB R10, [R11, R12]
	
	AND R0, R10, #2_11110000	; Atualiza DSDP:DSE (PA7:PA4)
	BL PortA_Output
	
	AND R0, R10, #2_00001111	; Atualiza DSD:DSA (PQ3:PQ0)
	BL PortQ_Output

	MOV R0, #2_00100000			; Ativa o transistor do DS2 (PP5)
	BL PortP_Output
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	MOV R0, #2_00000000			; Desativa o transistor do DS2 (PP5)
	BL PortP_Output
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	POP {LR}
	BX LR						; Retoma
	
; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo
