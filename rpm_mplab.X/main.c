#define _XTAL_FREQ 20000000
#define TMR2PRESCALE 4

#include <xc.h>
#include <stdio.h>

// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG


// LCD DISPLAY
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7
#include "lcd.h";
#include "pwm.h";




DC_Napred() {
  PORTBbits.RB1= 1;
  PORTBbits.RB2 = 0;
}

DC_Nazad() {
  PORTBbits.RB1 = 0;
  PORTBbits.RB2 = 1;
}

PWM1_procenat(int i){

    
   // Lcd_Clear();
 //   char s[20];
    float koef = 10.23;
    float procenat = koef*i;
    
  //  sprintf(s, "%d procenata", i);
  //  Lcd_Set_Cursor(1,1);
  //  Lcd_Write_String(s);
    
    
    return PWM1_Duty(procenat);
}


Druga_kolona(int i) {
    char s[20];
    sprintf(s, "%d", i);
    Lcd_Set_Cursor(2,1);
    //Lcd_Write_String("icaaa");
    Lcd_Write_String(s);
}



int count=0;
int sekunda=0;
void __interrupt () my_isr_routine (void) {
    if (INTCONbits.TMR0IF) { // check for flag
        INTCONbits.TMR0IF=0; //clear flage
        count++;
        if(count==76){
            count = 0;
            sekunda++;
           //Druga_kolona(sekunda);
           //PORTD=~PORTD; //flipping valueo of portD
           RC4=~RC4;
           
        }
    }
}






void main()
{
     // ----------------- STOPERICA ----------------------
    //TRISD=0x00;
    TRISC4=0; // izlaz
    //PORTD=0x00;
    INTCONbits.GIE=1; // enable all global interupts
     INTCONbits.PEIE=1; // enable peiferal int
     INTCONbits.TMR0IE=1;// enable timer interupts
     INTCONbits.TMR0IF=0; //clear timer overflow flag
     ei();
     
     //configure out timer
     OPTION_REGbits.T0CS=0; // internatl instruction cycle clock
     OPTION_REGbits.T0SE=0; // edge selectiin
     OPTION_REGbits.PSA=0; // prescaller assigned to timer0
     
     //sellect prescaler as 1:256
     OPTION_REGbits.PS0=1;
     OPTION_REGbits.PS1=1;
     OPTION_REGbits.PS2=1;
    // ----------------- STOPERICA ----------------------
    
  PWM1_Init(5000);
  PWM2_Init(5000);
  
  // OUTPUT
  TRISB1 = 0;
  TRISB2 = 0;
  
  // CCP1
  TRISC2 = 0;
  // CCP2
  TRISC1 = 0;
  
  // TRISD = 0xFF;
  TRISD0 = 1; // INPUT D0
  TRISD1 = 1; // INPUT D1
  
  //LCD OUTPUT
  TRISD2 = 0;
  TRISD3 = 0;
  TRISD4 = 0;
  TRISD5 = 0;
  TRISD6 = 0;
  TRISD7 = 0;
  
 
  //citaj encoder
  TRISC5 = 1;
   
  PWM1_Duty(0);
  PWM2_Duty(0);
  PWM1_Start();
  PWM2_Start();
  
  
// incijalizacija test
    Lcd_Init();
    Lcd_Clear();
//    Lcd_Set_Cursor(1,1);
//    Lcd_Write_String("ICA");
//    __delay_ms(1000);
//    Lcd_Clear();
  
  int flag1 = 0; // procenti
  int flag2 = 0;
  int flag3 = 0; // rpm1
  int flag4 = 0;
  int cnt = 0;
  int rpm;
  char prva[20];
  char druga[20];
  
  
  int mod_1 = 0; // 1 ON, 0 OFF
  int mod_2 = 1; // 1 ON, 0 OFF
  
  
  int procenat = 60;
  int rpm_zadato = 200;
  
  
  do
  {
      DC_Napred();
//      __delay_ms(1000);
//      DC_Nazad();
//      __delay_ms(1000);
  
      
    //------------cnt-----------
    if ((RC5 == 1) && (flag3 == 0)) {
        flag3 = 1;
        cnt++;
         //Druga_kolona(cnt);
    }
    if (RC5 == 0) {
        flag3 = 0;
    } 
      
    // MOD 1  
    if (mod_1 == 1) {
    if((RD0 == 0) && (procenat < 100) && (flag1 == 0)) {
        flag1 = 1;
        procenat = procenat + 10;
        //sekunda = 0;
        cnt = 0;
    }
      if ((RD0 =! 0) && (flag1 == 1)) {
      flag1 = 0;
      }

    if((RD1 == 0) && (procenat > 0) && (flag2 == 0)) {
        flag2 = 1;
      procenat = procenat - 10;
      //sekunda = 0;
      cnt = 0;
    }
      if ((RD1 =! 0) && (flag2 == 1)) {
         flag2 = 0;
      }
    
      PWM1_procenat(procenat);
      Lcd_Clear();
      sprintf(prva, "%d%% Faktor ispune", procenat);
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String(prva);
    
    }

   
      
      
    // MOD 2
      if (mod_2 == 1) {
        if((RD0 == 0) && (flag1 == 0)) {
            flag1 = 1;
            rpm_zadato = rpm_zadato + 5;
            if (rpm_zadato >= 300) {
                rpm_zadato = 300;
            }
            Lcd_Clear();
            sprintf(prva, "%d%% Faktor ispune", procenat);
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(prva);
            sprintf(druga, "%d RPM (%d)",rpm ,rpm_zadato);
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String(druga);
        }
        if ((RD0 =! 0) && (flag1 == 1)) {
            flag1 = 0;
        }
        if((RD1 == 0) && (flag2 == 0)) {
            flag2 = 1;
            rpm_zadato = rpm_zadato - 5;
            if (rpm_zadato <= 0) {
                rpm_zadato = 0;
            }
            Lcd_Clear();
            sprintf(prva, "%d%% Faktor ispune", procenat);
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(prva);
            sprintf(druga, "%d RPM (%d)",rpm ,rpm_zadato);
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String(druga);
        }
        if ((RD1 =! 0) && (flag2 == 1)) {
            flag2 = 0;
        }  
        

        
        
       if (sekunda == 1) {
          sekunda = 0;
          //PWM1_Stop();
          rpm = cnt*60/20;
          if (rpm_zadato > rpm) {
              procenat++; 
              //PWM1_procenat(procenat);
          }
          if (rpm_zadato < rpm) {
              procenat--;
                //PWM1_procenat(procenat);
              }
         if (procenat >= 100) {
            procenat = 100;
         }
          
        PWM1_procenat(procenat);
        Lcd_Clear();
        //char prva[20];
        //char druga[20];
        sprintf(prva, "%d%% Faktor ispune", procenat);
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String(prva);
        
        
        if (rpm >= 300) {
        sprintf(druga, "%d RPM MAX (%d)",rpm ,rpm_zadato);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(druga);
        cnt = 0;  
        } else if (rpm < 300){
        sprintf(druga, "%d RPM (%d)",rpm ,rpm_zadato);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(druga);
        cnt = 0;  
        }

      }  
        
    
            
        }
      
      
      
      
     
     
  }while(1);
}