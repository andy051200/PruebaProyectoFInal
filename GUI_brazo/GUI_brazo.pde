/*---------------Proyecto final, Programacion de microcontroladores----------------
----------------------------- Andy Bonilla, 19451 ---------------------------------
-------------Interfaz grafica para control y monitoreo de proyecto----------------*/


/*----------------------------------------------------------------------------------
-------------------------------librerias a implementar------------------------------
----------------------------------------------------------------------------------*/
import processing.serial.*;

/*----------------------------------------------------------------------------------
---------------------------definicion de variables a implementar--------------------
----------------------------------------------------------------------------------*/
Serial myPort;                        //variable objeto para comunicacion serial

byte[] dePic = new byte[6];           //bytes recibidos desde PIC
int [] dePicInt = {0,0,0,0,0,0};        //se desconoce su valor, por eso en 0

byte [] haciaPic = new byte [6];      //byts para mandar a PIC
int [] haciaPicInt = {0,1,0,1,1,0};    //arreglo de datos a mandar al PIC, [servo1_0,servo1_45,servo2_0,servo2_45,servo3_0,servo3_45]

int interval = 100;
int previousMillis = 0;
int currentMillis = 0;
int dato_transmitido=0;    //variable para mandar al PIC
int dato_recibido;         //variable con datos desde el PIC
String puerto[];          //variable tipo string para los datos


/*----------------------------------------------------------------------------------
-----------------------------definicion de cadenas a implementar--------------------
----------------------------------------------------------------------------------*/
int boton1[] = {100,150,60};             //punto en x, punto en y,tamaño0
int led1[] = {150,100,50};                         //puntox, puntoy,tamaño
int boton_eeprom[] = {220,425, 60};              //puntox, puntoy y tamaño

/*----------------------------------------------------------------------------------
---------------------------------implementacion de funciones------------------------
----------------------------------------------------------------------------------*/

//--------------------------------configuracion general-----------------------------
void setup()
{
//------------------Configuracion de puertos seriales
//puerto = Serial.list()[0];                 //declaraccion de puerto USB para USARTcom
myPort = new Serial(this, Serial.list()[0], 9600);          //configuracion de puerto y braudeaje
myPort.buffer(6);
thread("serial_comm");

  
size(550,580);                                      //tamaño de 500*500 pixeles
background(100,100,100);                            //color de fondo
textSize(20);                                       //tamaño de texto en pantalla
text("Proyecto final de Programacion", 120, 50);
text("de Micrcontroladores",160,80);
textSize(15);      
text("No es mucho pero es trabajo honesto", 120, 300);
}

//--------------------------funcion de comunicacion serial----------------------------

void serialEvent(Serial myPort)
{
  myPort.readBytes(dePic);
  for (int i=0;i<6 ;i++)
  {
    dePicInt[i] = dePic[i] & 0xFF;
  }
  
}

void serial_comm()
{
  while (true)
  {
    currentMillis = millis();
  
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      for (int i = 0; i<6;i++)
      {
        haciaPic[i]=byte(haciaPicInt[i]);
      }
      myPort.write(haciaPic);
    }
  }
}

//----------------------------------funcion de dibujo------------------------------------
void draw()
{
  println("---------------------------------------------");
  println("ESTADO DE LOS SERVO MOTORES EN EL CIRCUITO");
  println("---------------------------------------------");
  println("El servo 1 está en:");       //texto de referencia
  println(dePicInt[0]);                 //estado del servo 1
  println("El servo 2 está en:");      //texto de referencia
  println(dePicInt[1]);                 //estado del servo 2
  println("El servo 3 está en:");      //texto de referencia
  println(dePicInt[2]);                //estado del servo3
  
  
  
  //idealmente tambien debería mostrar los valores que se estan modificando de la eeprom y demas
  
  
  fill(1300,1300,1300);
  
  //------------------------DIBUJO DE BOTON PARA POSICION 1
  //servos a 0°
  fill(300,300,300);                                  //
  textSize(15);                                       //tamaño de texto en pantalla
  text("Posicion 1°",boton1[0]-2, boton1[1]-10);
  square(boton1[0],boton1[1],boton1[2]);
  if (mousePressed)
  {
    if (mouseX > boton1[0] && mouseX < boton1[0]+boton1[2])
    {
      if (mouseY > boton1[1] && mouseY < boton1[1]+boton1[2])
      {
        fill(255,0,0);
        square(boton1[0],boton1[1],boton1[2]);
        dato_transmitido=0;
        myPort.write(dato_transmitido);       //valor
      }
    }
  }
  
  
  
   //------------------------DIBUJO DE BOTON PARA POSICION 2
  //servo 2 a 0°
  fill(300,300,300);                                  //
  textSize(15);                                       //tamaño de texto en pantalla
  text("Servos a 90",boton1[0]+120, boton1[1]-10);
  square(boton1[0]+120,boton1[1],boton1[2]);
  if (mousePressed)
  {
    if (mouseX > boton1[0]+120 && mouseX < boton1[0]+boton1[2]+120)
    {
      if (mouseY > boton1[1] && mouseY < boton1[1]+boton1[2])
      {
        fill(255,0,0);
        square(boton1[0]+120,boton1[1],boton1[2]);
        dato_transmitido=1;
        myPort.write(dato_transmitido);
      }
    }
  }
  
  
  //------------------------DIBUJO DE BOTON PARA POSICION 3
  fill(300,300,300);                                  //
  textSize(15);                                       //tamaño de texto en pantalla
  text("Servos a 180",boton1[0]+240, boton1[1]-10);
  square(boton1[0]+240,boton1[1],boton1[2]);
  if (mousePressed)
  {
    if (mouseX > boton1[0]+240 && mouseX < boton1[0]+boton1[2]+240)
    {
      if (mouseY > boton1[1] && mouseY < boton1[1]+boton1[2])
      {
        fill(255,0,0);
        square(boton1[0]+240,boton1[1],boton1[2]);
        dato_transmitido=2;
      }
    }
  }
  
  
  //------------------------DIBUJO DE BOTON PARA DATOS EN EEPROM
  fill(300,300,300);
  textSize(15);
  text("Guardar en EEPROM", boton_eeprom[0]-32,boton_eeprom[1]-5);
  square(boton_eeprom[0],boton_eeprom[1], boton_eeprom[2]);
  if (mousePressed)
  {
    if (mouseX > boton_eeprom[0] && mouseX < boton_eeprom[0]+boton_eeprom[2] )
    {
      if (mouseY > boton_eeprom[1] && mouseX < boton_eeprom[1]+boton_eeprom[2])
      {
        fill(255,0,0);
        square(boton_eeprom[0],boton_eeprom[1], boton_eeprom[2]);
        //poner variable que va a 
        dato_transmitido=3; 
      }
    }
  }
  println(dato_transmitido);
}
