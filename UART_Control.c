

#include <msp430.h>

unsigned char Rx_Data = 2;					// Byte received via UART
unsigned char superflag= 0;
  char flag=1;
    int slow;
    char prev;
    char Dup_Data;
 
void main(void)
{
	/*** Set-up system clocks ***/
	WDTCTL = WDTPW + WDTHOLD;				// Stop WDT
        DCOCTL = 0;
	BCSCTL1 = CALBC1_1MHZ;					// Set DCO
	DCOCTL = CALDCO_1MHZ;
	/*** Set-up GPIO ***/
	P1SEL = BIT1 + BIT2;					// P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;					// P1.1 = RXD, P1.2=TXD
        
        
        P2DIR |= BIT1;                // TA1CCR1 on P2.1
        P2SEL |= BIT1;                // TA1CCR1 on P2.1
        P2SEL &= ~(BIT1);
        
       //  P2OUT |= ~(BIT4+BIT5);                
       
        
	P1DIR |= BIT6 + BIT0;					// P1.6 set as output
        P1SEL |= BIT6 + BIT0;
        P1SEL &= ~(BIT6 + BIT0);
        
        TA1CTL |= TASSEL_2 + MC_1;
        
    
	//P1OUT &= ~(BIT6 + BIT0);				// P1.6 set low
	/*** Set-up USCI A ***/
	UCA0CTL1 |= UCSSEL_2;						// 1MHz 9600
	UCA0BR1 = 0;							// 1MHz 9600
	UCA0MCTL = UCBRS0;						// Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;			// SMCLK
	UCA0BR0 = 104;								// Initialize USCI state machine
	//IE2 |= UCA0RXIE;						// Enable USCI_A0 RX interrupt
        
        TA1CCR0 = 9999;
       
        
     
        
        
        //enable all timers
        TA1CCTL0 |= CCIE;                // enable CC interrupts
        TA1CCTL1 |= OUTMOD_4|CCIE;        // set TACCTL1 to Set / Reset mode//enable CC interrupts
        TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
        
          
        
	IE2 |= UCA0RXIE;                  // Enable USCI_A0 RX interrupt

    __bis_SR_register(LPM0_bits + GIE);  // interrupts enabled

  

   
    
    while (1)
{
        while (superflag ==1)
        {
             if (Rx_Data==251) // Sports Car Mode
                {
                            flag=0;
                           Rx_Data = 2;
                           TA1CCR1=80;
                           superflag = 0;
                }
                
                else if (Rx_Data==252) //Truck Mode
                {
                            flag =1;
                            Rx_Data= 2;
                            TA1CCR1=80;
                            superflag = 0;
                }
        
                else  if (Rx_Data==253) //Coast
                {
                          Rx_Data=2;
                          TA1CCR1=80;
                           superflag = 0;
                           flag=prev;
                }
                
               
                       else if (Rx_Data==254) //Left Turn
                      {
                    
                    
                  
                   P2REN |= BIT4;
                    P2OUT |= BIT4;
                    P2OUT &= ~BIT5;
                    P2REN &= ~BIT5;
                    __delay_cycles(20000);
                    
                   superflag =0;
                    
                     P2OUT &= ~(BIT4 + BIT5);
                     flag=prev;
            
                  }

             
                  else if(Rx_Data==255) //Right Turn
                  {   
                    
                    P2REN |= BIT5;
                    P2OUT |= BIT5;
                    P2OUT &= ~BIT4;
                    P2REN &= ~BIT4;
                     
                     __delay_cycles(20000);
                      superflag =0;
                     flag=prev;
                     P2OUT &= ~(BIT4 + BIT5);
                    
                  }
        }    
     
        
        
      while (superflag==0)
      {
        P2REN &= ~(BIT4+BIT5);
        P2OUT &= ~(BIT4 + BIT5);
    
    if (flag==1) // In Truck Mode PWM increments & decrements are slower
    { 
      prev=flag;
      for(slow=TA1CCR1;slow<(Dup_Data*40);slow++) // In Truck Mode PWM increments & decrements are slower
      {
              TA1CCR1=slow;
             __delay_cycles(3000);
      }
      
      for (slow=TA1CCR1;slow>(Dup_Data*40);slow--)
      {        
              TA1CCR1=slow;
              __delay_cycles(3000);
      }
      
    }
    
    else if (flag==0)  // In Sportscar Mode PWM increments & decrements are faster
      
 {
     prev=flag;
      
      for(slow=TA1CCR1;slow<(Dup_Data*40);slow++)
      {         
                    __delay_cycles(100);
                    TA1CCR1=slow;   
                
      }
      
      for (slow=TA1CCR1;slow>(Dup_Data*40);slow--)
              {
                   
                __delay_cycles(100);
                    TA1CCR1=slow; 
                     

              }
}
}
}
}

#pragma vector=USCIAB0RX_VECTOR  // Receive UART interrupt
__interrupt void USCI0RX_ISR(void)
{
	Rx_Data = UCA0RXBUF; // Assign received byte to Rx_Data
         
        if (Rx_Data>250)
        {
          superflag = 1;
        }
        
        else if (Rx_Data<=250)
        {
          superflag=0;
          Dup_Data=Rx_Data;
        
        }
	__bic_SR_register_on_exit(LPM0_bits);	// Wake-up CPU
}



#pragma vector = TIMER1_A0_VECTOR
__interrupt void captureCompareInt1 (void) {
    P1OUT |= BIT6 + BIT0;                        //Turn on LED
     P2OUT |= BIT1;
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
   
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void captureCompareInt2 (void) {
    P1OUT &= ~(BIT6 + BIT0);                        //Turn off LED
     P2OUT &= ~BIT1;
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}





