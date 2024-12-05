#include <p18f4520.h>  // Thu vi?n d�nh cho PIC18F4520
#include <delays.h>    // Thu vi?n t?o tr?
#include <adc.h>       // Thu vi?n ADC d? d?c gi� tr? t? c?m bi?n nhi?t d?
#include <stdio.h>     // Thu vi?n s? d?ng h�m sprintf cho hi?n th?

// C?u h�nh c�c bit cho PIC18F4520
#pragma config OSC = HS         // Ch?n dao d?ng t?c d? cao (High-Speed Oscillator)
#pragma config MCLRE = ON       // B?t ch�n Master Clear (MCLR)
#pragma config WDT = OFF        // T?t Watchdog Timer
#pragma config PBADEN = OFF     // T?t t�nh nang analog cho c�c ch�n PORTB
#pragma config PWRT=ON          // B?t Power-up Timer
#pragma config BOREN=OFF        // T?t Brown-out Reset
#pragma config LVP=OFF          // T?t ch? d? l?p tr�nh di?n �p th?p

#define quat PORTBbits.RB5      // Qu?t k?t n?i v?i ch�n RB5
#define bom PORTBbits.RB6       // Bom k?t n?i v?i ch�n RB6
#define nut_nhan PORTBbits.RB2  // N�t nh?n k?t n?i v?i ch�n RB2

// H�m ghi l?nh l�n LCD
void Lcd_Ghi_Lenh (char lenh)
{
    PORTEbits.RE0 = 0; // RS = 0 (Ch?n thanh ghi l?nh)
    PORTEbits.RE1 = 0; // RW = 0 (Ch? d? ghi)
    PORTEbits.RE2 = 1; // K�ch ho?t ch�n EN
    PORTD = lenh;      // G?i l?nh l�n PORTD (D0-D7)
    PORTEbits.RE2 = 0; // H?y k�ch ho?t EN
    Delay1KTCYx(100);  // Tr? 10ms
}

// H�m ghi d? li?u l�n LCD
void Lcd_Ghi_Dulieu (char dulieu)
{
    PORTEbits.RE0 = 1; // RS = 1 (Ch?n thanh ghi d? li?u)
    PORTEbits.RE1 = 0; // RW = 0 (Ch? d? ghi)
    PORTEbits.RE2 = 1; // K�ch ho?t ch�n EN
    PORTD = dulieu;    // G?i d? li?u l�n PORTD (D0-D7)
    PORTEbits.RE2 = 0; // H?y k�ch ho?t EN
    Delay1KTCYx(10);   // Tr? 10ms
}

// H�m ghi chu?i k� t? l�n LCD t? b? nh? chuong tr�nh (ROM)
void Lcd_Ghi_Chuoi(const char rom *chuoi){
    while(*chuoi){
        Lcd_Ghi_Dulieu(*chuoi); // Ghi t?ng k� t?
        chuoi++;
    }
}

// H�m ghi chu?i k� t? l�n LCD t? b? nh? RAM
void Lcd_Ghi_Chuoi1(char *chuoi){
    while(*chuoi){
        Lcd_Ghi_Dulieu(*chuoi); // Ghi t?ng k� t?
        chuoi++;
    }
}

// H�m kh?i t?o LCD
void Lcd_Init(void){
    Lcd_Ghi_Lenh(0x38);  // Ch? d? 8-bit, 2 d�ng
    Lcd_Ghi_Lenh(0x06);  // T? d?ng d?ch con tr?
    Lcd_Ghi_Lenh(0x0c);  // B?t m�n h�nh, t?t con tr?
    Lcd_Ghi_Lenh(0x01);  // X�a m�n h�nh
    Delay1KTCYx(10);     // Tr? 10ms
}

int adc;                // Bi?n luu gi� tr? ADC
float volt;             // Bi?n luu gi� tr? di?n �p d?c t? ADC
int nhietdo;            // Bi?n luu gi� tr? nhi?t d?
char M[50];             // Chu?i t?m cho vi?c hi?n th?
int tt = 0;             // Tr?ng th�i chuy?n d?i LCD
int baochay = 0;        // C? b�o ch�y (0 = kh�ng ch�y, 1 = c� ch�y)
int nut_nhan_cu = 1;    // Bi?n luu tr?ng th�i n�t nh?n tru?c d� (m?c d?nh l� chua nh?n - 1)

// H�m d?c gi� tr? t? c?m bi?n ADC
void ADC (void){
    OpenADC( ADC_FOSC_32 &        // C?u h�nh ADC clock
             ADC_RIGHT_JUST &     // Can ph?i k?t qu?
             ADC_12_TAD,          // Th?i gian l?y m?u
             ADC_CH0 &            // Ch?n k�nh AN0
             ADC_INT_OFF &        // T?t ng?t ADC
             ADC_VREFPLUS_VDD &   // �i?n �p tham chi?u duong l� VDD
             ADC_VREFMINUS_VSS, 14 ); // �i?n �p tham chi?u �m l� VSS
    ConvertADC();                 // B?t d?u chuy?n d?i ADC
    while(BusyADC());             // Ch? chuy?n d?i ho�n t?t
    adc = ReadADC();              // �?c gi� tr? ADC
    CloseADC();                   // ��ng ADC
}

// Ch? d? hi?n th? th�ng tin b�nh thu?ng
void chedo_thuong(){
    if((nhietdo < 10) && (tt == 0)){
        Lcd_Ghi_Lenh(0x01);        // X�a m�n h�nh
        Delay1KTCYx(10);           // Tr? 10ms
        tt = 1;
    }
    else if (nhietdo > 10){
        tt = 0;
    }
    Lcd_Ghi_Lenh(0x80);            // Di chuy?n con tr? d?n d?u d�ng
    Lcd_Ghi_Chuoi("**Nhom9 - DACN**");
    sprintf(M, "Nhiet do: %d", nhietdo);  // �?nh d?ng chu?i nhi?t d?
    Lcd_Ghi_Lenh(0xc0);            // Di chuy?n con tr? d?n d�ng th? hai
    Lcd_Ghi_Chuoi1(M);
}

// Ch? d? hi?n th? th�ng tin khi c� ch�y
void chedo_baochay(){
    Lcd_Ghi_Lenh(0x01);            // X�a m�n h�nh
    Lcd_Ghi_Lenh(0x80);            // Di chuy?n con tr? d?n d?u d�ng
    Lcd_Ghi_Chuoi("**DANG CO CHAY**");    // Hi?n th? c?nh b�o ch�y
    sprintf(M, "Nhiet do: %d", nhietdo);  // �?nh d?ng chu?i nhi?t d?
    Lcd_Ghi_Lenh(0xc0);            // Di chuy?n con tr? d?n d�ng th? hai
    Lcd_Ghi_Chuoi1(M);
}

int main(){
    // C?u h�nh c�c ch�n I/O
    TRISA = 0x03;    // RA0, RA1 l�m d?u v�o (ADC)
    TRISD = 0x00;    // PORTD l�m d?u ra (LCD)
    TRISE = 0x00;    // PORTE l�m d?u ra (LCD)
    TRISB = 0b00000100; // RB2 l�m d?u v�o (n�t nh?n), RB5, RB6 l�m d?u ra (quat, bom)
    ADCON1 = 0x0e;   // C?u h�nh c�c ch�n l�m digital (tr? RA0, RA1)

    Lcd_Init();      // Kh?i t?o LCD

    while(1){
        // �?c gi� tr? ADC v� t�nh to�n nhi?t d?
        ADC();
        volt = (float)(adc * 5) / 1023;
        nhietdo = (int)(volt * 100);

        // Ki?m tra tr?ng th�i n�t nh?n RB2
        if (nut_nhan == 0 && nut_nhan_cu == 1) {  // N?u n�t nh?n du?c nh?n
            baochay = 1;   // K�ch ho?t c? b�o ch�y
        } else if (nut_nhan == 1 && nut_nhan_cu == 0 && nhietdo < 50) {  
            // N?u nh? n�t ra v� nhi?t d? du?i 50 th� t?t b�o ch�y
            baochay = 0;
        }

        // C?p nh?t tr?ng th�i n�t nh?n cu
        nut_nhan_cu = nut_nhan;

        // �i?u khi?n bom v� qu?t d?a tr�n nhi?t d? v� tr?ng th�i b�o ch�y
        if (nhietdo >= 50) {
            // N?u nhi?t d? >= 50 d?, t? d?ng b�o ch�y v� m? bom, qu?t
            baochay = 1;
            bom = 1;
            quat = 1;
        } else if (baochay == 0) {
            // N?u nhi?t d? du?i 50 v� kh�ng c� b�o ch�y, t?t bom v� qu?t
            bom = 0;
            quat = 0;
        }

        // Hi?n th? th�ng tin l�n LCD
        if (baochay == 0) {
            chedo_thuong();  // Ch? d? b�nh thu?ng
        } else {
            chedo_baochay();  // Ch? d? b�o ch�y
        }
    }
}
