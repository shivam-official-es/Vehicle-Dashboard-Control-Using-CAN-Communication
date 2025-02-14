#include<LPC21XX.h>

#define LCD 0x0f<<20
#define RS 1<<17
#define RW 1<<18
#define E 1<<19

#define SW1 14
#define SW2 15
#define SW3 16

typedef int u32;
typedef struct CAN2
{
   u32 id;
   u32 rtr;
   u32 dlc;
   u32 byteA;
   u32 byteB;
}CAN;

void LCD_INIT(void);
void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_STRING(unsigned char*);
void LCD_STR(unsigned char*,int);

void can_init(void);
void can_tx(CAN m1);
void can_rx(CAN *m1);
void delay_ms(int ms);


int i=0,c=0;

int main()
{
    CAN m1;	 //Indicator
	CAN m0;  //Fuel
	CAN m2;	 //Fuel
	CAN m3;	 //Wiper

 		can_init();
	LCD_INIT();


	m0.rtr=0;
 	m0.dlc=4;
 	m0.byteA=0xffffffff;
 	m0.byteB=0;


	m1.id=0x01;
	m1.rtr=0;
	m1.dlc=0x04;
	m1.byteA=0xDDCCBBAA;
	m1.byteB=0x00;

	m3.id=0x5;
	m3.rtr=0;
 	m3.dlc=4;
 	//m3.byteA=0xffffffff;
 	m3.byteB=0;

	LCD_STRING("V24CE2V2");
	delay_ms(2000);
	LCD_COMMAND(0x01);

	while(1)
	{
		if(((IOPIN0 >> SW1)&1) == 0) //Fuel
		{  
            m0.id = 0x01;    
            can_tx(m0);
			can_rx(&m2);
			//LCD_COMMAND(0xcb);
			//LCD_INTEGER(m2.byteA);

			if (m2.byteA <=360) 
			{
       			 LCD_COMMAND(0xc0); 
       			 LCD_STRING("LOW FUEL..!       ");
			}
			 
			if (m2.byteA >361 && m2.byteA<=700) 
			{
        		LCD_COMMAND(0xc0);
       			LCD_STRING("MEDIUM FUEL..        ");
	  		}

			if (m2.byteA >700) 
			{
       		 LCD_COMMAND(0xc0); 
       		 LCD_STRING("HIGH FUEL...        ");
			}        
        }


	    if(((IOPIN0>>SW2)&1)==0)   //Indicator
		{
	   
			delay_ms(500);
			i++;
			

			if(i==1)
			{	
				m1.id=0x01;
				can_tx(m1);
				LCD_COMMAND(0x80);
			 	LCD_STRING("INDICATOR ON...");
			}
			else
			{
				m1.id=0x02;
				can_tx(m1);
			    LCD_COMMAND(0x80);
			 	LCD_STRING("INDICATOR OFF...");
			}

			if(i==2)
			{
			 	i=0;
			}

		}
		delay_ms(500);
	


	 if(((IOPIN0>>SW3)&1) == 0) 		//Wiper
		{  

	     	delay_ms(500);
			c++;
			switch(c)
			{
				case 1:
				{
					//m3.id=0x5;
					m3.byteA=9000;
					can_tx(m3);
					LCD_COMMAND(0XC0);
			        LCD_STRING("Low Speed...      "); 
					break;
				}
				case 2:
				{
					//m3.id=0x5;
					m3.byteA=12000;
					can_tx(m3);
					LCD_COMMAND(0XC0);
			        LCD_STRING("Medium Speed...      ");
					break;
				}
				case 3:
				{
					//m3.id=0x5;
					m3.byteA=15000;
					can_tx(m3);
					LCD_COMMAND(0XC0);
			        LCD_STRING("High Speed...          ");
					break;
				}
				default:
				{
					//m3.id=0x5;
					m3.byteA=0;
					can_tx(m3);
					LCD_COMMAND(0XC0);
			        LCD_STRING("Wiper off...         "); 
					c=0;

				}
			}

		}
	

	}
}



void can_init()
{
   VPBDIV=1;
   PINSEL1=0x00014000;
   C2MOD=0x01;
   AFMR=0x02;
   C2BTR=0x001c001d;
   C2MOD=0x0;

}

void can_tx(CAN m1)
{
   C2TID1=m1.id;
   C2TFI1=(m1.dlc<16);
   if(m1.rtr==0)
   {
	  C2TFI1 &=~(1<<30);
	  C2TDA1=m1.byteA;
	  C2TDB1=m1.byteB;

   }
   else
   {
	   C2TFI1 |=(1<<30);
   }

   C2CMR=(1<<0)|(1<<5);

   while((C2GSR & (1<<3))==0);

}

void can_rx(CAN *m1)
{
   while((C2GSR & 0x1)==0);

   m1->id = C2RID;
   m1->dlc=(C2RFS>>16)&0xf;
   m1->rtr=(C2RFS>>30) & 0x1;

   if(m1->rtr==0)
   {
	   m1->byteA=C2RDA;
	   m1->byteB=C2RDB;
   }

   C2CMR=(1<<2);

}

void LCD_INIT(void)
{
 IODIR1=LCD|RS|RW|E;
 LCD_COMMAND(0x01);
 LCD_COMMAND(0x02);
 LCD_COMMAND(0x0c);
 LCD_COMMAND(0x28);
 LCD_COMMAND(0x80);


}

void LCD_STRING(unsigned char *s)
{
while(*s)
LCD_DATA(*s++);
}

void LCD_COMMAND(unsigned char cmd)
{
 IOCLR1=LCD;
 IOSET1=(cmd&0xf0)<<16;
 IOCLR1=RS;
 IOCLR1=RW;
 IOSET1=E;
 delay_ms(2);
 IOCLR1=E;

 IOCLR1=LCD;
 IOSET1=(cmd&0x0f)<<20;
 IOCLR1=RS;
 IOCLR1=RW;
 IOSET1=E;
 delay_ms(2);
 IOCLR1=E;

}

void LCD_DATA(unsigned char d)
{
 IOCLR1=LCD;
 IOSET1=(d&0xf0)<<16;
 IOSET1=RS;
 IOCLR1=RW;
 IOSET1=E;
 delay_ms(2);
 IOCLR1=E;

 IOCLR1=LCD;
 IOSET1=(d&0x0f)<<20;
 IOSET1=RS;
 IOCLR1=RW;
 IOSET1=E;
 delay_ms(2);
 IOCLR1=E;

}

void LCD_STR(unsigned char* s,int k)
{
 int i;
 for(i=0;s[i];i++)
 {
  LCD_DATA(s[i]);
  k++;
  if(k==0x90)
  LCD_COMMAND(0x80);

 }
}

void delay_ms(int ms)
{
   T0PR=15000-1;
   T0TCR=0x01;
   while(T0TC<ms);
   T0TCR=0x03;
   T0TCR=0x00;
}
