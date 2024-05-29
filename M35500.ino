/*************************************************/
/* This code is not clean, is only a point where */
/* we can get the basic funtions to try control  */
/* the VFD driver ref: M66003 of MITSUBISH.      */
/*************************************************/
#define VFD_in 7// If 0 write LCD, if 1 read of LCD
#define VFD_clk 8 // if 0 is a command, if 1 is a data0
#define VFD_stb 9 // Must be pulsed to LCD fetch data of bus
#define VFD_rst 10// Reset at low value take place. Note on the schematic the pin of reset at connector is marked as /BLK.

#define delay_tcsh _delay_us(16)


//typedef int boolean;
#define true 1
#define false 0
#define L ((1<<14)-1)
#define R ((1<<12)-1)

unsigned int timePackMan = 100;


void M66003_init(void){
  unsigned char ch = 0x00;
  unsigned char val = 0x00;
  digitalWrite(VFD_rst, HIGH);
  delay(1);
  digitalWrite(VFD_rst, LOW);
  delay(30);
  digitalWrite(VFD_rst, HIGH);
  //Reset stay at high value!
  //
  delay(100); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure VFD display (grids)
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111000);// 0xF8
  delayMicroseconds(5);
  cmd_without_stb(0b11111111);// 0x7F  D5,D6,D7 is frequency, D4 is upside down, D3,D2,D1,D0 is 18 digits 35 segments.
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  //Dimmer settings
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111001);// 0xF9
  delayMicroseconds(5);
  cmd_without_stb(0b01110000);// 0x70
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b11110101);//
  delayMicroseconds(1);
//  // set DIMM/PWM to value
//  cmd_with_stb((0b00001000) | 7);//0 min - 7 max  )(0b01010000)
//  delayMicroseconds(1);
  
  for (uint8_t i = 0x00; i<18; i++){  //This switch to ON the number of DSP to be ON
    digitalWrite(VFD_stb, LOW);
    delayMicroseconds(5);
    //val=i << 4;
    ch = ((0x00) | i);
    cmd_without_stb(ch);
    delayMicroseconds(5);
    cmd_without_stb(0xFF);
    delayMicroseconds(5);
    digitalWrite(VFD_stb, HIGH);
    delayMicroseconds(5);
    Serial.println(ch, HEX);
  }

  for (uint8_t i = 0x00; i<17; i++){  //This switch to ON the number of DSP to be ON
    digitalWrite(VFD_stb, LOW);
    delayMicroseconds(5);
    //val=i << 4;
    ch = ((0x70) | i);
    cmd_without_stb(ch);
    delayMicroseconds(5);
    cmd_without_stb(0xFF);
    delayMicroseconds(5);
    digitalWrite(VFD_stb, HIGH);
    delayMicroseconds(5);
    Serial.println(ch, HEX);
  }
//Zone to teste the VFD and let it again as normal operation.
for(uint8_t s = 0; s<4; s++){
cmd_with_stb(0b11110010);//All sements ON 
delay(500);
cmd_with_stb(0b11110000);//All sements OFF 
delay(500);
}
cmd_with_stb(0b11110001);//Normal operation
cmd_with_stb(0b11110101); //Autoincremente set to ON

//Set digits active
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11110110);// 0xF6 Digit active lower
  delayMicroseconds(5);
  cmd_without_stb(0b00010001);// 0x11
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);

//Set RAM address
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111011);// 0xFB Digit active lower
  delayMicroseconds(5);
  cmd_without_stb(0b00000000);// 0x00
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char transmit = 7; //define our transmit pin
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
   }
   //digitalWrite(VFD_clk, LOW);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char transmit = 7; //define our transmit pin
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(1);
         for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
           digitalWrite(VFD_clk, LOW);
           delayMicroseconds(1);
               if (data & mask){ // if bitwise AND resolves to true
                  digitalWrite(VFD_in, HIGH);
               }
               else{ //if bitwise and resolves to false
                 digitalWrite(VFD_in, LOW);
               }
          digitalWrite(VFD_clk, HIGH);
          delayMicroseconds(1);
         }
   digitalWrite(VFD_stb, HIGH);
   delayMicroseconds(1);
}



void wrCGRAM(){
  //writeCGRAM draw customers symbols
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(0x90);// Address of CGRAM go until 0x9F
  delayMicroseconds(10);
    cmd_without_stb(0b11111111);//
    cmd_without_stb(0b11111111);//
    cmd_without_stb(0b11111111);//
    cmd_without_stb(0b11111111);//
    cmd_without_stb(0b11111111);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);
}
void moveLeft(uint8_t grid){
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
  cmd_without_stb(0x92);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay(timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x91);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x94);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x91);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
}
void moveRight(uint8_t grid){
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
  cmd_without_stb(0x92);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay(timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x91);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x93);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
//
//teste of grids > of 14.
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
   cmd_without_stb(0x91);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delay (timePackMan);
}
void packmanCenter(uint8_t grid){
//writeCGRAM draw Packman
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(grid);// Address of CGRAM go until 0x9F
  delayMicroseconds(10);
    cmd_without_stb(0b01111111);//
    cmd_without_stb(0b11010100);//
    cmd_without_stb(0b11110111);//
    cmd_without_stb(0b11010100);//
    cmd_without_stb(0b01111111);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);  
}
void packmanLegs(uint8_t grid){
//writeCGRAM draw Packman
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(grid);// Address of CGRAM go until 0x9F
  delayMicroseconds(10);
    cmd_without_stb(0b01111100);//
    cmd_without_stb(0b11010111);//
    cmd_without_stb(0b11110100);//
    cmd_without_stb(0b11010111);//
    cmd_without_stb(0b01111100);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);  
}
void packmanLeft(uint8_t grid){
//writeCGRAM draw Packman
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(grid);// Address of CGRAM go until 0x9F
  delayMicroseconds(10);
    cmd_without_stb(0b01111110);//
    cmd_without_stb(0b11011110);//
    cmd_without_stb(0b11110100);//
    cmd_without_stb(0b11011110);//
    cmd_without_stb(0b01111100);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);  
}
void packmanRight(uint8_t grid){
//writeCGRAM draw Packman
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(grid);// Address of CGRAM go until 0x9F
  delayMicroseconds(10);
    cmd_without_stb(0b01111100);//
    cmd_without_stb(0b11011110);//
    cmd_without_stb(0b11110100);//
    cmd_without_stb(0b11011110);//
    cmd_without_stb(0b01111110);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);  
}
void packmanEmpty(uint8_t grid){
//writeCGRAM draw Packman
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111100);// 0xFC write char
  delayMicroseconds(5);
  cmd_without_stb(grid);// Address of CGRAM go until 0x9F
  delayMicroseconds(5);
    cmd_without_stb(0b00000000);//
    cmd_without_stb(0b00000000);//
    cmd_without_stb(0b00000000);//
    cmd_without_stb(0b00000000);//
    cmd_without_stb(0b00000000);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (5);  
}
void showCGRAM(){
  //present the CGRAM keeped at address 0x90 (RAM1), 0x91 (RAM2) and so on!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000);// Address of grid
  delayMicroseconds(10);
    cmd_without_stb(0x90);//
    cmd_without_stb(0x91);//
    cmd_without_stb(0x92);//
    cmd_without_stb(0x93);//
    cmd_without_stb(0x94);//
    cmd_without_stb(0x95);//
    cmd_without_stb(0x96);//
    cmd_without_stb(0x97);//
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
  delayMicroseconds(10);
}
void clearGrid(uint8_t grid){
  //write a space to let the grid position clean!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000 | grid);// Address of grid
  delayMicroseconds(10);
    cmd_without_stb(0x20);//Space
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (1);
}
void msgHiFolks(){
  //write HI FOLKS
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11111010);// 0xFA write char
  delayMicroseconds(5);
  cmd_without_stb(0b00000000);// Address of grid
  delayMicroseconds(10);
    cmd_without_stb(0x48);//H
    cmd_without_stb(0x49);//I
    cmd_without_stb(0x20);//Space
    cmd_without_stb(0x46);//F
    cmd_without_stb(0x4F);//O
    cmd_without_stb(0x4C);//L
    cmd_without_stb(0x4B);//K 
    cmd_without_stb(0x53);//S
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (1);
}

void msgSend(char *str){
  int len = strlen(str);
  Serial.println(len);
  //msg
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(5);
        cmd_without_stb(0b11111010);// 0xFA write char
        delayMicroseconds(5);
        cmd_without_stb(0b00000000);// Address of grid
        delayMicroseconds(10);
        for (uint8_t i=0;i<len;i++){
              Serial.println(str[i]);
               
              cmd_without_stb(str[i]);//
              delayMicroseconds(10);
        }
    delayMicroseconds(10);
    digitalWrite(VFD_stb, HIGH);
    delay (1);
}
void pixelRevert(){
  //write pixel inverted... not good results!!!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(5);
  cmd_without_stb(0b11110011);// 0xF3 to reverse pixels
  delayMicroseconds(5);
  cmd_without_stb(0b00000111);// Address of grid 7 to revert bit5 at 1 invert at 0 is normal
  delayMicroseconds(10);
  digitalWrite(VFD_stb, HIGH);
delay (2);
}
void setup() {
// put your setup code here, to run once:

pinMode(VFD_in, OUTPUT);// If 0 write LCD, if 1 read of LCD
pinMode(VFD_clk, OUTPUT); // if 0 is a command, if 1 is a data0
pinMode(VFD_stb, OUTPUT); // Must be pulsed to LCD fetch data of bus
pinMode(VFD_rst, OUTPUT); // Reset take place at low value

// initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  

M66003_init();
}
void loop() {
  byte str[]={0x41, 0x42, 0x43, 0x44, 0x45};
  wrCGRAM(); // All segments at RAM1 pos 0x90 is set to ON.
  //
  //This is to draw the packMan and keep it at positions 0x90 to 0x9F
  packmanCenter(0x91);
  packmanLegs(0x92);
  packmanLeft(0x93);
  packmanRight(0x94);
  packmanEmpty(0x95);

//clear of 14 grids filling they with char spaces!
        for(uint8_t grd = 0x00; grd < 0x0F; grd++){
          clearGrid(grd); 
          delayMicroseconds(10);
        }
while(1){
msgHiFolks();
delay(1800);

//pixelRevert(); // This function work, but the result of invertion of seg is not so big as we expected...
//delay(10);
//Move to the left
for(uint8_t grid = 0x0C; grid >0; grid--){
  moveRight(grid);
  Serial.println(grid, HEX);
  delay(400);
  clearGrid(grid);
  delay(100);
}
//Move to the right
for(uint8_t grid = 0x00; grid < 14; grid++){
  moveLeft(grid);
  Serial.println(grid, HEX);
  delay(400);
  clearGrid(grid);
  delay(100);
}

   showCGRAM(); //Show of draws of packman at CGRAM
   delay(2000);
   //Used to debug, you can comment it because is only the led of Arduino to tell you where the running is happen!
   for(unsigned int s=0; s<8; s++){
    digitalWrite(13, !digitalRead(13));
    delay(30);
   }
   strcpy(str, "  I'm M66003  ");
   msgSend(str);
   delay(2000); 
   strcpy(str, "  MITSUBISHI  ");
   msgSend(str);
   delay(2000); 
   strcpy(str, "Yamaha RX-V463");
   msgSend(str);
   delay(2000);
   // 
//clear of 14 grids filling they with char spaces!
        for(uint8_t grd = 0x00; grd < 0x0F; grd++){
          clearGrid(grd); 
          delayMicroseconds(10);
        }
}
}
