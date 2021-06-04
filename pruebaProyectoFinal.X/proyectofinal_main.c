/*------------------------------------------------------------------------------
Archivo: mainsproject.s
Microcontrolador: PIC16F887
Autor: Andy Bonilla
Compilador: pic-as (v2.30), MPLABX v5.45
    
Programa: Proyecto final de Programacion de microcontroaladores
Hardware: PIC16F887, 
    
Creado: 26 de abril de 2021    
Descripcion: 
//se usan modulo ADC, CCP1, CCP2, ida y vuelta UART, W/R EEPROM
------------------------------------------------------------------------------*/

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <xc.h>
#include <stdint.h>

/*-----------------------------------------------------------------------------
 ------------------------DIRECTIVAS DE COMPILADOR------------------------------
 -----------------------------------------------------------------------------*/
#define _XTAL_FREQ 4000000

/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);
void writeToEEPROM(uint8_t data, uint8_t address);
uint8_t readFromEEPROM(uint8_t address);


/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/
char cuenta;
char antirrebote1;
char antirrebote1_posicion=1;
char antirrebote2;
char antirrebote2_posicion=1;
char dato_recibido;
char estado1_servos=48;
char estado2_servos=49;
char estado3_servos=50;

unsigned char potenciometro1 [5] = {0x04,0x05,0x06,0x07,0x08};
//int botonPrevState;
/*-----------------------------------------------------------------------------
 ---------------------------- INTERRUPCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    //interrupcion por timer0
    if (INTCONbits.T0IF)
    {
        if (cuenta >0 && cuenta <85)  //variar oscilacion de 18ms
        {
            PORTDbits.RD0=1;
            PORTDbits.RD1=1;
            PORTDbits.RD2=1;
            __delay_ms(2);
            PORTDbits.RD0=0;
            PORTDbits.RD1=0;
            PORTDbits.RD2=0;
            TMR0 = 70;  
            INTCONbits.T0IF = 0;
        }
        else if (cuenta >85 && cuenta <170)    //variar oscilacion de 18.5ms
        {
            PORTDbits.RD0=1;
            PORTDbits.RD1=1;
            PORTDbits.RD2=1;
            __delay_ms(1.5);
            PORTDbits.RD0=0;
            PORTDbits.RD1=0;
            PORTDbits.RD2=0;
            TMR0 = 72;
            INTCONbits.T0IF=0;
        }   
        else                                    //variar oscilacion de 19ms
        {
            PORTDbits.RD0=1;
            PORTDbits.RD1=1;
            PORTDbits.RD2=1;
            __delay_ms(1);
            PORTDbits.RD0=0;
            PORTDbits.RD1=0;
            PORTDbits.RD2=0;
            TMR0 = 74;
            INTCONbits.T0IF=0;
        }
    }
    
    //interrupcion por recepcion serial
    if(PIR1bits.RCIF)
    {
        RCREG=dato_recibido;    //se almacena dato recibido en variable
    }
}


/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void)
{
    setup();
    __delay_us(100);
    ADCON0bits.GO=1;
    while(1)
    {
       //---------------switch de canales de ADC
        if (ADCON0bits.GO==0)
        {
            switch(ADCON0bits.CHS)
            {
                case(0):
                    cuenta=ADRESH;  //potenciometro 1
                    __delay_us(100);
                    ADCON0bits.CHS=1;
                    break;
                    
                case(1):
                    CCPR1L =ADRESH;             //valor de pot2 en CCP1
                    __delay_us(100);            //delay para cargar capacitor
                    ADCON0bits.CHS=2;           //switch de canal
                    break;
                    
                case(2):
                    CCPR2L =ADRESH;             //valor del pot3 en CCP2
                    __delay_us(100);            //delay para cargar capacitor
                    ADCON0bits.CHS=0;           //switch de canal
                    break;
            }
            __delay_us(100);
            ADCON0bits.GO=1;   
        }
        //---------------escritura de EEPROM
        if (PORTBbits.RB0 ==0)
        {
            antirrebote1 = 1;
        }
        
        if (PORTBbits.RB0 == 1 && antirrebote1 == 1)
        {         
            switch(antirrebote1_posicion)
            {
                case(1):
                    writeToEEPROM(cuenta, potenciometro1[0]);
                    antirrebote1_posicion=2;
                    break;
                
                case(2):
                    writeToEEPROM(cuenta, potenciometro1[1]);
                    antirrebote1_posicion=3;
                    break;
                
                case(3):
                    writeToEEPROM(cuenta, potenciometro1[2]);
                    antirrebote1_posicion=4;
                    break;
                
                case(4):
                    writeToEEPROM(cuenta, potenciometro1[3]);
                    antirrebote1_posicion=1;
                    break;

            }
            if (antirrebote1_posicion>=5)
            {
                antirrebote1_posicion=0;
            }
        }
        //---------------- lectura desde EEPROM
        if (PORTBbits.RB1 == 0)
        {
            antirrebote2 = 1;
        }
        
        if (PORTAbits.RA4  == 1 && antirrebote2 == 1)
        {
            antirrebote2=0;
             switch(antirrebote2_posicion)
             {
                 case(1):       //inclur prender ledas
                     readFromEEPROM(potenciometro1[0]);
                     antirrebote2_posicion++;
                     __delay_ms(100);
                     break;
                     
                 case(2):
                     readFromEEPROM(potenciometro1[1]);
                     antirrebote2_posicion++;
                     __delay_ms(100);
                     break;
                     
                 case(3):
                     readFromEEPROM(potenciometro1[2]);
                     antirrebote2_posicion++;
                     __delay_ms(100);
                     break;
                     
                 case(4):
                     readFromEEPROM(potenciometro1[3]);
                     antirrebote2_posicion++;
                     __delay_ms(100);
                     break;   
             }
             if (antirrebote2_posicion>4)
             {
                 antirrebote2_posicion=0;
             }
        }
        
        //-------recepcion UART
        switch(dato_recibido)
        {
            case(48):       //ver si recibio un 0 en ascii
                cuenta=50;
                TMR0 = 70;
                TXREG = estado1_servos;     //manda de vuelta el valor asquii
                break;
                
            case(49):       //ver si recibio un 1 en ascii
                cuenta=150;
                TMR0 = 72;
                TXREG=estado2_servos;
                __delay_ms(100);
                break;
                
            case(50):       //ver si recibio un 20 en ascii
                cuenta=200;
                TMR0 = 74;
                TXREG=estado3_servos;
                __delay_ms(100);
                break;
            
            case(51):
                readFromEEPROM(potenciometro1[0]);
                __delay_ms(100);
                break;
        }    
    }
}
/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
void setup(void)
{
    //CONFIGURACION DE ENTRDAS Y SALIDAS
    ANSEL=0;
    ANSELH=0;
    ANSELbits.ANS0=1;       //An0 como entrada analogica
    ANSELbits.ANS1=1;       //An1 como entrada analogica
    ANSELbits.ANS2=1;       //An2 como entrada analogica
    
    TRISAbits.TRISA0=1;     //entrada para AN0
    TRISAbits.TRISA1=1;     //entrada para AN1
    TRISAbits.TRISA2=1;     //entrada para AN2

    TRISBbits.TRISB0=1;     //RB0 como entrada
    TRISBbits.TRISB1=1;     //RB1 como entrada
    
    TRISDbits.TRISD0=0;     //salida para servo1
    TRISDbits.TRISD1=0;     //salida para servo2
    TRISDbits.TRISD2=0;     //salida para servo3
    
    PORTA=0x00;
    PORTD=0x00;
    
    //CONFIGURACION DE RELOJ
    OSCCONbits.IRCF = 0b110; //Config. de oscilacion 4MHz
    OSCCONbits.SCS = 1;      //reloj interno
    
    //CONFIGURACION DEL TIMER0
    OPTION_REGbits.T0CS = 0;    //Uso reloj interno
    OPTION_REGbits.PSA = 0;     //Uso pre-escaler
    OPTION_REGbits.PS = 0b111;  //PS = 111 / 1:256
    TMR0 = 78;                  //Reinicio del timmer
    
    //WEAK PULL UPs PORTB
    OPTION_REGbits.nRBPU = 0;   // enable Individual pull-ups
    WPUBbits.WPUB0 = 1;         // enable Pull-Up de RB0 & RB1
    WPUBbits.WPUB1 = 1;         // 
    
    //CONFIGURACION DEL ADC **3 CANALES
    ADCON1bits.ADFM = 0 ;   // se justifica a la isquierda
    ADCON1bits.VCFG0 = 0 ;  // voltajes de referencia
    ADCON1bits.VCFG1 = 0 ;  // voltaje de referencia
    ADCON0bits.ADCS = 0b01 ; // se usa division de 4us con F/32
    ADCON0bits.CHS = 0;     // seleccion de canal 0
    __delay_us(100);         //delay de 50us para que cargue capacitor
    ADCON0bits.ADON = 1 ;   // se prende modulo ADC
    
    //CONFIGURACION DE CCPx
    //CCP1
    TRISCbits.TRISC2=1;         // a motor se desconecta
    CCP1CONbits.P1M = 0;        //
    CCP1CONbits.CCP1M = 0b1100; // se configura como modo PWM
    CCPR1L = 0x0f ;             // ciclo de trabajo inicial de la onda cuadrada
    CCP1CONbits.DC1B = 0;       // LSB para ciclo de trabajo
    //CCP2
    TRISCbits.TRISC1 = 1;       // 
    CCP2CONbits.CCP2M = 0b1100; // se configura como modo PWM
    CCPR2L = 0x0f;              // ciclo de trabajo inicial de la onda cuadrada
    CCP2CONbits.DC2B1 = 0;      // LSB para ciclo de trabajo
    
    //CONFIGURACION DEL TIMER2
    PIR1bits.TMR2IF = 0;        // apagar bandera de interrupcion del timer2
    T2CONbits.T2CKPS = 0b11;    // preescaler del timer2 1:16
    T2CONbits.TMR2ON = 1;       //se prende el timer2
    //configuracion del timer2 para el PWM
    //while(PIR1bits.TMR2IF==0);  //ciclo para que nunca se prenda bandera
    PIR1bits.TMR2IF=0;          // se apaga bandera por si las moscas
    TRISCbits.TRISC2 = 0;       //salida del pwm1
    TRISCbits.TRISC1= 0;        // salida del pwm 2
   
    //CONFIGURACION DE UART
    //transmision
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    //braudeaje
    BAUDCTLbits.BRG16 = 1;
    SPBRG = 25;         // SPBRGH:SPBRG = 25
    SPBRGH = 0;
    //recepcion
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    //se encienden modulos
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 1;
    
    //CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;       //se habilitan interrupciones globales
    INTCONbits.PEIE = 1;    //habilitan interrupciones por perifericos
    INTCONbits.T0IE=1;      //se habilita interrupcion timer0
    INTCONbits.T0IF=0;      //se baja bandera de timer0
    PIR2bits.EEIF = 0;
    PIE2bits.EEIE= 0;
    return;
}

/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
//funcion para escribir en la EEPROM
void writeToEEPROM(uint8_t data, uint8_t address)
{
    PORTCbits.RC4=1;
    EEADR = address;        // argumento para direccion de EEPROM 
    EEDAT = data;           // argumento para el dato a almacenar
    
    EECON1bits.EEPGD = 0;   // Escribir a memoria de datos
    EECON1bits.WREN = 1;    // Habilitar escritura a EEPROM (datos)
    
    INTCONbits.GIE = 0;     // Deshabilitar interrupciones
    
    EECON2 = 0x55;          // Secuencia obligatoria
    EECON2 = 0xAA;          // secuencia obligatoria
    EECON1bits.WR = 1;      // se habilita la escritura
    
    //while(PIR2bits.EEIF==0);    //mini loop
    __delay_ms(300);
    PIR2bits.EEIF = 0;
    
    INTCONbits.GIE = 1;     // Habilitar interrupciones
    EECON1bits.WREN = 0;    // Deshabilitar escritura de EEPROM
    PORTCbits.RC4=0;
    return;
}

//funcion para leer de la EEPROM
uint8_t readFromEEPROM(uint8_t address)
{
    PORTCbits.RC5=1;
    EEADR = address;        // direccion a leer
    EECON1bits.EEPGD = 0;   // memoria de datos
    EECON1bits.RD = 1;      // hace una lectura
    uint8_t data = EEDAT;   // guardar el dato le�do de EEPROM
    PORTCbits.RC5=0;
    return data;
}