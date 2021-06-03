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
#pragma config FOSC = INTRC_NOCLKOUT //  se declara oscilador interno
#pragma config WDTE = OFF       //Watchdog Timer apagado
#pragma config PWRTE = OFF      //Power-up Timer apagado
#pragma config MCLRE = OFF      //MCLRE apagado
#pragma config CP = OFF         //Code Protection bit apagado
#pragma config CPD = OFF        //Data Code Protection bit apagado
#pragma config BOREN = OFF      //Brown Out Reset apagado
#pragma config IESO = OFF       //Internal External Switchover bit apagado
#pragma config FCMEN = OFF      //Fail-Safe Clock Monitor Enabled bit apagado
#pragma config LVP = OFF        //low voltaje programming apagado

// CONFIG2
#pragma config BOR4V = BOR40V   // configuración de brown out reset
#pragma config WRT = OFF        // apagado de auto escritura de codigo

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
//configuracion del pic
void setup(void);
//transmision UART
void transmision_tx(char data);  //funcion para transmitir datos via UART
//cadena UART
void USART_Cadena(char *str);
//recepcion UART
char recepcion_rx();    //funcion para recepcion datos via UART

/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/
int cuenta;
char dato_recibido;     //valor recibido de interfaz
char dato ;
//-----
const char data = 10;
char out_str;
/*-----------------------------------------------------------------------------
 ---------------------------- INTERRUPCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    if (INTCONbits.T0IF)
    {
        if (cuenta >0 && cuenta <340)   //rango 1 de servos, 18ms
        {
            PORTDbits.RD0=1;        //se encienden los pines de salida
            PORTDbits.RD1=1;        //se encienden los pines de salida
            PORTDbits.RD2=1;        //se encienden los pines de salida
            __delay_ms(2);          //unico delay si se cumple 
            PORTDbits.RD0=0;        //se apagan los pines de salida
            PORTDbits.RD1=0;        //se apagan los pines de salida
            PORTDbits.RD2=0;        //se apagan los pines de salida
            TMR0 = 70;              //interrupcion cada 18ms
            INTCONbits.T0IF = 0;
        }
        else if (cuenta >340 && cuenta <682)    //rango 2 de servos, 18.5ms
        {
            PORTDbits.RD0=1;        //se encienden los pines de salida
            PORTDbits.RD1=1;        //se encienden los pines de salida
            PORTDbits.RD2=1;        //se encienden los pines de salida
            __delay_ms(1.5);        //unico delay si se cumple 
            PORTDbits.RD0=0;        //se apagan los pines de salida
            PORTDbits.RD1=0;        //se apagan los pines de salida
            PORTDbits.RD2=0;        //se apagan los pines de salida
            TMR0 = 72;              //interrupcion cada 18.5ms
            INTCONbits.T0IF=0;
        }
        else                                    //rango 3 de servos, 19ms
        {
            PORTDbits.RD0=1;        //se encienden los pines de salida
            PORTDbits.RD1=1;        //se encienden los pines de salida
            PORTDbits.RD2=1;        //se encienden los pines de salida
            __delay_ms(1);          //unico delay si se cumple 
            PORTDbits.RD0=0;        //se apagan los pines de salida
            PORTDbits.RD1=0;        //se apagan los pines de salida
            PORTDbits.RD2=0;        //se apagan los pines de salida
            TMR0 = 74;              //interrupcion cada 19ms
            INTCONbits.T0IF=0;
        }
        
    }
    if (PIR1bits.ADIF)
    {
        if (ADCON0bits.GO==0)
        {
            if (ADCON0bits.CHS==0)          //revisar canal 0
            {
                cuenta=ADRESH;              //guardo valor del pot1 en variable
                __delay_us(100);            //delay para cargar capacitor
                ADCON0bits.CHS=1;           //switch de canal
            }
            else if (ADCON0bits.CHS==1)     //revisar canal 1
            {
                CCPR1L =ADRESH;             //valor de pot2 en CCP1
                __delay_us(100);            //delay para cargar capacitor
                ADCON0bits.CHS=2;           //switch de canal
            }
            else                            //revisar canal 2
            {
                CCPR2L =ADRESH;             //valor del pot3 en CCP2
                __delay_us(100);            //delay para cargar capacitor
                ADCON0bits.CHS=0;           //switch de canal
            }
        }
        ADCON0bits.GO=1;                    //se prende conversion
        PIR1bits.ADIF=0;                    //apago bandera de interrupcion
    
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
       USART_Cadena("\r wenaaaaaaaas");
       
       if (PIR1bits.RCIF==0)
       {
           dato_recibido = recepcion_rx; //se almacena dato recibio en variable 
       }
       
       switch(dato_recibido)
        {
            //casos para el servo1
            case ('1'):                 //en caso se reciba 1, servo1 a 0°
                cuenta=300;            //se llama funcion para 0°
                transmision_tx('Servo 1 a 0');  //estado del servo a GUI
                PORTDbits.RD3=1;                //se prende led de recibido
                break;
                
            case ('2'):                 //en caso se reciba 2, servo1 a 45°
                cuenta=500;            //se llama funcion para 45°
                transmision_tx('Servo 1 a 90'); //estado del servo a GUI
                PORTDbits.RD4=1;                //se prende led de recibido
                break;
            case ('3'):                 //en caso se reciba 2, servo1 a 45°
                cuenta=800;            //se llama funcion para 45°
                transmision_tx('Servo 1 a 180'); //estado del servo a GUI
                PORTDbits.RD5=1;                //se prende led de recibido
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
    ANSELbits.ANS0=1;       //An0 como entrada analogica
    ANSELbits.ANS1=1;       //An1 como entrada analogica
    ANSELbits.ANS2=1;       //An2 como entrada analogica
    
    TRISAbits.TRISA0=1;     //RA0 como entrada
    TRISAbits.TRISA1=1;     //RA1 como entrada
    TRISAbits.TRISA2=1;     //RA2 como entrada
    
    //TRISBbits.TRISB0=1;     //RB0 como entrada
    
    TRISD=0x00;             //PortD como salida
    TRISE=0x00;
    
    PORTA=0x00;
    //PORTB=0x00;
    PORTD=0x00;
    PORTE=0x00;
    
    //CONFIGURACION DE RELOJ
    OSCCONbits.IRCF = 0b110; //Config. de oscilacion 4MHz
    OSCCONbits.SCS = 1;      //reloj interno
    
    //CONFIGURACION DEL TIMER0
    OPTION_REGbits.T0CS = 0;    //Uso reloj interno
    OPTION_REGbits.PSA = 0;     //Uso pre-escaler
    OPTION_REGbits.PS = 0b111;  //PS = 111 / 1:256
    TMR0 = 78;                  //Reinicio del timmer
    
    //CONFIGURACION DEL TIMER1
    
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
    TRISCbits.TRISC2=1;         // RC2/CCP1 como entrada a motor se desconecta
    CCP1CONbits.P1M = 0;        // configuracion de una señales de salida
    CCP1CONbits.CCP1M = 0b1100; // se configura como modo PWM
    CCPR1L = 0x0f ;             // ciclo de trabajo inicial de la onda cuadrada
    CCP1CONbits.DC1B = 0;       // LSB para ciclo de trabajo
    //CCP2
    TRISCbits.TRISC1 = 1;       // RC1/CCP2 como entrada a motor se desconecta
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
    PR2 = 10;                  //configurando el periodo de oscilación
   
    //CONFIGURACION DE UART
    //transmision
    TXSTAbits.SYNC = 0;         //se habilita transmision asincrona
    TXSTAbits.BRGH = 1;         //se habilita transmision de alta velocidad
    BAUDCTLbits.BRG16 = 1;      //se habilita uso los 16 bits   
    //braudeaje
    SPBRG = 104;                //valor adecuado al SPBRG para braudeaje                  
    SPBRGH = 0;                 //pagina 168 del datasheet del 2009       
    //recepcion
    RCSTAbits.SPEN = 1;          //se enciendeel modulo
    RCSTAbits.RX9 = 0;           //se habilita transmision de 8bits
    //encendido de modulos
    RCSTAbits.CREN = 1;          //se activa la recepción
    TXSTAbits.TXEN = 1;          //se activa la transmision 
    
    //CONFIGURACION DE EEPROM
    
    //CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;       //se habilitan interrupciones globales
    INTCONbits.PEIE = 1;    //habilitan interrupciones por perifericos
    INTCONbits.T0IE=1;      //se habilita interrupcion timer0
    INTCONbits.T0IF=0;      //se baja bandera de timer0
    PIE1bits.ADIE=1;        //se habilita interrupcion del ADC
    PIR1bits.ADIF=0;   
}

/*-----------------------------------------------------------------------------
 ----------------------------------FUNCIONES-----------------------------------
 -----------------------------------------------------------------------------*/
//funcion para transladar datos recibidos
char recepcion_rx()
{
    return RCREG;       //el valor de RCREG lo pongo en ese caracter
}

//funcion para transmitir datos a interfaz
void transmision_tx(char data)
{
    while(TXSTAbits.TRMT == 0)
    {
        TXREG = data;
    }
}

//funcion con cadena de caracteres
void USART_Cadena(char *str)
{
    while(*str != '\0')
    {
        transmision_tx(*str);
        str++;
    }
}
//funcion para escritura de datos de EEPROM
void writeToEEPROM(char data, int address) //los ints en teoria son de 8bits
{

}

//funcion para lectura de datos EEPROM
unsigned char readFromEEPROM(int address)
{

}
