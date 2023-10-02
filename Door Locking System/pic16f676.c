//PIC16F676

#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON     // RA3/MCLR pin function select (RA3/MCLR pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)


#include <xc.h>
#define _XTAL_FREQ 20000000

#define b1 RA0
#define b2 RA1
#define b3 RA2
#define b4 RA4
#define ch RA5

#define in1 RC0
#define in2 RC1
#define En RC2

#define red RC3
#define green RC4
#define blue RC5

unsigned int len,plen=4,max_try=5;
unsigned char key[];
unsigned char password[]="1234";
unsigned char byte;

void read(unsigned char addr){
    EEADR = addr;
  //  EEPGD=0;
    RD=1;
    byte=EEDATA;
}
void write(unsigned char data,unsigned char addr){
    EEADR = addr;
    EEDATA=data;
//    EEPGD=0;
    WREN=1;
    EECON2=0x55;
    EECON2=0xAA;
    WR=1;
    while(EEIF==0);
    EEIF=0;
}


void key_detect(){
    int kp,k,i;
    len=plen;
    for(k=0;k<len;k++){
        kp=0;
        do{
        if(b1==0){
            __delay_us(200);
            while(b1==0);
            key[k]='1';
            kp=1;
            __delay_ms(2);
        }
        if(b2==0){
            __delay_us(200);
            while(b2==0);key[k]='2';kp=1;
            __delay_ms(2);
        }
        if(b3==0){
            __delay_us(200);
            while(b3==0);key[k]='3';kp=1;
            __delay_ms(2);
        }
        if(b4==0){
            __delay_us(200);
            while(b4==0);key[k]='4';kp=1;
            __delay_ms(2);
        }
        if(ch==0){
            __delay_us(200);
            while(ch==0);key[k]='#';kp=1;
            __delay_ms(2);
        }
        }while(kp==0);
        if(key[0]=='#'){
            len=(2*plen)+1;
            blue=1;
        }
    }
    
    if(len==((2*plen)+1)){
    if(key[1]==password[0]){
        if(key[2]==password[1]){
            if(key[3]==password[2]){
                if(key[4]==password[3]){
                //    if(key[5]==password[4]){
                //    if(key[6]==password[5]){
                        if(key[plen+1]!='#'){
                            for(i=0;i<plen;i++){
                                password[i]=key[i+plen+1];
                            }
                        }
                //    }
                //    }
                }
            }
        }
    }
    }
}

void forward(){
    En=1; in1=0; in2=1;
    __delay_ms(7000);
    En=0; in1=0; in2=0;
    
}
void reverse(){
    En=1; in1=1; in2=0;
    __delay_ms(7000);
    En=0; in1=0; in2=0; 
}

void main(void) {
    
    unsigned char flag,i,p,r,try=0;
   
    TRISA=0xFF;
    TRISC=0;
    PORTA=0;
    PORTC=0;
    ANSEL=0x00;
    CMCON |= 0x07;
    OPTION_REGbits.nRAPU=0;
// forgot the password uncommand the next line  
//   for(r=0;r<plen;r++){write(password[r],r);}
        
    while(1){      
        red=1; green=0; blue=0;
        flag=0;
        p=0;
        
        for(r=0;r<plen;r++){
            read(r);
            password[r]=byte;
        }
        
        key_detect();
        if (len == ((plen*2)+1)) {
            p = plen+1;
        }
        if(key[p]==password[0]){
            if(key[p+1]==password[1]){
                if(key[p+2]==password[2]){
                    if(key[p+3]==password[3]){
                    //    if(key[p+4]==password[4]){
                    //    if(key[p+5]==password[5]){
                            flag=1;
                    //    }
                    //    }
                    }
                }
            }
        }
        if((flag==1) && (len== ((plen*2)+1) )){
            red=0; green=0; blue=0;
            try=0;
            for(r=0;r<plen;r++){
                write(password[r],r);
            }
            
            for(i=0;i<4;i++){
                blue=~blue;
                __delay_ms(200);   
            }
            green=1;
            forward();
            //wait time
            __delay_ms(2000);
            reverse();
            red=1; green=0; blue=0;
        }
        else if(((flag==0) && (len== ((plen*2)+1))) || (flag==0)){
            red=1; green=0; blue=0;
            try++;
            for(i=0;i<4;i++){
                red= ~red; 
                __delay_ms(200);   
            }          
        }
        else if(flag==1){
            red=0; green=1; blue=0;
            try=0;
            forward();
            //wait time
            __delay_ms(2000);
            reverse();
            red=1; green=0; blue=0;
        }
        if(try==max_try){
            red=1; blue=1;
            for(i=0;i<12;i++){
            __delay_ms(10000);
            }
            try=0;
        }
        red=0; green=0; blue=1;
        __delay_ms(500); 
       
    }
}
