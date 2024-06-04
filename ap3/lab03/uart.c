#include <stdint.h>
#include <math.h>

#include "uart.h"
#include "tm4c1294ncpdt.h"

#define UARTSysClk 80000000
#define BAUDRATE 19200 

void SysTick_Wait1ms(uint32_t delay);

// Fun��o UART_Init
// Inicializa a UART
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void UART_Init(void)
{
	// 1. Habilitar clock no m�dulo UART e verificar se est� pronta para uso.
	SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;
	
	while ((SYSCTL_PRUART_R & SYSCTL_PRUART_R0) != SYSCTL_PRUART_R0) {
		//
	}
	
	// 2. Garantir que a UART esteja desabilitada antes de fazer as configura��es.
	//UART0_CTL_R = UART0_CTL_R & (~UART_CTL_UARTEN);
	UART0_CTL_R = 0x00;
	
	// 3. Escrever o baud-rate nos registradores UARTIBRD e UARTFBRD.
	
	// Verifica se o bit HSE do UART0_CTL_R � 1 ou 0 e define clkDiv como 8 ou 16, respectivamente.
	int clkDiv = ((UART0_CTL_R & 0x20) == 0) ? 16 : 8;	// 0x20 = 2_100000
	float BRD = UARTSysClk/(clkDiv * BAUDRATE);
	
	//UART0_IBRD_R = (int) BRD;
	//UART0_FBRD_R = (int) round((BRD - (int) BRD) * 64);
	UART0_IBRD_R = 260;
	UART0_FBRD_R = 27;

	// 4. Configurar o registrador UARTLCRH para o n�mero de bits, paridade, stop bits e fila.
	// UARTLCRH: SPS | WLEN | FEN | STP2 | EPS | PEN | BRK
	// Lembrar de trocar parity bit
	// 2_0111 1110 -> 7E -> paridade par 
	// 2_0111 1010 -> 7A -> paridade ímpar
	UART0_LCRH_R = 0xF2; 
	

	// 5. Garantir que a fonte de clock seja o clock do sistema no registrador UARTCC escrevendo 0.
	UART0_CC_R = 0x00;
	
	// 6. Habilitar as flags RXE, TXE e UARTEN no registrador UARTCTL (habilitar a recep��o, transmiss�o e a UART).
	//UART0_CTL_R = (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
	UART0_CTL_R = 0x301;
}

// Fun��o UART_Receive
// Recebe dados
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: Dado recebido
unsigned char UART_Receive(void)
{
	unsigned char message = 0;
	unsigned long queueEmpty = (UART0_FR_R & UART_FR_RXFE) >> 4;

	if (!queueEmpty)
	{
		message = UART0_DR_R;
	}

	return message;
};

// Fun��o UART_Transmit
// Transmite dados
// Par�metro de entrada: Caractere a ser transmitido
// Par�metro de sa�da: N�o tem
void UART_Transmit(unsigned char character)
{
	unsigned long queueFull = (UART0_FR_R & UART_FR_TXFF) >> 5;

	if ((!queueFull) && (character != 0))
	{
		UART0_DR_R = character;
	}
	
	SysTick_Wait1ms(10);
};

// Fun��o UART_SendString
// Transmite uma string transmitindo cada caractere em sequ�ncia
// Par�metro de entrada: String a ser transmitida
// Par�metro de sa�da: N�o tem
void UART_SendString(unsigned char* string)
{
	unsigned char character = string[0];
	int i = 1;
	
  while (character != '\0')
	{
		UART_Transmit(character);
		character = string[i++];
	}
};
