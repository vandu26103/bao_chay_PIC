#include <p18f4520.h>  // Thu vi?n dành cho PIC18F4520
#include <delays.h>    // Thu vi?n t?o tr?
#include <adc.h>       // Thu vi?n ADC d? d?c giá tr? t? c?m bi?n nhi?t d?
#include <stdio.h>     // Thu vi?n s? d?ng hàm sprintf cho hi?n th?

// C?u hình các bit cho PIC18F4520
#pragma config OSC = HS         // Ch?n dao d?ng t?c d? cao (High-Speed Oscillator)
#pragma config MCLRE = ON       // B?t chân Master Clear (MCLR)
#pragma config WDT = OFF        // T?t Watchdog Timer
#pragma config PBADEN = OFF     // T?t tính nang analog cho các chân PORTB
#pragma config PWRT=ON          // B?t Power-up Timer
#pragma config BOREN=OFF        // T?t Brown-out Reset
#pragma config LVP=OFF          // T?t ch? d? l?p trình di?n áp th?p

#define quat PORTBbits.RB5      // Qu?t k?t n?i v?i chân RB5
#define bom PORTBbits.RB6       // Bom k?t n?i v?i chân RB6
#define nut_nhan PORTBbits.RB2  // Nút nh?n k?t n?i v?i chân RB2

// Hàm ghi l?nh lên LCD
void Lcd_Ghi_Lenh (char lenh)
{
    PORTEbits.RE0 = 0; // RS = 0 (Ch?n thanh ghi l?nh)
    PORTEbits.RE1 = 0; // RW = 0 (Ch? d? ghi)
    PORTEbits.RE2 = 1; // Kích ho?t chân EN
    PORTD = lenh;      // G?i l?nh lên PORTD (D0-D7)
    PORTEbits.RE2 = 0; // H?y kích ho?t EN
    Delay1KTCYx(100);  // Tr? 10ms
}

// Hàm ghi d? li?u lên LCD
void Lcd_Ghi_Dulieu (char dulieu)
{
    PORTEbits.RE0 = 1; // RS = 1 (Ch?n thanh ghi d? li?u)
    PORTEbits.RE1 = 0; // RW = 0 (Ch? d? ghi)
    PORTEbits.RE2 = 1; // Kích ho?t chân EN
    PORTD = dulieu;    // G?i d? li?u lên PORTD (D0-D7)
    PORTEbits.RE2 = 0; // H?y kích ho?t EN
    Delay1KTCYx(10);   // Tr? 10ms
}

// Hàm ghi chu?i ký t? lên LCD t? b? nh? chuong trình (ROM)
void Lcd_Ghi_Chuoi(const char rom *chuoi){
    while(*chuoi){
        Lcd_Ghi_Dulieu(*chuoi); // Ghi t?ng ký t?
        chuoi++;
    }
}

// Hàm ghi chu?i ký t? lên LCD t? b? nh? RAM
void Lcd_Ghi_Chuoi1(char *chuoi){
    while(*chuoi){
        Lcd_Ghi_Dulieu(*chuoi); // Ghi t?ng ký t?
        chuoi++;
    }
}

// Hàm kh?i t?o LCD
void Lcd_Init(void){
    Lcd_Ghi_Lenh(0x38);  // Ch? d? 8-bit, 2 dòng
    Lcd_Ghi_Lenh(0x06);  // T? d?ng d?ch con tr?
    Lcd_Ghi_Lenh(0x0c);  // B?t màn hình, t?t con tr?
    Lcd_Ghi_Lenh(0x01);  // Xóa màn hình
    Delay1KTCYx(10);     // Tr? 10ms
}

int adc;                // Bi?n luu giá tr? ADC
float volt;             // Bi?n luu giá tr? di?n áp d?c t? ADC
int nhietdo;            // Bi?n luu giá tr? nhi?t d?
char M[50];             // Chu?i t?m cho vi?c hi?n th?
int tt = 0;             // Tr?ng thái chuy?n d?i LCD
int baochay = 0;        // C? báo cháy (0 = không cháy, 1 = có cháy)
int nut_nhan_cu = 1;    // Bi?n luu tr?ng thái nút nh?n tru?c dó (m?c d?nh là chua nh?n - 1)

// Hàm d?c giá tr? t? c?m bi?n ADC
void ADC (void){
    OpenADC( ADC_FOSC_32 &        // C?u hình ADC clock
             ADC_RIGHT_JUST &     // Can ph?i k?t qu?
             ADC_12_TAD,          // Th?i gian l?y m?u
             ADC_CH0 &            // Ch?n kênh AN0
             ADC_INT_OFF &        // T?t ng?t ADC
             ADC_VREFPLUS_VDD &   // Ði?n áp tham chi?u duong là VDD
             ADC_VREFMINUS_VSS, 14 ); // Ði?n áp tham chi?u âm là VSS
    ConvertADC();                 // B?t d?u chuy?n d?i ADC
    while(BusyADC());             // Ch? chuy?n d?i hoàn t?t
    adc = ReadADC();              // Ð?c giá tr? ADC
    CloseADC();                   // Ðóng ADC
}

// Ch? d? hi?n th? thông tin bình thu?ng
void chedo_thuong(){
    if((nhietdo < 10) && (tt == 0)){
        Lcd_Ghi_Lenh(0x01);        // Xóa màn hình
        Delay1KTCYx(10);           // Tr? 10ms
        tt = 1;
    }
    else if (nhietdo > 10){
        tt = 0;
    }
    Lcd_Ghi_Lenh(0x80);            // Di chuy?n con tr? d?n d?u dòng
    Lcd_Ghi_Chuoi("**Nhom9 - DACN**");
    sprintf(M, "Nhiet do: %d", nhietdo);  // Ð?nh d?ng chu?i nhi?t d?
    Lcd_Ghi_Lenh(0xc0);            // Di chuy?n con tr? d?n dòng th? hai
    Lcd_Ghi_Chuoi1(M);
}

// Ch? d? hi?n th? thông tin khi có cháy
void chedo_baochay(){
    Lcd_Ghi_Lenh(0x01);            // Xóa màn hình
    Lcd_Ghi_Lenh(0x80);            // Di chuy?n con tr? d?n d?u dòng
    Lcd_Ghi_Chuoi("**DANG CO CHAY**");    // Hi?n th? c?nh báo cháy
    sprintf(M, "Nhiet do: %d", nhietdo);  // Ð?nh d?ng chu?i nhi?t d?
    Lcd_Ghi_Lenh(0xc0);            // Di chuy?n con tr? d?n dòng th? hai
    Lcd_Ghi_Chuoi1(M);
}

int main(){
    // C?u hình các chân I/O
    TRISA = 0x03;    // RA0, RA1 làm d?u vào (ADC)
    TRISD = 0x00;    // PORTD làm d?u ra (LCD)
    TRISE = 0x00;    // PORTE làm d?u ra (LCD)
    TRISB = 0b00000100; // RB2 làm d?u vào (nút nh?n), RB5, RB6 làm d?u ra (quat, bom)
    ADCON1 = 0x0e;   // C?u hình các chân làm digital (tr? RA0, RA1)

    Lcd_Init();      // Kh?i t?o LCD

    while(1){
        // Ð?c giá tr? ADC và tính toán nhi?t d?
        ADC();
        volt = (float)(adc * 5) / 1023;
        nhietdo = (int)(volt * 100);

        // Ki?m tra tr?ng thái nút nh?n RB2
        if (nut_nhan == 0 && nut_nhan_cu == 1) {  // N?u nút nh?n du?c nh?n
            baochay = 1;   // Kích ho?t c? báo cháy
        } else if (nut_nhan == 1 && nut_nhan_cu == 0 && nhietdo < 50) {  
            // N?u nh? nút ra và nhi?t d? du?i 50 thì t?t báo cháy
            baochay = 0;
        }

        // C?p nh?t tr?ng thái nút nh?n cu
        nut_nhan_cu = nut_nhan;

        // Ði?u khi?n bom và qu?t d?a trên nhi?t d? và tr?ng thái báo cháy
        if (nhietdo >= 50) {
            // N?u nhi?t d? >= 50 d?, t? d?ng báo cháy và m? bom, qu?t
            baochay = 1;
            bom = 1;
            quat = 1;
        } else if (baochay == 0) {
            // N?u nhi?t d? du?i 50 và không có báo cháy, t?t bom và qu?t
            bom = 0;
            quat = 0;
        }

        // Hi?n th? thông tin lên LCD
        if (baochay == 0) {
            chedo_thuong();  // Ch? d? bình thu?ng
        } else {
            chedo_baochay();  // Ch? d? báo cháy
        }
    }
}
