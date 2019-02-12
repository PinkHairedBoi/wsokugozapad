//--------����������� ���������--------------------------------------
//-------------------------------------------------------------------
#define F_CPU 8000000UL      			//������� ����������
                                //�������� ������ ��
#include <avr/interrupt.h>      //����������
#include <util/delay.h>         //�������� �������
//*******************************************************************

//--------����������� ������� ������� ��16S2H------------------------
//-------------------------------------------------------------------
#define A0_PORT PORTB //�������/������
#define A0_DDR  DDRB
#define A0      5 

#define E_PORT PORTB  //���������� ��������������
#define E_DDR  DDRB
#define E      4 

//���� ������ �������
#define DB4_PORT PORTB
#define DB4_DDRD DDRB   
#define DB4      0

#define DB5_PORT PORTB
#define DB5_DDRD DDRB
#define DB5      1

#define DB6_PORT PORTB
#define DB6_DDRD DDRB
#define DB6      2

#define DB7_PORT PORTB
#define DB7_DDRD DDRB
#define DB7      3

#define set_A0() A0_PORT|=1<<A0     //���������� ������ �������/������
#define clr_A0() A0_PORT&=~(1<<A0)

#define set_E()  E_PORT|=1<<E       //���������� ������ �������������
#define clr_E()  E_PORT&=~(1<<E)
//*******************************************************************

//--------������� ��� ������ � ��� ��16S2H---------------------------
//-------------------------------------------------------------------
       //����� �������

void MT16S2H_init(void);                                //������������� �������
void MT16S2H_clr(void);                                 //������� �������
void MT16S2H_gotoxy(unsigned char x,unsigned char y);   //������� ������� (� �� 0 �� 15, y 0 ��� 1)
void MT16S2H_putstr(char *str);                         //����� ��������� ������

void MT16S2H_set_DB(unsigned char data);                //��������� ���� ������

#define DATA    1
#define COMMAND 0
void MT16S2H_write(unsigned char data,unsigned char a); //�������� ������
void MT16S2H_on()
{
	MT16S2H_write(0x0C,COMMAND);
}  //��������� �������
void MT16S2H_getchar(char c)
{
	MT16S2H_write(c, DATA);
}
//*******************************************************************

//--------������������ ������� ����������----------------------------
//-------------------------------------------------------------------
//--------����-------------------------------------------------------
enum Notes {C1=0,C1d,D1,D1d,E1,F1,F1d,G1,G1d,A1,A1d,H1,C2};         //����������� ���
unsigned char rateNotes[]={78,73,69,65,61,58,55,51,49,46,43,41,38}; //�������� ��� OCR0A

//--------������� ����������-----------------------------------------
#define melodyLenght 40					//���������� ��� � �������
unsigned char melody[2][melodyLenght]=  //� ������� ������� ����� ����, � ������ ������� ������������ ����
{{F1,E1,D1,C1,G1,G1, F1,E1,D1,C1,G1,G1, F1,A1,A1,F1, E1,G1,G1,E1, D1,E1,F1,D1,C1,C1, F1,A1,A1,F1, E1,G1,G1,E1, D1,E1,F1,D1,C1,C1},
 { 4, 4, 4, 4, 2, 2,  4, 4, 4, 4, 2, 2,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 2, 2,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 2, 2}};

#define soundON() TIMSK2=1<<OCIE2A		//������ ������ ������� ��������������� �������
//*******************************************************************

//--------��������� �����--------------------------------------------
//-------------------------------------------------------------------
typedef enum {       //��������� ��������� ��������
    notR,            //��� ��������  
    left,            //�������  ������ ������� �������
    right            //������� �� ������� �������
} Dir;
Dir direction=notR;  //����������� �������� ��������
Dir encoderStatus(void){Dir temp=direction;direction=notR;return temp;}//������� ���������� ��������� �������� � ���������� ������ ��� ���������� ���������� ������������

typedef enum{   //��������� ������
    noPress,    //�� ������
    shortPress, //������� �������
    longPress   //������� �������
} Button;
#define shortTime 150  //����� ��������� ������� ������
#define longTime  500  //����� �������� ������� ������

Button getKey(unsigned char tick);  //����� ������, ���������� ���������� �������� ������ ����� �������� ������
//*******************************************************************

//--------��������� �������------------------------------------------
//-------------------------------------------------------------------
typedef struct {                        //��������� ��� �������� �������
               unsigned char hour;      //����
               unsigned char min;       //������
               unsigned char sec;       //�������
               }Time;
               
Time realTime;                          //�������� �����
Time alarm[3];                          //����������
unsigned char activateAlarm[3];         //���������� �����������

#define ON  1
#define OFF 0
unsigned char clockOnOff=OFF;           //���� ������ �������� �����
#define clock(n) clockOnOff=n           //��������� ��� ���������� ��������� �����

//--------����� �� ������� �������-----------------------------------
void displayTime(unsigned char h,unsigned char m,unsigned char s,   //������������ ��������
                 unsigned char x,unsigned char y,                   //��������� ������� ��� ������ 
                 unsigned char blPos);                              //0 - �� ������, >3 - ������ ���, 1,2,3 - ������ ��������� �������

//--------��������� �������� �������---------------------------------
unsigned char setTime(unsigned char *h,unsigned char *m,unsigned char *s,//��������������� ��������                                               
                      Dir enc,Button key,unsigned char tick);			 //��������� �������, ��������� ������, ����� ����� �������� �������
//*******************************************************************

//--------��������� ����---------------------------------------------
//-------------------------------------------------------------------
#define TICK 10			//������������� ��������� ���� ������� ���������

enum menuStateType{
    expectInput,		//�������� ����� � ���� (���� �� ��������)
    enterMenu,			//���� � ����
    processing,			//��������� ����
    execution,			//���������� �������
    activate,			//������������ ���������
    save,				//����� ��������� � ���������� ���������
    exitMenu,			//��������� ������ ����
};
menuStateType menuState=expectInput; //��������� ��������� ����

#define maxMenu 4                   //���������� ������� ����
const char menuNames[maxMenu][17]={ //�������� ������� ����
    "��������� ����� ",
    "������ ���������",
    "������ ���������",
    "������ ���������"
};    
//*******************************************************************

//========������� ������� �������====================================
//===================================================================
int main(void){
//--------��������� ����������---------------------------------------
Button tempKey;           //�������� �������� ������� ������
Dir tempEncoder;          //�������� ��������� ��������
unsigned char number=0;   //����� ������ ����
unsigned int counter=0;   //��� �������� �������
   
//--------���������� �������� �������--------------------------------
CLKPR=1<<CLKPCE;
CLKPR=0;

//--------��������� ������ �����/������------------------------------
                        //����� ������� T0 ��� ���������� �����
					    //�������� ������ ��� ��������
					    //�������� ����� ��� ������

//--------��������� �0-----------------------------------------------
TCCR0A=(0<<COM0A1)|(0<<COM0A0)|(0<<WGM01)|(0<<WGM00);       //����� ������ �=2 CTC, �������� ������ OC0A ��� ���������� �������

//--------��������� �1-----------------------------------------------
TCCR1A=(0<<COM1A1)|(0<<COM1A0)|(0<<WGM11)|(0<<WGM10);       //����� �4 ���
TCCR1B=(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //�=64
OCR1AH=15625>>8;                                            //���������� ��� � �������
OCR1AL=15625;
TIMSK1=(1<<OCIE1A);                                         //���������� ���������� �� ����������

//--------��������� �2-----------------------------------------------
TCCR2A=(0<<WGM21)|(0<<WGM20);                       //����� ��� 
TCCR2B=(0<<WGM22)|(0<<CS22)|(0<<CS21)|(0<<CS20);    //�=1024
OCR2A=0x9C;                                         //�������� 10��

//--------��������� �������� ����������------------------------------
EICRA=(0<<ISC11)|(0<<ISC10)|(0<<ISC01)|(0<<ISC00);  //���������� �� ����� ��������
EIMSK=(0<<INT1)|(0<<INT0);                          //�������� ������� ���������� INT0
EIFR=(0<<INTF1)|(1<<INTF0);

SREG|=1<<7;                                         //����� ���������� ���������� 

//--------������������� �������--------------------------------------
					//������������� �������                
					//��������� �������

//--------�����������------------------------------------------------
soundON();                                 //�������� ��������� �������
MT16S2H_putstr((char*)"2018");             //����� ��������� ������ �� ������� ����� 
									       //������� � ������ ������ ������
MT16S2H_putstr((char*)" WSR ");            //����� ��������� ������ �� ����������
_delay_ms(10000);                          //�������� ����������� ����������� 
                                           //������� �������
clock(ON);                                 //�������� ��������� �������

//--------������� ���������------------------------------------------
for(;;_delay_ms(TICK)){               //������� ��������� �������������� � �������������� � 10��
    //--------����� �����--------------------------------------------
    tempKey=getKey(TICK);             //���������� ������
    tempEncoder=encoderStatus();      //���������� �������
       
    //--------��������� ����:----------------------------------------
    switch(menuState){                //���������� �������������� ����� ����
    //--------�������� ����� � ���� (���� �� ��������)---------------
    case expectInput:               
        if((tempKey!=noPress)||       //���� ������ ������ ���
           (tempEncoder!=notR))       //�������� �������, ��
            menuState=enterMenu;      //������ � ����
    break;                                                    
    
    //--------���� � ����:-------------------------------------------    
    case enterMenu:                 
        clock(OFF);									//��������� ��������� �������
        MT16S2H_clr();								//������� �����
        MT16S2H_putstr((char*)"    ���� 1:     ");	//����� ��������� ����
        menuState=processing;						//��������� � ��������� ����                  
    break;                  
    
    //--------��������� ����:----------------------------------------    
    case processing:                
        if(tempEncoder==right){     //�������� ���� �����
            if(++number>=maxMenu)   //����������� ������� ����
                number=0;
        }
        if(tempEncoder==left){      //�������� ���� ����
            //if()                  //��������� ������� ����
            //   ;                            
        }
        
        if(tempKey==longPress){     //���� ���� ������� ������� ������, �� 
            menuState=exitMenu;     //��������� ������ ����  
        }
        
        if(tempKey==shortPress){    //���� ���� �������� ������� ������, �� 
            menuState=execution;    //���������� � ���������� �������  
        }
        
        MT16S2H_gotoxy(9,0);                         //��������� � ������� ������ ������
        MT16S2H_getchar(number+'1');                 //������� ����� ����
        MT16S2H_gotoxy(0,1);                         //��������� � ������ ������ ������
        //MT16S2H_putstr((char*)&menuNames[number][0]);//������� �������� ����
    break;                                                 
    
    //--------���������� �������:------------------------------------
    case execution:      
        MT16S2H_gotoxy(0,0);                         //��������� � ������ ������ ������
        //MT16S2H_putstr((char*)&menuNames[number][0]);//������� �������� ����
        
        //--------���� �������---------------------------------------
        if(number==0){                        
            if(setTime(&realTime.hour,&realTime.min,&realTime.sec,tempEncoder,tempKey,TICK))
                menuState=save;                   //������� ��������� � ���������� ����������
        }
        //--------�������� �����������-------------------------------                                                            
        else{                                                                                
            if(setTime(&alarm[number-1].hour,&alarm[number-1].min,&alarm[number-1].sec,tempEncoder,tempKey,TICK))
                menuState=activate;               //������� ��������� � ���������� ����������
        }
    break;
    
    //--------������������ ���������---------------------------------
    case activate:              
        if(tempKey==longPress){               //���� ���� ������� �������, �� ��������� ���������
            menuState=save;                   //������� ��������� � ���������� ����������                    
        }    
        else                                  //����� ���������� ����������� ���������
        if((tempKey!=noPress)||               //���� ���� �������������� ����� �������, ��
           (tempEncoder!=notR)){
            activateAlarm[number-1]=!activateAlarm[number-1];//����������� ���������
        }
        
        MT16S2H_gotoxy(0,0);                  //��������� � ������ ������ ������
        //MT16S2H_putstr((char*)"    ���������   ");   //����� �������
        MT16S2H_gotoxy(0,1);                  //��������� � ������ ������ ������ 
        if(activateAlarm[number-1]==ON)       //���� ��������� �������
            MT16S2H_putstr((char*)"    �������!!!  ");   //������� �������
        //else                                             
            //MT16S2H_putstr((char*)"     ��������   ");   //������� �������
    break;
    
    //--------����� ��������� � ���������� ���������-----------------
    case save:
        if((counter+=TICK)>300){             //���� ������ 3 �������, ��         
            menuState=enterMenu;              //������������ � ����
            counter=0;
        }
    break;           
            
    //--------��������� ������ ����:---------------------------------    
    case exitMenu:                  
        number=0;                   //���������� ����� ����
                                    //������� �����
        clock(OFF);                 //�������� ����
        menuState=expectInput;                 //������� �� ����
    break;
    }   
}//for(;;)
return 0;
}//END MAIN
//===================================================================

//========���������� �1 �� ���������� � ������ �=====================
//===================================================================
ISR(TIMER1_COMPA_vect){
//--------��������� ����������---------------------------------------
static unsigned char updatingText=0;            //����� ������ ���������� �����    
unsigned char txt[3][17]={"   WorldSkills  ",   //����� ��� ������ �� ������ ������
                          "     Russia     ",
                          "   Electronics  "};
unsigned char i;                                //������� ��� ������ ������

//--------������� ������� � �������� �����������---------------------
if(++realTime.sec>=60){             //���� ������
                                    //���� �����
                                    //���� �����

    //�������� ����������� ���� ��� � ������
    for(i=0;i<3;++i){                           //��������� ��� ��� ����������
        if((activateAlarm[i]==ON)&&             //���� ��������� ������, �� 
           (alarm[i].hour==realTime.hour)&&
           (alarm[i].min==realTime.min)){
            soundON();                          //�������� ��������������� �������
        }            
    }
}

if(clockOnOff==OFF)return;                      //���������� ���������

//--------����� ������� �� �������-----------------------------------
displayTime(realTime.hour,realTime.min,realTime.sec,0,0,0);

MT16S2H_getchar(' ');

//--------��������� ���������� �����������---------------------------
if(activateAlarm[0]==ON);                         //���� ��������� �������, �� ������ �����������
else                    MT16S2H_getchar(' ');                    

if(activateAlarm[1]==ON);                         //���� ��������� �������, �� ������ �����������
else                    MT16S2H_getchar(' ');                    

if(activateAlarm[2]==ON)                    ;    //���� ��������� �������, �� ������ �����������
else                    MT16S2H_getchar(' ');                    

//--------����� ������� � ������ ������ ������-----------------------
MT16S2H_gotoxy(0,1);      //��������� �� ������ ������
if(++updatingText>=6)     //����������, ����� ����� ��������
    updatingText=6;
    
MT16S2H_putstr((char*)&txt[updatingText/2][3]);//����� ��������� ������      
}//void tim1_compa(void)
//===================================================================

//========���������� �2 �� ���������� � ������ �=====================
//===================================================================
ISR(TIMER2_COMPA_vect){
//--------��������� ����������---------------------------------------
static unsigned char notPosition=0; //����� ��������������� ���� � �������
static unsigned char iteration=0;   //��� ��������� ������������ ��������������� ����
static unsigned char leng=0;        //������������ ��������������� ����

//--------��������� �������------------------------------------------
if(iteration++==0){                           //��������� ����� ����
    TCNT0=0;                                  //�������������� ���������� ����� �������� � 0
    OCR0A=rateNotes[melody[0][notPosition]];  //�������� ������� ��������������� ����
    TCCR0B=(0<<CS02)|(0<<CS01)|(0<<CS00);     //������������ k=64 (��������� ��������� �������)                                                          
        
    if(melody[1][notPosition]==4)leng=20;     //�������� ������������ ��������������� ����
    else if(melody[1][notPosition]==2)leng=40;
         
}else if(iteration>leng){                     //������������ �������� ����� ����� ������
    iteration=0;                              //���������� ������� ������������ ����
    TCCR0B=0;                                 //��������� ��������� ��������� �������
    
    if(++notPosition>=melodyLenght){          //�������� ��������� ���� �� ������ �������
        notPosition=0;                        //����� ������ ��������������� ����
        TIMSK2=0;                             //��������� ��������������� �������
        TCCR0B=0;                             //���������� ��������� �������
        TCNT0=0;
    }
}
}//void tim1_compa(void)
//===================================================================

//========���������� �������� ���������� INT0========================
//===================================================================
ISR(INT0_vect){
//--------����� ����������� �������� ��������------------------------
if(PIND&1<<3)direction=left;     //���� ������ ������� �������� ���������, ������ ��� ������ ������ ������� �������
else         direction=right;    //�����, ������, ��� ��� ������ �� ������� �������
}//void ext_int0(void)
//===================================================================

//--------��������� ������-------------------------------------------
//-------------------------------------------------------------------
Button getKey(unsigned char tick){
//--------��������� ����������---------------------------------------
static unsigned int counter=0; //������� ������� ������
Button press=noPress;          //��������� ������� ������

//--------�������� ����� ������� ������------------------------------
if((PIND&1<<4)==0){            //���� ������ ������, ��
    if((counter+tick)<counter) //����������� ������� � ������, 
        counter=3000;          //���� �� �� ������������
    else counter+=tick;        //����� ����������� ����� ������� ������
}
else{ 
    if(counter>=longTime){     //���� ������ �� ������, �� ��������� ����� �� ������� 
        press=longPress;                                                    
    }
    else if(counter>=shortTime){     
        press=shortPress;                                                    
    }
    
    counter=0;                 //������� ������� ��� ���������� ���������
}

return press;                  //���������� ������������ ������� ������
}//unsigned char get_key(void)
//*******************************************************************

//--------����� �� ������� �������-----------------------------------
//-------------------------------------------------------------------
void displayTime(unsigned char h,unsigned char m,unsigned char s,
                 unsigned char x,unsigned char y,unsigned char blPos){
MT16S2H_gotoxy(x,y);            //��������� ������� ������ �������
if(blPos==1){                   //��������� ��������� �������        
    MT16S2H_getchar(' ');
    MT16S2H_getchar(' ');
}
else{    
    MT16S2H_getchar(h/10+'0');  //����� �������� �����
    MT16S2H_getchar(h%10+'0');  //����� ������ �����  
}
MT16S2H_getchar(':');           //����� ��������������� �����
if(blPos==2){                   //��������� ��������� �������        
    MT16S2H_getchar(' ');
    MT16S2H_getchar(' ');
}
else{    
    MT16S2H_getchar(' ');  //����� �������� �����
    MT16S2H_getchar(' ');  //����� ������ �����
}
MT16S2H_getchar(':');           //����� ��������������� �����
if(blPos==3){                   //��������� ��������� �������        
    MT16S2H_getchar(' ');
    MT16S2H_getchar(' ');
}
else{    
    MT16S2H_getchar(' ');  //����� �������� ������
    MT16S2H_getchar(' ');  //����� ������ ������
}
}//displayTime(h,m,s,blPos=0)                
//*******************************************************************

//--------��������� �������� �������---------------------------------
//-------------------------------------------------------------------
unsigned char setTime(unsigned char *h,unsigned char *m,unsigned char *s,
                      Dir enc,Button key, unsigned char tick){
//--------��������� ����������---------------------------------------
static unsigned char pos=1;            //������� ����� �����
static unsigned int counter=0;         //������� ��� ��������� �������

*s=0;                                  //���������� �������

if((counter+=tick)>1000)counter=0;     //������� �����

if(key==shortPress){                   //���� ��������� �������� �������, ��
    if(++pos>2)pos=1;                  //��������� � ����� ��������� �������
    counter=500;                       //����� ������� ����� ������ ������ � ������������ ��������� 
}

if(enc==right){                        //���������� �����
    counter=0;                         //���� ������� �� ������ �� ����� �����
    if(pos==1){                        //������������� ����
        if(++*h>=24)*h=0;        
    }
    else{             
        if(++*m>=60)*m=0;
    }
}

if(enc==left){                         //���������� �����
    counter=0;                         //���� ������� �� ������ �� ����� �����
    if(pos==1){                        //������������� ����
        if(--*h>=24)*h=23;        
    }
    else{             
        if(--*m>=60)*m=59;
    }
}

if(key==longPress){
    pos=1;          //���������������� � ���������� �������
    counter=0;
    return 1;       //���� ���� ������� ������� ������, �� �������� �������
}

//--------����� ������������� ��������-------------------------------
MT16S2H_gotoxy(0,1);
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
if(counter<500)                   //������ ��� � �������
    displayTime(*h,*m,*s,4,1,0);  //������� ��� �������
else
    displayTime(*h,*m,*s,4,1,pos);//��������� �������� ������� 
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');
MT16S2H_getchar(' ');      

return 0;       //������� �� ��������� ���� ������           
}//void setTime(*h,*m,*s,x,y)
//*******************************************************************

//--------������������� �������--------------------------------------
//-------------------------------------------------------------------
void MT16S2H_init(void){
//--------��������� ������ ��� ���������� ��������-------------------
A0_DDR|=1<<A0; 
E_DDR|=1<<E;  
DB4_DDRD|=1<<DB4;   
DB5_DDRD|=1<<DB5;
DB6_DDRD|=1<<DB6;
DB7_DDRD|=1<<DB7;

//--------������������� ������� ��� ������ �� 4� ������ ����---------
clr_A0();          //�������� ������  
_delay_ms(100);    //���� ���� ������� ������

//���������� ����������� ����������
MT16S2H_set_DB(0b00000011);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

MT16S2H_set_DB(0b00000010);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

//��������� ����������
MT16S2H_set_DB(0b00000010);
set_E();           _delay_us(1);
clr_E();           _delay_us(1);

MT16S2H_set_DB(0b00001010);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

//���������� �������
MT16S2H_set_DB(0b00000000);
set_E();           _delay_us(1);
clr_E();           _delay_us(1);

MT16S2H_set_DB(0b00001000);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

//������� �������
MT16S2H_set_DB(0b00000000);
set_E();           _delay_us(1);
clr_E();           _delay_us(1);

MT16S2H_set_DB(0b00000001);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);

//��������� ������ �����
MT16S2H_set_DB(0b00000000);
set_E();           _delay_us(1);
clr_E();           _delay_us(1);

MT16S2H_set_DB(0b00000110);
set_E();           _delay_us(1);
clr_E();           _delay_ms(1);
}//void MT16S2H_init(void)
//*******************************************************************

//--------������������� �������--------------------------------------
//-------------------------------------------------------------------
void MT16S2H_write(unsigned char data,unsigned char a){
if(a==1)set_A0();else clr_A0();

MT16S2H_set_DB(data>>4);
set_E();_delay_us(1);
clr_E();_delay_us(1);

MT16S2H_set_DB(data);
set_E();_delay_us(1);
clr_E();_delay_us(40);
}//void MT16S2H_write(unsigned char data,unsigned char a)
//*******************************************************************

//--------��������� ���� ������--------------------------------------
void MT16S2H_set_DB(unsigned char data){
if(data&(1<<0))DB4_PORT|=1<<DB4;else DB4_PORT&=~(1<<DB4);
if(data&(1<<1))DB5_PORT|=1<<DB5;else DB5_PORT&=~(1<<DB5);
if(data&(1<<2))DB6_PORT|=1<<DB6;else DB6_PORT&=~(1<<DB6);
if(data&(1<<3))DB7_PORT|=1<<DB7;else DB7_PORT&=~(1<<DB7);
}//void MT16S2H_set_DB(unsigned char data)                
//*******************************************************************

//--------������� �������--------------------------------------------
//-------------------------------------------------------------------
void MT16S2H_gotoxy(unsigned char x,unsigned char y){
if(y>0)x+=0x40; //������� �� ������ ������ 

MT16S2H_write(x|(1<<7),COMMAND);
}//void MT16S2H_gotoxy(unsigned char x,unsigned char y)   
//*******************************************************************

//--------����� ��������� ������-------------------------------------
//-------------------------------------------------------------------
void MT16S2H_putstr(char *str){
for(;*str!='\0';++str)
    MT16S2H_getchar(*str);
}//void MT16S2H_putstr(unsigned char *str)                
//*******************************************************************


//--------������� �������--------------------------------------------
//-------------------------------------------------------------------
void MT16S2H_clr(void){
MT16S2H_write(1,COMMAND);
_delay_ms(2);
}//void MT16S2H_clr(void)
//*******************************************************************                                                                
