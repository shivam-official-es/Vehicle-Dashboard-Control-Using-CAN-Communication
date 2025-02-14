#include<lpc21xx.h>
#define LCD_D 0X0F<<20
#define RS 1<<17
#define RW 1<<18
#define E 1<<19

void LCD_INIT(void);
void LCD_COMMAND(char );
void LCD_DATA(char );
void LCD_STR( char *);
void delay_millisec(int );
void LCD_INTEGER(int );
void LCD_2INT(int );

int flag=0;
int *ptr=(int *)0xE0038000;

struct can
{
	int id;
	int rtr;
	int dlc;
	int byteA;
	int byteB;
};


void delay_ms(int s)
{
 	T0PR=60000-1;
 	T0TCR=0X01;
 	while(T0TC<s);
 	T0TCR=0X03;
	T0TCR=0;
}

void can_init(){
 
	PINSEL1|=0x00014000;//P0.23-->RD2 & P0.24-->TD2
	VPBDIV=1; //PCLK=60MHz

	C2MOD=0x1; //CAN2 into Reset  Mode 
	C2BTR=0x001C001D; //B125Kbps @ PLCK=60MHz(BRP=Pclk/bit rate*16)
	ptr[0]=0x20132001;
	//ptr[1]=0x20052007;
	//00ptr[2]=0x2009200B;
	SFF_sa=0x00;
	SFF_GRP_sa=0xc;
	EFF_sa=0xc;
	EFF_GRP_sa=0xc;
	ENDofTable=0xc;
	AFMR=0x0; //accept all receiving messages(data/remote)
	C2MOD=0x0; //CAN1 into Normal Mode
}


void can_tx(struct can m1)
{
 	C2TID2=m1.id;
	C2TFI2=m1.dlc<<16;
	if(m1.rtr==0){
	 	C2TFI2&=~(1<<30);//RTR=0
		C2TDA2=m1.byteA;
		C2TDB2=m1.byteB;
	}
	else{
		C2TFI2|=(1<<30);//RTR=1
	}
	C2CMR=(1<<0)|(1<<6);
	while(C2GSR&(1<<3)==0);
}

void can_rx(struct can *m2)
{
 	while(C2GSR&(0X01)==0);
	m2->id=C2RID;
	m2->dlc=C2RFS>>16&0XF;
	m2->rtr=C2RFS>>30&1;
	if(m2->rtr==0){
		m2->byteA=C2RDA;
		m2->byteB=C2RDB;
		}
		C2CMR=1<<2;
}


 
void adc_init(void) 
{
    //PINSEL1 |= (1 << 29); 
    ADCR = 0x00200404; // Enable ADC and set clock
}

unsigned int adc_read(void) 
{
    ADCR |= (1 << 24); 
    while (((ADDR>>31)&1)==0);
    return ((ADDR >> 6) & 0x3FF); 
}

void displayStatusAndVoltage(unsigned int dutycycle) {
    int voltage = (dutycycle/1023.0)*3.3; 

    if (dutycycle >=360 && dutycycle<=560) {
        LCD_COMMAND(0x80); 
        LCD_STR("LOW FUEL..!");
    } else if (dutycycle >560 && dutycycle<=860) {
        LCD_COMMAND(0x80);
        LCD_STR("MEDIUM FUEL");
	}else if (dutycycle >860) {
        LCD_COMMAND(0x80); 
        LCD_STR("HIGH FUEL...");
	}
	LCD_COMMAND(0xc0);
	LCD_INTEGER(dutycycle);
	LCD_COMMAND(0xc9);
	LCD_INTEGER(voltage);


	delay_millisec(500);
	LCD_COMMAND(0x01);
}

	
        
int main()
{
     struct can m2;
	 struct can m1;
 	 int adcValue;
	 adc_init();
	 LCD_INIT();
	 
	 can_init();

	 while (1) 
 	 {
	 	adcValue = adc_read();
        displayStatusAndVoltage(adcValue); 
        delay_millisec(200); 
        can_rx(&m2); 
		if(m2.id==0x01)
		{
			m1.id=0x09;
			m1.rtr=0;
 			m1.dlc=4;
 			m1.byteA=adcValue;
 			m1.byteB=0;
			can_tx(m1);
		}
    }
 
 	
}


void LCD_INIT(void)
{
	IODIR1=LCD_D|RS|RW|E;
	LCD_COMMAND(0x01);
	LCD_COMMAND(0x02);
	LCD_COMMAND(0x0c);
	LCD_COMMAND(0x28);
	LCD_COMMAND(0x80);
}
void LCD_COMMAND(char cmd)
{
	IOCLR1=LCD_D;
	IOSET1=(cmd & 0xf0)<<16;
	IOCLR1=RS;
	IOSET1=E;
	delay_millisec(2);
	IOCLR1=E;
	IOCLR1=LCD_D;
	IOSET1=(cmd & 0x0f)<<20;
	IOCLR1=RS;
	IOSET1=E;
	delay_millisec(2);
	IOCLR1=E;
}
void LCD_DATA(char d)
{
	IOCLR1=LCD_D;
	IOSET1=(d & 0xf0)<<16;
	IOSET1=RS;
	IOSET1=E;
	delay_millisec(2);
	IOCLR1=E;
	IOCLR1=LCD_D;
	IOSET1=(d & 0x0f)<<20;
	IOSET1=RS;
	IOSET1=E;
	delay_millisec(2);
	IOCLR1=E;
}
void LCD_STR(char *s)
{
	while(*s)
	LCD_DATA(*s++);
}
void delay_millisec(int ms)
{
T0PR=15000-1;
T0TCR=0x01;
while(T0TC<ms);
T0TCR=0x03;
T0TCR=0x00;
}

void LCD_INTEGER(int n)
{
	unsigned char a[5];
	signed char i=0;
	if(n==0)
		LCD_DATA('0');
	else
		if(n<0)
		{
			LCD_DATA('-');
			n=-n;
		}
		while(n>0)
		{
			a[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
		LCD_DATA(a[i]+48);
}

void LCD_2INT(int n)
{
	LCD_DATA(n/10+48);
	LCD_DATA(n%10+48);
}


