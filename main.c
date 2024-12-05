#include <P18f4520.h>
#include <delays.h>
#include <adc.h>
#include <stdio.h>
#include <usart.h>
#include <string.h>
#pragma config OSC = HS 
#pragma config MCLRE = ON 
#pragma config WDT = OFF 
#pragma config PBADEN = OFF 
#pragma config PWRT=ON 
#pragma config BOREN=OFF 
#pragma config LVP=OFF 


#define LCD_RS PORTDbits.RD2 
#define LCD_RW PORTDbits.RD3 
#define LCD_EN PORTCbits.RC4 
#define coi PORTCbits.RC0 
#define quat PORTCbits.RC1 
#define bom PORTCbits.RC2
void Lcd_Ghi_Lenh (char lenh)
{
	LCD_RS = 0; 
	LCD_RW = 0; 
	LCD_EN = 1; 	
	PORTDbits.RD4 = (lenh>>4)&1;
	PORTDbits.RD5 = (lenh>>5)&1;
	PORTDbits.RD6 = (lenh>>6)&1;
	PORTDbits.RD7 = (lenh>>7)&1;
	LCD_EN = 0; 
	Delay1KTCYx(50);////5ms
	LCD_EN = 1; 
	PORTDbits.RD4 = lenh&1;
	PORTDbits.RD5 = (lenh>>1)&1;
	PORTDbits.RD6 = (lenh>>2)&1;
	PORTDbits.RD7 = (lenh>>3)&1;
	LCD_EN = 0; 
	Delay1KTCYx(50);//5m
}	
void Lcd_Ghi_Dulieu (char dulieu)
{
	LCD_RS = 1; 
	LCD_RW = 0; 
	LCD_EN = 1; 
	PORTDbits.RD4 = (dulieu>>4)&1;
	PORTDbits.RD5 = (dulieu>>5)&1;
	PORTDbits.RD6 = (dulieu>>6)&1;
	PORTDbits.RD7 = (dulieu>>7)&1;
	LCD_EN = 0; 
	Delay1KTCYx(50);//5ms
	LCD_EN = 1; 
	PORTDbits.RD4 = dulieu&1;
	PORTDbits.RD5 = (dulieu>>1)&1;
	PORTDbits.RD6 = (dulieu>>2)&1;
	PORTDbits.RD7 = (dulieu>>3)&1;
	LCD_EN = 0; 
	Delay1KTCYx(50);//5ms
}

void Lcd_Ghi_Chuoi(char rom *chuoi){
	while(*chuoi){
		Lcd_Ghi_Dulieu(*chuoi);
		chuoi++;
	}
}
void Lcd_Ghi_Chuoi1(char *chuoi){
	while(*chuoi){
		Lcd_Ghi_Dulieu(*chuoi);
		chuoi++;
	}
}
void Lcd_xoa_man_hinh(){
	Lcd_Ghi_Lenh(0x01);
	Delay1KTCYx(50);
}	
void Lcd_Init(void){
	Lcd_Ghi_Lenh(0x03);
	Lcd_Ghi_Lenh(0x02); // tro ve dau dong ///k co giao tiep 4 chan k chay
	Lcd_Ghi_Lenh(0x28);
	Lcd_Ghi_Lenh(0x06);
	Lcd_Ghi_Lenh(0x0c);
	Lcd_Ghi_Lenh(0x01);
	Delay1KTCYx(50);//5ms
}
	

int isPush = 0;
int baochay=0;
int time =0;
void ngat_ngoai(void);
#pragma code uu_tien_cao = 0x08
void ngat_cao(void)
{
	ngat_ngoai(); //g?i d?n CTCPVN
}
#pragma code
#pragma interrupt ngat_ngoai

void ngat_ngoai(void)
{
	 // Kiem tra co ngat
	if (INTCONbits.INT0IF) { 
		time = 0;
		INTCONbits.INT0IF = 0;	
        baochay = !baochay;  
 	}
}
int adc0;
int adc1;
float volt0;
float volt1;
int nhietdo;
int khi;
void ADC (){
	OpenADC( ADC_FOSC_32 &
			ADC_RIGHT_JUST &
			ADC_12_TAD,
			ADC_CH1 &
		
	ADC_INT_OFF & 
			ADC_VREFPLUS_VDD &
			ADC_VREFMINUS_VSS, 13 );
	Delay10KTCYx(5);
	// chan a0
	SetChanADC(ADC_CH0);	
	ConvertADC();
	while(BusyADC());
	adc0 = ReadADC();
	volt0 = (float)(adc0*5)/1023;
	nhietdo = (int)(volt0*1000);
	//chan a1
	SetChanADC(ADC_CH1);	
	ConvertADC();
	while(BusyADC());
	adc1 = ReadADC();
	volt1 = (float)(adc1*5)/1023;
	khi = (int)(volt1*100);
	CloseADC();
}
char result;
int i=0;
int nhiet[50];
int tbc;
int tong =0;
char M[50];	
int nutnhan =0;
int ATisOK = 0;
//char ATcommand[64];
int isCall = 0;
int isClear = 0;

int main(){
	TRISD = 0;
	TRISC = 0x80;;
	TRISB = 0b00000011;
	ADCON1 = 0x0e; 
	PORTC = 0b00000000;
	//------------------- cau hinh INTERRUPT -----------------------------------------
	//RCONbits.IPEN = 0;
	INTCONbits.GIE = 1;
	//INTCONbits.PEIE = 1;
	
	//--------------- ngat INT0 ----------------------------
	INTCONbits.INT0IE = 1;
	INTCON2bits.INTEDG0 = 0;
	
	
	Lcd_Init();
	Lcd_Ghi_Lenh(0x80);
	Lcd_Ghi_Chuoi("Welcome!!");
	OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
				 USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH,10);
	Delay10KTCYx(250);
	Delay10KTCYx(250);			 
//	while(!ATisOK){
//		while (BusyUSART());
//		putrsUSART("AT\r\n");
//		while (BusyUSART());
//		ATreceive = ReadUSART();
//		if(ATreceive =='A'){
//			ATisOK = 1;
//		}	
//	}
	
	while(1){
		for (i=0;i<50;i++){
			tong = 0;
			ADC();
			nhiet[i] = nhietdo;
		}
		for(i=0;i<50;i++){
			tong = tong + nhiet[i];
		}
		tbc = tong/50;		
		if(baochay==0){
			Lcd_Ghi_Lenh(0x80);
			Lcd_Ghi_Chuoi("**Nhom9 - DACN**");
			sprintf(M,"Nh:%d.%d, kh:%d     ",tbc/10,tbc%10,khi); 
			Lcd_Ghi_Lenh(0xc0);
			Lcd_Ghi_Chuoi1(M);
			quat=0;coi=0;bom=0;
			isCall = 0;	
			if(nutnhan==0){
				if(nhietdo > 500  || khi >300 ){
					nutnhan=1;
					baochay=1;
				}
			}
			if(nhietdo < 500  && khi < 300 ){
					nutnhan=0;	
			} 
		}		
		else{ 

			if(nutnhan==0){
				quat=1;
				bom=1;
				Lcd_Ghi_Lenh(0x80);
				Lcd_Ghi_Chuoi("Dang Bao Chay***");
			}

			if (nhietdo >500){
				Lcd_Ghi_Lenh(0x80);
				Lcd_Ghi_Chuoi("Dang Bao Chay***");
				bom =1;
				if(khi>300){
					quat =1;
				}
			}
			else{
				if(khi>300){
					Lcd_Ghi_Lenh(0x80);
					Lcd_Ghi_Chuoi("RO RI KHI GAS***");
					quat =1;
				}	
			}
			coi =1;
			sprintf(M,"Nh:%d.%d, kh:%d     ",tbc/10,tbc%10,khi);  
			Lcd_Ghi_Lenh(0xc0);
			Lcd_Ghi_Chuoi1(M);	
			if(isCall == 0){
				while (BusyUSART());
				putrsUSART("ATD0339558364;	\r\n");
				isCall = 1;
			}
			}
		
		
		
	}	
}