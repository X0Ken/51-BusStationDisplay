#include <stdio.h>
#include <math.h>
#include <reg51.h>
#include <string.h>


/******************************************************************/
/*                    ����ӿ���Ϣ                                */
/******************************************************************/

#define  SYS_STOP 0
#define  SYS_RUNING 1
#define  BUS_STOP 0
#define  BUS_RUNING 1
#define  BUS_DIRECTION_UP 1
#define  BUS_DIRECTION_DOWN -1
#define  KEY P1

sbit    LCD_RS =P2^4;
sbit    LCD_WRD=P2^5;
sbit    LCD_E=  P2^6;
sbit    LCD_ENABLE=  P2^7;
sbit    led =P2^0;


unsigned char *next_msg="ǰ����վ��";
unsigned char *stop_msg="������վ��";
unsigned char *dir_msg="��ʻ·�ߣ�";
char station_index = 0;
char bus_direction = BUS_DIRECTION_UP;
char bus_status = BUS_STOP;
char work_status = SYS_STOP;
char all_station = 6;

unsigned char *station_data[]={
  "��վһ",
  "��վ��",
  "��վ��",
  "��վ��",
  "��վ��",
  "��վ��",
};
unsigned char *dir_data[]={
  "����",
  "����"
};

/******************************************************************/
/*                    ��������                                    */
/******************************************************************/
void TransferData(char data1,bit DI);
void delay(unsigned int m);
void initinal();
void show_title();
void show_station();
void set_next_index();
void bus_stop();
void bus_running();
void display();
void system_stop();
void system_running();
void show_dir();


/******************************************************************/
/*                    ������                                      */
/******************************************************************/
int  main(void)
{
  initinal();
  while(1)
  {
    if (work_status == SYS_RUNING) {
      display();
    }
    while (!~KEY);
    led = 0;
    switch (KEY) {
      case 0xFE:
        if (work_status == SYS_RUNING) {
          bus_running();
        }
        break;
      case 0xFD:
        if (work_status == SYS_RUNING) {
          bus_stop();
        }
        break;
      case 0xFB:
        if (work_status == SYS_RUNING) {
          system_stop();
        }
        break;
      case 0xF7:
        if (work_status == SYS_STOP) {
          system_running();
        }
        break;
      default:
        break;
    }
    while (~KEY);
    led = 1;
  }
}

void system_stop(){
  char i;
  work_status = SYS_STOP;
  TransferData(0x80, 0);
  delay(100);
  for (i = 0; i < 16; i++) {
    TransferData(0, 1);
  }
  TransferData(0x90, 0);
  delay(100);
  for (i = 0; i < 16; i++) {
    TransferData(0, 1);
  }
  TransferData(0x98, 0);
  delay(100);
  for (i = 0; i < 16; i++) {
    TransferData(0, 1);
  }
  TransferData(0x88, 0);
  delay(100);
  for (i = 0; i < 16; i++) {
    TransferData(0, 1);
  }
  LCD_ENABLE = SYS_STOP;
}

void system_running(){
  work_status = SYS_RUNING;
  LCD_ENABLE = SYS_RUNING;
  station_index = 0;
  bus_direction = BUS_DIRECTION_UP;
  bus_status = BUS_STOP;
}

void display(){
  if (work_status == SYS_RUNING) {
    show_title();
    show_station();
    show_dir();
  }
}

void bus_stop(){
  bus_status = BUS_STOP;
}

void bus_running(){
  bus_status = BUS_RUNING;
  set_next_index();
}

void set_next_index(){
  if (bus_direction == BUS_DIRECTION_UP && station_index + bus_direction >= all_station) {
    bus_direction = BUS_DIRECTION_DOWN;
  }
  else if (bus_direction == BUS_DIRECTION_DOWN && station_index + bus_direction < 0) {
    bus_direction = BUS_DIRECTION_UP;
  }
  station_index += bus_direction;
}

void show_title(){
  char i = 0;
  TransferData(0x80, 0);
  delay(100);
  if (bus_status == BUS_RUNING) {
    while(next_msg[i] && i < 16){
      TransferData(next_msg[i], 1);
      i++;
    }
  }
  else{
    while(stop_msg[i] && i < 16){
      TransferData(stop_msg[i], 1);
      i++;
    }
  }
}

void show_dir(){
  char i = 0;
  TransferData(0x88, 0);
  delay(100);
  while(dir_msg[i] && i < 16){
    TransferData(dir_msg[i], 1);
    i++;
  }
  TransferData(0x98, 0);
  delay(100);
  i = 0;
  if (bus_direction == BUS_DIRECTION_UP) {
    while(dir_data[0][i] && i < 16){
      TransferData(dir_data[0][i], 1);
      i++;
    }
  }
  else{
    while(dir_data[1][i] && i < 16){
      TransferData(dir_data[1][i], 1);
      i++;
    }
  }
}

void show_station(){
  char i = 0;
  TransferData(0x90, 0);
  delay(100);
  while(station_data[station_index][i] && i < 16){
    TransferData(station_data[station_index][i], 1);
    i++;
  }
}


/******************************************************************/
/*                    ��������                                    */
/******************************************************************/
void   TransferData(char data1,bit DI)  //�������ݻ�������,��DI=0��,��������,��DI=1,��������.
{
  LCD_WRD=0;
  LCD_RS=DI;
  delay(1);
  P0=data1;
  LCD_E=1;
  delay(1);
  LCD_E=0;
}

void    delay(unsigned int m)            //��ʱ����
{
  unsigned int i,j;
  for(i=0;i<m;i++)
     for(j=0;j<10;j++);
}

void   initinal(void)           //LCD�ֿ��ʼ������
{
   TransferData(0x30,0);  //Extended Function Set :8BIT����,RE=0: basic instruction set, G=0 :graphic display OFF
   delay(100);            //����100uS����ʱ����
   TransferData(0x30,0);  //Function Set
   delay(37);             ////����37uS����ʱ����
   TransferData(0x08,0);  //Display on Control
   delay(100);            //����100uS����ʱ����
   TransferData(0x10,0);  //Cursor Display Control�������
   delay(100);            //����100uS����ʱ����
   TransferData(0x0C,0);  //Display Control,D=1,��ʾ��
   delay(100);            //����100uS����ʱ����
   TransferData(0x01,0);  //Display Clear
   delay(10);             //����10mS����ʱ����
   TransferData(0x06,0);  //Enry Mode Set,�����������1λ�ƶ�
   delay(100);            //����100uS����ʱ����
}
