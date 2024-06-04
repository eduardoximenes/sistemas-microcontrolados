// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Template by Prof. Guilherme Peron

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// Defini��es dos Ports
#define GPIO_PORTA (0x00000001)	// SYSCTL_PPGPIO_P0
#define GPIO_PORTF (0x00000020)	// SYSCTL_PPGPIO_P5
#define GPIO_PORTH (0x00000080)	// SYSCTL_PPGPIO_P7
#define GPIO_PORTJ (0x00000100)	// SYSCTL_PPGPIO_P8
#define GPIO_PORTN (0x00001000)	// SYSCTL_PPGPIO_P12
#define GPIO_PORTP (0x00002000)	// SYSCTL_PPGPIO_P13
#define GPIO_PORTQ (0x00004000)	// SYSCTL_PPGPIO_P14

void SysTick_Wait1ms(uint32_t delay);

// Global Flags (external)
extern int stepperMotorActive;
extern int currentAngle;
extern int stopRotating;
extern int direction;

// -------------------------------------------------------------------------------
// Fun��o GPIO_Init
// Inicializa os ports A, F, H, J, N, P e Q
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void GPIO_Init(void)
{
	// 1. Habilitar o clock no m�dulo GPIO no registrador RCGGPIO (cada bit representa uma GPIO) e
	// esperar at� que a respectiva GPIO esteja pronta para ser acessada no registrador PRGPIO (cada
	// bit representa uma GPIO).
	SYSCTL_RCGCGPIO_R = (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ);
	SYSCTL_RCGCUART_R = 1;
	
  while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ) ) != 
													 (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ) )
	{
		while((SYSCTL_PRUART_R & 1 ) != 1 ){};
	};
	
	// Configura��o UART
	UART0_CTL_R = 0x00;
	UART0_IBRD_R = 260;      //230626 3 : baudrate 19200
	UART0_FBRD_R = 27;
	UART0_LCRH_R = 0x7A;     //2_0111 1010 -> 7A -> paridade ímpar 
	UART0_CC_R = 0x00;
	UART0_CTL_R = 0x301;
	
	// 2. Desabilitar a funcionalidade anal�gica no registrador GPIOAMSEL.
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;				
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;				// UART0
	GPIO_PORTF_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
															
	// 4. DIR para 0 se for entrada, 1 se for sa�da
	GPIO_PORTA_AHB_DIR_R = 0xF2;				// 11110010	: PA7 ao PA4 e PA1
	GPIO_PORTF_AHB_DIR_R = 0x11;				// 10001		: PF3 ao PF1
	GPIO_PORTH_AHB_DIR_R = 0x0F;				// 1111			: PH3 ao PH0 (driver de pot�ncia)
	GPIO_PORTJ_AHB_DIR_R = 0x00;				// 00				: PJ0
	GPIO_PORTN_DIR_R = 0x03;						// 11				: PN1 ao PN0
	GPIO_PORTP_DIR_R = 0x20;						// 100000		: PP5
	GPIO_PORTQ_DIR_R = 0x0F;						// 1111			: PQ3 ao PQ0
		
	// 5. Limpar os bits AFSEL para selecionar GPIO sem fun��o alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;			// UART
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0xF3;				// 11110011	: PA7 ao PA4 e PA1 ao PA0
	GPIO_PORTF_AHB_DEN_R = 0x11;				// 10001		: PF3 ao PF1
	GPIO_PORTH_AHB_DEN_R = 0x0F;				// 1111			: PH3 ao PH0 (driver de pot�ncia)
	GPIO_PORTJ_AHB_DEN_R = 0x01;				// 1				: PJ0
	GPIO_PORTN_DEN_R = 0x03;						// 11				: PN1 ao PN0
	GPIO_PORTP_DEN_R = 0x20;						// 100000		: PP5
	GPIO_PORTQ_DEN_R = 0x0F;						// 1111			: PQ3 ao PQ0
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x01;				// PJ0
	
	// 8. Interrup��es
	GPIO_PORTJ_AHB_IM_R		= 0x00;
	GPIO_PORTJ_AHB_IS_R		= 0x00;
	GPIO_PORTJ_AHB_IBE_R	= 0x00;
	GPIO_PORTJ_AHB_IEV_R	= 0x00;
	GPIO_PORTJ_AHB_ICR_R	= 0x01;
	GPIO_PORTJ_AHB_IM_R		= 0x01;
	NVIC_EN1_R						= 0x80000;		// 2_1 << 19
	NVIC_PRI12_R					= 0xA0000000;	// 2_5 << 29
}

// Fun��o LED_Timer_Init
// Habilita o timer que far� o LED da placa EK-TM4C1294-XL a cada 200ms
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void LED_Timer_Init(void)
{
	// 1. Habilitar o clock do timer 2 e esperar at� estar pronto para uso.
	SYSCTL_RCGCTIMER_R = 0x4; // 0x1 << 2
	
	while ((SYSCTL_PRTIMER_R & 0x4) != 0x4)
	{
		//
	}
	
	// 2. Desabilita o timer 2 para configura��o
	TIMER2_CTL_R = TIMER2_CTL_R & 0xFFE;
	
	// 3. Configura��o do timer 2
	TIMER2_CFG_R = 0x00;				// Quantos bits ser� a contagem do temporizador (32 bits: 0x00)
	TIMER2_TAMR_R = 0x02; 			// Modo de opera��o do timer 2 (0x02: Peri�dico)
	TIMER2_TAILR_R = 0x7A11FF ; 		// Valor da contagem calculado baseado no clock (7.999.999)
	TIMER2_TAPR_R = 0x0; 				// Valor do prescaler (0x0: sem prescaler)
	
	TIMER2_ICR_R = 0x1;					// Limpa a flag de interrup��o do timer 2
	TIMER2_IMR_R = 0x1; 				// Habilita a interrup��o do timer 2
	
	NVIC_PRI5_R = 4 << 29; 			// Seta prioridade 4 para a interrup��o
	NVIC_EN0_R = 1 << 23; 			// Habilita a interrup��o do timer no NVIC
	
	// 4. Habilita o timer 2 ap�s a configura��o
	TIMER2_CTL_R = TIMER2_CTL_R | 0x1;
}

// Fun��o Timer2A_Handler
// Alterna o estado do LED da placa EK-TM4C1294-XL
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void Timer2A_Handler(void)
{
	TIMER2_ICR_R = 0x01; // Limpa a flag de interrup��o do timer 2
	
	if (stepperMotorActive) // Alterna o estado do LED
	{
		if (GPIO_PORTN_DATA_R == 0x0)
		{
			GPIO_PORTN_DATA_R = 0x1;
		}
		else 
		{
			GPIO_PORTN_DATA_R = 0x0;
		}
		
	} 
	else 
	{
		GPIO_PORTN_DATA_R = 0x0;
	}
	
	uint32_t temp1 = GPIO_PORTA_AHB_DATA_R;
	uint32_t temp2 = GPIO_PORTQ_DATA_R;
	
	GPIO_PORTA_AHB_DATA_R = 0x0;
	GPIO_PORTQ_DATA_R = 0x0;
	SysTick_Wait1ms(25);
	
	GPIO_PORTA_AHB_DATA_R = temp1;
	GPIO_PORTQ_DATA_R = temp2;
}

// Fun��o Reset_LEDs
// Reseta os LEDs da PAT
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void Reset_LEDs(void)
{
	GPIO_PORTA_AHB_DATA_R = GPIO_PORTA_AHB_DATA_R & 0x00;	
	GPIO_PORTQ_DATA_R = GPIO_PORTQ_DATA_R & 0xF0;  			
}

// Fun��o LEDs_Output
// Acende os LEDs da PAT de acordo com a rota��o do motor
// Par�metro de entrada: Sentido de rota��o (Hor�rio ou Anti-hor�rio)
// Par�metro de sa�da: N�o tem
void LEDs_Output(char direction)
{
	//GPIO_PORTQ_DATA_R = 0x0;
	//GPIO_PORTQ_DATA_R = 0x0;
	GPIO_PORTA_AHB_DATA_R = 0x0;
	GPIO_PORTQ_DATA_R = 0x0;
	if (currentAngle % 45 == 0)
	{
		int currentLED = currentAngle/45;
		
		if (direction == '0')
		{
			if (currentLED == 0) {GPIO_PORTQ_DATA_R = 0x1;}
			if (currentLED == 1) {GPIO_PORTQ_DATA_R = 0x3;}
			if (currentLED == 2) {GPIO_PORTQ_DATA_R = 0x7;}
			if (currentLED == 3) {GPIO_PORTQ_DATA_R = 0xF;}
			if (currentLED == 4) 
				{GPIO_PORTA_AHB_DATA_R = 0x10; 
				GPIO_PORTQ_DATA_R = 0xF;}
			if (currentLED == 5) 
				{GPIO_PORTA_AHB_DATA_R = 0x30; 
				GPIO_PORTQ_DATA_R = 0xF;}
			if (currentLED == 6) 
				{GPIO_PORTA_AHB_DATA_R = 0x70; 
				GPIO_PORTQ_DATA_R = 0xF;}
			if (currentLED == 7) 
				{GPIO_PORTA_AHB_DATA_R = 0xF0; 
				GPIO_PORTQ_DATA_R = 0xF;}
		} 
		else if (direction == '1')
		{
			if (currentLED == 0) {GPIO_PORTA_AHB_DATA_R = 0x80;}
			if (currentLED == 1) {GPIO_PORTA_AHB_DATA_R = 0xC0;}
			if (currentLED == 2) {GPIO_PORTA_AHB_DATA_R = 0xE0;}
			if (currentLED == 3) {GPIO_PORTA_AHB_DATA_R = 0xF0;}
			if (currentLED == 4) {GPIO_PORTQ_DATA_R = 0x8;
														GPIO_PORTA_AHB_DATA_R = 0xF0;}
			if (currentLED == 5) {GPIO_PORTQ_DATA_R = 0xC;
														GPIO_PORTA_AHB_DATA_R = 0xF0;}
			if (currentLED == 6) {GPIO_PORTQ_DATA_R = 0xE;
														GPIO_PORTA_AHB_DATA_R = 0xF0;}
			if (currentLED == 7) {GPIO_PORTQ_DATA_R = 0xF;
														GPIO_PORTA_AHB_DATA_R = 0xF0;}
		}
	}
		
	GPIO_PORTP_DATA_R = 0x20; 	// Ativa o transistor PP5
	SysTick_Wait1ms(5);	
	
}

// Fun��o GPIOPortJ_Handler
// Trata a interrup��o no PJ0
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void GPIOPortJ_Handler(void)
{	
	if (stopRotating)
	{
		stopRotating = 0;
	} 
	else 
	{
		stopRotating = 1;
	}
	
	GPIO_PORTJ_AHB_ICR_R = 1;	// Limpa a flag de interrupção
}

// Fun��o PortH_Output
// Escreve no Port H
// Par�metro de entrada: Quais fases do motor deve acionar
// Par�metro de sa�da: N�o tem
void PortH_Output(uint32_t phases)
{
	uint32_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0x00;
	
	temp = temp | phases;
	
	GPIO_PORTH_AHB_DATA_R = temp;
}
