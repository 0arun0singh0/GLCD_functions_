
#define _XTAL_FREQ 16000000

#include <xc.h>




#define RS PORTBbits.RB0
#define RW PORTBbits.RB1
#define EN PORTBbits.RB2
#define CS1 PORTBbits.RB3
#define CS2 PORTBbits.RB4
#define RST PORTBbits.RB5
#define datatris TRISD
#define commandtris TRISB
#define dataport PORTD
#define commandport PORTB

/*
#define RS PORTCbits.RC2
#define RW PORTCbits.RC3
#define EN PORTCbits.RC4
#define CS1 PORTCbits.RC5
#define CS2 PORTCbits.RC6
#define RST PORTCbits.RC7
#define datatris TRISD
#define commandtris TRISC
#define dataport PORTD
#define commandport PORTC
  
  */

// REMEMBER IT SHIFTS COLUMN ON ITS OWN 

void enable_pulse();
void glcd_on();
void set_start_line(unsigned char);
void goto_col(unsigned char);
void goto_row(unsigned char);
void goto_xy(unsigned char, unsigned char);
void glcd_write(unsigned char);
unsigned char glcd_read(unsigned char);
void glcd_clear_line(unsigned char);
void glcd_clear();
void glcd_draw_pixel(unsigned char, unsigned char, unsigned char);
void glcd_text(unsigned char);
void glcd_num(unsigned int);
void glcd_string(unsigned char*);
void glcd_line(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void glcd_rect(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void glcd_circle(int, int, int, int, int);




void enable_pulse()
{
    EN = 1;
    __delay_us(5);
    EN = 0;
    __delay_us(5);
}

void glcd_on()
{
    datatris = 0x00;
    commandtris = 0x00;
    dataport = 0x00;
    commandport = 0x00;
    RST = 1;
    CS1 = 1;
    CS2 = 1;
    // Activate both chips
    CS1 = 1;            // Activate Left side
    CS2 = 1;            // Activate Right side
    RS = 0;             // Low --> command write
    RW = 0;             // Low --> Write data
    dataport = 0x3F;    // ON command               //00111111 code for screen on
    enable_pulse();
}

void set_start_line(unsigned char line)
{
    RS = 0;             //RS low --> command
    RW = 0;             //RW low --> write
    //Activate both chips
    CS1 = 1;
    CS2 = 1;
    dataport = 0xC0 | line;     //Set Start Line command
    enable_pulse();
}

void goto_col(unsigned char col)
{
    unsigned char Col_Data;
    RS = 0;              //RS low --> command
    RW = 0;              //RW low --> write
    if(col<64)             //left section
    {
        CS1 = 1;          //select chip 1
        CS2 = 0;          //deselect chip 2
        Col_Data = col;              //put column address on data port
    }
   else                 //right section
    {
 
        CS2 = 1;
        CS1 = 0;
        Col_Data = col-64;   //put column address on data port
    }
    Col_Data = (Col_Data | 0x40 ) & 0x7F;  //Command format
    dataport = Col_Data;
    enable_pulse();
}

void goto_row(unsigned char row)
{
    unsigned char Col_Data;
    RS = 0;                  //RS low --> command
    RW = 0;                  //RW low --> write
    Col_Data = (row | 0xB8 ) & 0xBF; //put row address on data port set command
    dataport = Col_Data;
    enable_pulse();
}

void goto_xy(unsigned char x,unsigned char y)
{
    goto_col(x);
    goto_row(y);
}

void glcd_write(unsigned char data)
{
    RS = 1;          //RS high --> data
    RW = 0;          //RW low --> write
    dataport = data;            //put data on data port
    __delay_us(5);
    enable_pulse();
}

void glcd_clear_line(unsigned char ln)
{
    int i;
    goto_xy(0,ln);      //At start of line of left side
    goto_xy(64,ln);     //At start of line of right side (Problem)
    CS1 = 1;
    for(i=0;i<65;i++)
    glcd_write(0);
}

void glcd_clear()
{
    unsigned short m;
    for(m=0;m<8;m++)
    {
        glcd_clear_line(m);
    }
}

void glcd_draw_pixel(unsigned char x, unsigned char y, unsigned char color)
{
    unsigned char Col_Data;;
    goto_xy(x,(y/8));
    switch (color)
    {
        case 0:         //Light spot
            Col_Data = ~(1<<(y%8)) & glcd_read(x);
        break;
        case 1:         //Dark spot
            Col_Data = (1<<(y%8)) | glcd_read(x);
        break;
    }
    goto_xy(x,(y/8));
    glcd_write(Col_Data);
}

unsigned char glcd_read(unsigned char column)
{
    unsigned short read_data = 0; //Read data here
    datatris = 0xFF;     //PORTD as Input
    RW = 1;             //Read
    RS = 1;             //Data
    CS1 = (column>63);
    CS2 = !CS1;         //Disable/Enable CS2
    __delay_us(5);        //tasu
    EN = 1;             //Latch RAM data into ouput register
    __delay_us(5);        //twl + tf
 
    //Dummy read
    EN = 0;             //Low Enable
    __delay_us(5);
    EN = 1;             //latch data from output register to data bus
    __delay_us(5);             //tr + td(twh)
 
    read_data = dataport;    //Input data
    EN = 0;             //Low Enable to remove data from the bus
    __delay_us(5);        //tdhr
    datatris = 0x00;     //Output again
    return read_data;
}


void glcd_text(unsigned char ch)
{
    switch(ch)
    {
        case 65:
            glcd_write(0x00);
            glcd_write(0x7E);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x7E);
            glcd_write(0x00);
            break;
            
        case 66:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x36);
            glcd_write(0x00);
            break;
            
        case 67:
            glcd_write(0x00);
            glcd_write(0x3E);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x22);
            glcd_write(0x00);
            break;
            
        case 68:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x3E);
            glcd_write(0x00);
            break;
            
        case 69:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x41);
            glcd_write(0x00);
            break;
            
        case 70:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x01);
            glcd_write(0x00);
            break;
            
        case 71:
            glcd_write(0x00);
            glcd_write(0x3E);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x49);
            glcd_write(0x3A);
            glcd_write(0x00);
            break;
            
        case 72:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x08);
            glcd_write(0x08);
            glcd_write(0x08);
            glcd_write(0x7F);
            glcd_write(0x00);
            break;
            
        case 73:
            glcd_write(0x00);
            glcd_write(0x00);
            glcd_write(0x41);
            glcd_write(0x7F);
            glcd_write(0x41);
            glcd_write(0x00);
            glcd_write(0x00);
            break;
            
        case 74:
            glcd_write(0x00);
            glcd_write(0x30);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x3F);
            glcd_write(0x00);
            break;
            
        case 75:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x08);
            glcd_write(0x14);
            glcd_write(0x22);
            glcd_write(0x41);
            glcd_write(0x00);
            break;
            
        case 76:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x00);
            break;
            
        case 77:   
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x02);
            glcd_write(0x0C);
            glcd_write(0x02);
            glcd_write(0x7F);
            glcd_write(0x00);
            break;
        
        case 78:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x02);
            glcd_write(0x04);
            glcd_write(0x08);
            glcd_write(0x7F);
            glcd_write(0x00);
            break;
            
        case 79:
            glcd_write(0x00);
            glcd_write(0x3E);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x41);
            glcd_write(0x3E);
            glcd_write(0x00);
            break;
            
        case 80:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x06);
            glcd_write(0x00);
            break;
            
        case 81:
            glcd_write(0x00);
            glcd_write(0x1E);
            glcd_write(0x21);
            glcd_write(0x21);
            glcd_write(0x21);
            glcd_write(0x5E);
            glcd_write(0x00);
            break;
            
        case 82:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x09);
            glcd_write(0x76);
            glcd_write(0x00);
            break;
            
        case 83:
            glcd_write(0x00);
            glcd_write(0x26);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x32);
            glcd_write(0x00);
            break;
            
        case 84:
            glcd_write(0x00);
            glcd_write(0x01);
            glcd_write(0x01);
            glcd_write(0x7F);
            glcd_write(0x01);
            glcd_write(0x01);
            glcd_write(0x00);
            break;
            
        case 85:
            glcd_write(0x00);
            glcd_write(0x3F);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x40);
            glcd_write(0x3F);
            glcd_write(0x00);
            break;
            
        case 86:
            glcd_write(0x00);
            glcd_write(0x1F);
            glcd_write(0x20);
            glcd_write(0x40);
            glcd_write(0x20);
            glcd_write(0x1F);
            glcd_write(0x00);
            break;
            
        case 87:
            glcd_write(0x00);
            glcd_write(0x7F);
            glcd_write(0x20);
            glcd_write(0x10);
            glcd_write(0x20);
            glcd_write(0x7F);
            glcd_write(0x00);
            break;
            
        case 88:
            glcd_write(0x00);
            glcd_write(0x41);
            glcd_write(0x22);
            glcd_write(0x1C);
            glcd_write(0x22);
            glcd_write(0x41);
            glcd_write(0x00);
            break;
            
        case 89:
            glcd_write(0x00);
            glcd_write(0x07);
            glcd_write(0x08);
            glcd_write(0x70);
            glcd_write(0x08);
            glcd_write(0x07);
            glcd_write(0x00);
            break;
            
        case 90:
            glcd_write(0x00);
            glcd_write(0x61);
            glcd_write(0x51);
            glcd_write(0x49);
            glcd_write(0x45);
            glcd_write(0x43);
            glcd_write(0x00);
            break;
            
        case 48:
            glcd_write(0x00);
            glcd_write(0x3E);
            glcd_write(0x51);
            glcd_write(0x49);
            glcd_write(0x45);
            glcd_write(0x3E);
            glcd_write(0x00);
            break;
            
        case 49:
            glcd_write(0x00);
            glcd_write(0x04);
            glcd_write(0x02);
            glcd_write(0x7F);
            glcd_write(0x00);
            glcd_write(0x00);
            glcd_write(0x00);
            break;
            
        case 50:
            glcd_write(0x00);
            glcd_write(0x42);
            glcd_write(0x61);
            glcd_write(0x51);
            glcd_write(0x49);
            glcd_write(0x46);
            glcd_write(0x00);
            break;
            
        case 51:
            glcd_write(0x00);
            glcd_write(0x22);
            glcd_write(0x41);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x36);
            glcd_write(0x00);
            break;
            
        case 52: 
            glcd_write(0x00);
            glcd_write(0x18);
            glcd_write(0x14);
            glcd_write(0x12);
            glcd_write(0x7F);
            glcd_write(0x10);
            glcd_write(0x00);
            break;
            
        case 53:
            glcd_write(0x00);
            glcd_write(0x27);
            glcd_write(0x45);
            glcd_write(0x45);
            glcd_write(0x45);
            glcd_write(0x39);
            glcd_write(0x00);
            break;
            
        case 54:
            glcd_write(0x00);
            glcd_write(0x3E);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x32);
            glcd_write(0x00);
            break;
            
        case 55:
            glcd_write(0x00);
            glcd_write(0x01);
            glcd_write(0x01);
            glcd_write(0x71);
            glcd_write(0x09);
            glcd_write(0x07);
            glcd_write(0x00);
            break;
            
        case 56:
            glcd_write(0x00);
            glcd_write(0x36);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x36);
            glcd_write(0x00);
            break;
            
        case 57:
            glcd_write(0x00);
            glcd_write(0x26);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x49);
            glcd_write(0x3E);
            glcd_write(0x00);
            break;
            
        case ' ':
            glcd_write(0x00);
            glcd_write(0x00);
            glcd_write(0x00);
            glcd_write(0x00);
            glcd_write(0x00);
            break;
			
		case 124:
			glcd_write(0xFF);
            glcd_write(0xFF);
            glcd_write(0xFF);
            glcd_write(0xFF);
            glcd_write(0xFF);
			break;
			
		case 20:
			glcd_write(0xFF);
			break;
			
		case '*':
            glcd_write(0x00);
            glcd_write(0x0C);
            glcd_write(0x12);
            glcd_write(0x12);
            glcd_write(0x0C);
            glcd_write(0x00);
            break;
			
            
        default:
            break;
    }
}


void glcd_num(unsigned int num)
{
    int i,j,arr[10];
    
    i=0;
    while(num!=0)
    {
        
        arr [i]=num%10;
        num=num/10;
        i++;
        
    }
    
    
    for(j=i-1;j>=0;j--)
    {
        glcd_text(arr [j] + 48);
        
    }

}


void glcd_string(unsigned char* str)
{
    int i=0;
    
    while(str[i]!=0)
    {
        glcd_text(str[i]);
        i++;
        
    }
}
