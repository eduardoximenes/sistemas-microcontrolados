// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Template by Prof. Guilherme Peron

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gpio.h"
#include "uart.h"
#include "stepper_motor.h"

// Declarations
void PLL_Init(void);
void SysTick_Init(void);

// Terminal Messages
unsigned char waitMsg[]					= "Aguarde...\n";
unsigned char breakLine[]				= "\n\r";
unsigned char space						= ' ';
unsigned char endMsg[]					= "Fim. Pressione * para recomecar.\n";
unsigned char getSpeedMsg[] 			= "Selecione a velocidade: Passo-completo (0) | Meio-passo (1) ";
unsigned char getDirectionMsg[] 		= "Selecione o sentido de rotacao: Horario (0) | Anti-horario (1) ";
unsigned char getRotationsMsg[] 		= "Quantas voltas o motor deve dar? ";

// Global Flags
int stepperMotorActive = 0;
float currentRotation = 0;
int currentAngle = 0;
int stopRotating = 0;

// Função getSpeed
// Recebe do terminal a velocidade em que o motor deve girar
// Parâmetro de entrada: Não tem
// Parâmetro de saída: A velocidade em que o motor deve girar
unsigned char getSpeed(void)
{
	UART_SendString(getSpeedMsg);
	
	unsigned char message = 0;
	while (message == 0x0)
	{
		message = UART_Receive();
		UART_Transmit(message);
	}
	
	UART_SendString(breakLine);
	
	return (message);
}

// Função getAngle
// Recebe do terminal o sentido que o motor deve girar
// Parâmetro de entrada: Não tem
// Parâmetro de saída: O sentido que o motor deve girar
unsigned char getDirection(void)
{
	UART_SendString(getDirectionMsg);
	
	unsigned char message = 0;
	while (message == 0x0)
	{
		message = UART_Receive();
		UART_Transmit(message);
	}
	
	UART_SendString(breakLine);
	
	return (message);
}

// Função getAngle
// Recebe do terminal quantos graus o motor deve girar
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Quanto graus o motor deve girar
unsigned char* getRotations(void)
{
	UART_SendString(getRotationsMsg);
	
	unsigned char message = 0;
	static unsigned char rotations[10];
	
	int i = 0;
	
	while(message != ' ')
	{
		message = UART_Receive();
		if (message != 0 && message != ' ')
		{
			UART_Transmit(message);
			rotations[i] = message;
			
			i++;
			message = 0;
		}
	}
	
	rotations[i] = '\0';
	UART_SendString(breakLine);
	
	return (rotations);
}

// Função ATOI
// ASCII to Integer Function
// Parâmetro de entrada: String ASCII
// Parâmetro de saída: Inteiro equivalente
uint32_t ATOI(unsigned char* string)
{
	uint32_t result = 0;
	int i = 0;
	
	while (string[i] != '\0')
	{
		int digit = string[i] - '0';
		
		if (digit >= 0 && digit <= 9)
		{
			result = result * 10 + digit;
		}
		else
		{
			break;
		}
		
		i++;
	}
	
	return result;
}

// Função PrintTerminal
// Imprime as informações no terminal do Putty
// Parâmetro de entrada: Ângulo, velocidade e sentido de rotação
// Parâmetro de saída: Não tem
void PrintTerminal(uint32_t rotation, unsigned char speed, unsigned char direction)
{
	// uint32_t angle to unsigned char
	uint32_t c = rotation/100;						// centenary
	uint32_t t = (rotation - c*100)/10;	// tens
	uint32_t u = rotation % 10;					// unit
	
	unsigned char angleVector[4];
	
	angleVector[0] = c + 0x30;
	angleVector[1] = t + 0x30;
	angleVector[2] = u + 0x30;
	angleVector[3] = '\0';
	
	UART_Transmit(speed);
	UART_Transmit(space);
	
	UART_Transmit(direction);
	UART_Transmit(space);
	
	UART_SendString(angleVector);
	UART_SendString(breakLine);
}

// Função WaitForChar
// Segura a execução do programa até que o caractere desejado seja inserido
// Parâmetro de entrada: Caractere desejado
// Parâmetro de saída: Não tem
void WaitForChar(char character)
{
	while (UART_Receive() != character)
	{
		//
	}
}

// Função Rotate
// Rotaciona o motor e exibe as informações acerca do giro
// Parâmetro de entrada: Velocidade, direção e ângulo
// Parâmetro de saída: Não tem
void Rotate(void)
{
	unsigned char speed = getSpeed();
	unsigned char direction = getDirection();
	unsigned char* rotations = getRotations();
	
	uint32_t rotationsATOI = ATOI(rotations);
	uint32_t rotationsLeft = ATOI(rotations);
	
	// "Ativa" o motor
	stepperMotorActive = 1;
	
	currentRotation = 0;
	currentAngle = 0;
	
	// Itera sobre o ângulo
	for (currentRotation = 0; rotationsLeft > 0 && stopRotating == 0; currentRotation += 1, currentAngle += 45) // incrementa o ângulo de 45 em 45 graus
	{
		Control_Stepper_Motor(direction, speed); // rotaciona
		
		LEDs_Output(direction);

		UART_SendString(waitMsg);
		UART_SendString(breakLine);
		
		if(currentAngle == 360)
		{
				currentAngle = 0;
				rotationsLeft--;
				PrintTerminal(rotationsLeft, speed, direction);
		}
	}
	
	// "Desativa" o motor
	stepperMotorActive = 0;
	
	//Limpa a flag se tiver ocorrido interrupcao
	stopRotating = 0;

	// Limpa os leds
		Reset_LEDs();
	
	//Exibe mensagem de fim
	UART_SendString(endMsg);
	UART_SendString(breakLine);
	
	WaitForChar('*');
}

// Função Main
// Loop principal
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	LED_Timer_Init();
	
	while (1)
	{
		Stepper_Motor_Init();
		Reset_LEDs();
		Control_Stepper_Motor(0, 0);
		Rotate();
	}
	
	return 0;
}
