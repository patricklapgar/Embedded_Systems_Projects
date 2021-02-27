/*Include files */

#include <msp432p401r.h>
#include <stdio.h>
#include <stdlib.h>
#include "UserFunctions.h"

// Function declarations
void userMenu(void);
void ADCInit(void);//initialize the ADC
float tempRead(void); //read temperature sensor

void ADCInit(void){
 //Ref_A settings
 REF_A ->CTL0 &= ~0x8; //enable temp sensor
 REF_A ->CTL0 |= 0x30; //set ref voltage
 REF_A ->CTL0 &= ~0x01; //enable ref voltage
 
 //do ADC stuff
 ADC14 ->CTL0 |= 0x10; //turn on the ADC
 ADC14 ->CTL0 &= ~0x02; //disable ADC
 ADC14 ->CTL0 |=0x4180700; //no prescale, mclk, 192 SHT
 ADC14 ->CTL1 &= ~0x1F0000; //configure memory register 0
 ADC14 ->CTL1 |= 0x800000; //route temp sense
 ADC14 ->MCTL[0] |= 0x100; //vref pos int buffer
 ADC14 ->MCTL[0] |= 22; //channel 22
 ADC14 ->CTL0 |=0x02; //enable adc
 return;
} 

float tempRead(void){
 //ADCInit();
 float temp; //temperature variable
 uint32_t cal30 = TLV ->ADC14_REF2P5V_TS30C; //calibration constant
 uint32_t cal85 = TLV ->ADC14_REF2P5V_TS85C; //calibration constant
 float calDiff = cal85 - cal30; //calibration difference
 ADC14 ->CTL0 |= 0x01; //start conversion
 while((ADC14 ->IFGR0) ==0) {
  //wait for conversion
 }
 
 temp = ADC14 ->MEM[0]; //assign ADC value
 temp = (temp - cal30) * 55; //math for temperature per manual
 temp = (temp/calDiff) + 30; //math for temperature per manual
 return temp; //return temperature in degrees C
 }

 // Delay SysTick timer
void delaySysTick(){
 SysTick->CTRL |= 0x1; // SysTick timer
 while((SysTick->CTRL & 0x10000)==0){
  // wait
 }
}

// Function for delaying Timer32
void delayTimer32(int time){
 // Enable Timer32 to turn on LED for 2 seconds and off for 2 seconds
 TIMER32_1->LOAD = time-1; // Delay for 2 seconds
 TIMER32_1->CONTROL |= 0x42; // Periodic mode
 TIMER32_1->CONTROL |= 0x80;
 while((TIMER32_1->RIS & 1) != 1){
  // wait
 }
 TIMER32_1->INTCLR &=~0x01;
 TIMER32_1->CONTROL &=~0x80; // Disable timer
}

// Function to display RGB menu and handle RGB functionality
void rgbMenu(){
 // Set up LED lights
 P1->SEL0 &= ~0x01; // Select pins on port 1
 P1->SEL1 &= ~0x01;
 
 P2->SEL0 &= ~0x07; // Select pins on port 2
 P2->SEL1 &= ~0x07;
 
 P1->DIR |= 0x01; // Red LED
 P1->DIR |= 0x07; // RGB
 P1->DIR &= ~0x12; // Set ports
 P1->REN |= 0x12;
 P1->OUT |= 0x12;
 
 P2->DIR |= 7; // Direction register for all LED lights
 P2->OUT |= 7;
 
 P1->OUT &= ~0x01; // P1 LED off
 P2->OUT &= ~0x07; // P2 LEDs off
 
 
 userOutput("Enter Combination of RGB (1 - 7): ");
 int rgbCombination = userInputNum();
 userOutput("\n\r");
 
 userOutput("Enter Toggle Time: ");
 int toggleTime = userInputNum();
 int timer = 3000000 * toggleTime;
 userOutput("\n\r");
 
 userOutput("Enter Number of Blinks: ");
 int numBlinks = userInputNum();
 userOutput("\n\r");
 
 userOutput("Blinking LED...");
 for(int i = 0; i < numBlinks; i++){
  
  SysTick->LOAD = timer - 1;
  SysTick->CTRL |= 0x4;
  P2->OUT |= rgbCombination;
  delaySysTick();
  
  // Disable LED
  P2->OUT &= ~rgbCombination;
  // Disable SysTick timer
  SysTick->CTRL &= ~0x1;
  
  SysTick->LOAD = timer - 1;
  SysTick->CTRL |= 0x4;
  //P2->OUT |= rgbCombination;
  delaySysTick();
  SysTick->CTRL &= ~0x1;
 }
 
 
 userOutput("\n\rDone\n\r");
 userMenu();
}


void digitalInput(){
 // Set up buttons
 P1->SEL0 &= ~0x13;
 P1->SEL1 &= ~0x13;
 P2->SEL0 &= ~0x07;
 P2->SEL1 &= ~0x07;
 
 P1->DIR &= ~0x12;
 P1->REN |= 0x12; // Enable pull up/ pull down resistors
 P1->OUT |= 0x12;
 
 int S1_Pressed = 1, S2_Pressed = 1;
 
 // Continuous loop
 while(1){
  
   S1_Pressed = (P1->IN & 0x02);
   S2_Pressed = (P1->IN & 0x10);
  
  // If both buttons are pressed 
  if((S1_Pressed == 0) && (S2_Pressed == 0)){
   
   userOutput("Buttons 1 and 2 pressed.\n\n\r");
   break;
  
  } else if(S1_Pressed == 0){
  
   userOutput("Button 1 pressed\n\n\r");
   break;
  
  } else if(S2_Pressed == 0){ // If switch 2 is pressed

   userOutput("Button 2 pressed\n\n\r");
   break;

  } else{ // If no buttons are pressed

   userOutput("\n\rNo Button pressed.\n\r");
   //userMenu();
   break;
  }
 }
 
 userMenu();
} 

void tempReading(){
 userOutput("Enter Number of Temperature Reading (1 - 5): ");
 int temperature = userInputNum();
 
 if(temperature > 5 || temperature < 1){
  userOutput("\n\rInvalid reading\n\r");
  userMenu();
 }
 
 ADCInit();
 
 int time = 3000000; // Timer counter for Timer32
 char reading[50];
 
 for(int i = 0; i < temperature; i++){
  // Call temperature sensor function
  float temp1 = tempRead();
  
  // Convert temp 1 to degrees Fahrenheit
  float temp2 = temp1 * (9/5) + 32;
  
  //Print out temperature readings
  sprintf(reading, "\n\rReading %d: %.2f C & %.2f F\n\r", i + 1, temp1, temp2);
  userOutput(reading);
  // Call Timer32 functionality in between iterations
  delayTimer32(time);
 }
 
 userMenu();
}

// Function that displays option to user
void userMenu(){
 char menu[] = "MSP432 Menu\n\r1. RGB Control\n\r2. Digital Input\n\r3. Temperature Reading: ";
 
 userOutput(menu);
 int input = userInputNum();
 
 switch(input){
  case 1:
   userOutput("\n\r");
   rgbMenu();
   break;
  
  case 2:
   userOutput("\n\r");
   digitalInput();
   break;
  
  case 3:
   userOutput("\n\r");
   tempReading();
   break;
 }
}


int main(void){
 // Set up UART
 EUSCI_A0->CTLW0 |= 1; // Set into reset state
 EUSCI_A0->MCTLW = 0; // Set modulation control register
 EUSCI_A0->CTLW0 |= 0x80; // Set control register to only 1 stop bit w/ no parity and only 8-bit data
 EUSCI_A0->BRW = 312; // Baud rate
 P1->SEL0 |= 0x0C;
 P1->SEL1 &= ~0x0C;
 EUSCI_A0->CTLW0 &= ~0x01; // Take out of reset state
 
 userMenu();
 
}
