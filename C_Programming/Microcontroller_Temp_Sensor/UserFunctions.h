// UserFunctions.h

#include <stdlib.h>
#include <stdio.h>
#include <msp432p401r.h>

// Accept user input from the RX buffer and return user input
int userInput(){
 int i = 0;
 char userInput[50];
 
 while(1){
  if((EUSCI_A0->IFG & 1) != 0){
   userInput[i] = EUSCI_A0->RXBUF; // Copy the user input from RX buffer and place into array
   
   // If the enter key is pressed
   if(userInput[i] == '\r'){
    userInput[i] = '\0';
    break; // Break the loop
   } else {
    i++;
   }
    // Echo the user input
    EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    while((EUSCI_A0->IFG & 2) == 0){
     // wait
    }
  }
 }
 
 // Return pointer to user input
 return *userInput;
}

// Same function as userInput() but returns an integer version of the user input
int userInputNum(){
 int i = 0;
 char userInput[60];
 int result;
 
 while(1){
  if((EUSCI_A0->IFG & 1) != 0){
   userInput[i] = EUSCI_A0->RXBUF; // Copy the user input from RX buffer and place into array
   
   // If the enter key is pressed
   if(userInput[i] == '\r'){
    userInput[i] = '\0';
    break; // Break the loop
   } else {
    i++;
   }
    // Echo the user input
    EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    while((EUSCI_A0->IFG & 2) == 0){
     // wait
    }
  }
 }
 
 result = atoi(userInput);
 // Return pointer to user input
 return result;
}

// Funciton that works like printf() and uses TX buffer to output statement
void userOutput(char statement[]){
 int i = 0;
 while(statement[i] != 0){
  
  // Use TX buffer to output statement
  EUSCI_A0 ->TXBUF = statement[i];
  while((EUSCI_A0->IFG & 2) == 0){
   // wait
  }
  
  i++;
 }
}
