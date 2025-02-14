88 #include<LPC21XX.h>

typedef int u32;
typedef struct CAN2
{
   u32 id;
   u32 rtr;
   u32 dlc;
   u32 byteA;
   u32 byteB;
}CAN;

void motor(int n);

void can_init(void);
void can_tx(CAN );
void can_rx(CAN *);
void delay_ms(int ms);


int main()
{
 	CAN m3;

	 can_init();
	 //LCD_INIT();
	

	while(1)
	{
	  can_rx(&m3);

		if(m3.id==0x5)

		{

			motor(m3.byteA);

			//LCD_COMMAND(0xc0);

			//LCD_INTEGER(m3.byteA);

			

		}

	}

}

void motor(int n)
{

	PINSEL0=0x00020000;
	PWMMR0=15000;
	PWMMR4=n;
	PWMMCR=0X00000003;
	PWMPCR=0X00001000;
	PWMTCR=0X00000009;
	PWMLER=0x00000010;

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

