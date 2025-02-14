#include<LPC21XX.h>

#define LED1 1<<17
#define LED2 1<<18

typedef int u32;
typedef struct CAN2
{
   u32 id;
   u32 rtr;
   u32 dlc;
   u32 byteA;
   u32 byteB;
}CAN;

void can_init(void);
void can_tx(CAN m1);
void can_rx(CAN *m1);
void delay_ms(int ms);


int main()
{
    CAN m1;
	

	IODIR0=LED1|LED2;
	IOSET0=LED1|LED2;

	can_init();



	while(1)
	{
	    can_rx(&m1);
		
		if(m1.id==0x01)
		{

			 IOCLR0=LED1|LED2;
//			IOCLR0=LED1;
//			delay_ms(200);
//			IOSET0=LED1;
//			IOCLR0=LED2;
//			delay_ms(200);
//			IOSET0=LED2;
	

		}

		if(m1.id==0x02)
		{
		 	IOSET0=LED1|LED2;
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

void delay_ms(int ms)
{
   T0PR=15000-1;
   T0TCR=0x01;
   while(T0TC<ms);
   T0TCR=0x03;
   T0TCR=0x00;
}
