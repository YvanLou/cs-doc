
#include "usb_task.h"
#include "usb_kbd_enum.h"
#include "ext_5131.h"
#include "reg_5131.h"
//////////////////////////////
code struct usb_st_device_descriptor usb_device_descriptor =
{
  sizeof(usb_device_descriptor), 0x01, 0x1001, 0,
  0, 0, 32, 0xeb03, 0x0320,
  0x0001, 0x01, 0x02, 0x03, 1
};
/*STRING描述符*/
code struct usb_st_manufacturer usb_manufacturer =
{ sizeof(usb_manufacturer),  0x03, {'X'<<8, 'I'<<8, 'I'<<8, 'Z'<<8, 'X'<<8} };

code struct usb_st_product usb_product =
{ sizeof(usb_product),       0x03, {'A'<<8, 'T'<<8, '8'<<8, '9'<<8, 'C'<<8, '5'<<8, '1'<<8, \
                               '3'<<8, '1'<<8, ' '<<8, \
                               'D'<<8, 'D'<<8, 'S'<<8, ' '<<8, \
                               } };

code struct usb_st_serial_number usb_serial_number =
{ sizeof(usb_serial_number), 0x03, {'1'<<8, '.'<<8, '2'<<8, '.'<<8, '3'<<8} };

code struct usb_st_language_descriptor usb_language =
{ sizeof(usb_language),      STRING, 0x0904 };
/*配置描述符集合*/
code struct  
{ struct usb_st_configuration_descriptor  cfg;
  struct usb_st_interface_descriptor      ifc;
  struct usb_st_hid_descriptor            hid ;
  struct usb_st_endpoint_descriptor       ep1 ;
  unsigned char                           rep[0x3b] ;
}
  usb_configuration =
  {
    { 9, 0x02, 0x2200, 1, 1,0, 0x80, 50},
    { 9, 0x04, 1, 0, 1, 0x03, 0, 0, 0 },
    { 9, 0x21, 0x1101, 8, 1, 0x22, 0x3B00 },
    { 7, 0x05, 0x01, 0x03, 32, 0x20 },
   
  };
unsigned char Bufin[64];
static  unsigned char   endpoint_status[2];
static  unsigned char   *pbuffer;
static  unsigned char   bmRequestType;
unsigned char   usb_configuration_nb;
extern  bit     usb_connected;
bit   usb_connected_stored;
bit   usb_configured_stored;
/////////////////////////////
void main (void)
{
  EA = 1;                 //使能中断
  init_timer0();         //定时器初始化
  delay(10);
  usb_task_init();       //usb控制器初始化
  while(1)               //任务循环
  {
  usb_task();           //usb任务处理子程序  
  }
}

void init_timer0()
{
  TMOD=0x05;
  TH0=0;
  TL0=0;
}
void delay(int n)
{
  TH0=n;
  TL0=0;
  TR0=1;
  if(TH0>1);
  return;
}

void usb_task_init(void)
{ 
  USBCON |= 0x80; //使能USB控制器
  USBCON |= 0x10; /*USB软件插拔*/
  delay(100);
  USBCON &= ~0x10;
  PLLDIV = 32; //配置控制器时钟
  PLLCON |= 0x02;//使能PLL
  //usb_configure_endpoint(EP_CONTROL, CONTROL);//配置控制端口
  UEPNUM = 0;//复位控制端口
  UEPCONX = 0;
  /*初始化状态变量*/
  usb_connected = 0;
  usb_connected_stored = 1;
  usb_configured_stored = 0;
  endpoint_status[0] = 0x00;
  endpoint_status[1] = 0x00;
  usb_connected = 0;
  usb_configuration_nb = 0x00;
}
/////////////////////////////////
void usb_task(void)
{
             if (UEPINT & 0)   usb_enumeration_process();
	         
			 if (UEPINT & 1)   
			 {
                 unsigned char data i;                 
                                  
                 i = ReadEp(2,Bufin);	
              switch(Bufin[0])
               {
                 case 1:
                   singletone_set();
                    break;
                 case 2:
                   unrampedfsk_set();
                   break;
                 case 3:
                  rampedfsk_set();
                   break;
                  case 4:
                  chirp();
                   break;
                 case 5:
                  bpsk();
                  break;
                } 	 
                
	         }
  
}
/*usb_enumeration_process ()*/

void usb_enumeration_process (void)//USB设备枚举过程
{ 
  UEPNUM = 0;//选择控制端口
  bmRequestType = UEPDATX;          /* 得到 bmRequestType */
  switch (UEPDATX)                  /* 判断 bRequest 的值 */
  {
    case GET_DESCRIPTOR:                   /*获得描述符请求*/
      usb_get_descriptor();
      break;
    case GET_CONFIGURATION:               /*获得配置请求*/
      usb_get_configuration();
      break;
    case SET_ADDRESS:                    /*设置地址请求*/
      usb_set_address();
      break;
    case SET_CONFIGURATION:              /*设置配置请求或设置HID报表请求*/
      if (bmRequestType == 0) { usb_set_configuration(); }
      else { usb_hid_set_report(); }
      break;
    case CLEAR_FEATURE:                 /*清除特性请求*/
      usb_clear_feature();
      break;
    case SET_FEATURE:                   /*设置特性请求*/
      usb_set_feature();
      break;
    case GET_STATUS:                    /*获得状态请求*/
      usb_get_status();
      break;
    case GET_INTERFACE:                 /*获得接口信息请求或设置响应间隔请求*/
      if (bmRequestType == 0x81) { usb_get_interface(); } 
      else { usb_hid_set_idle(); }
      break;
    case HID_GET_IDLE:                  /*获得响应间隔请求*/
      usb_hid_get_idle();
      break;
    case SET_DESCRIPTOR:
    case SET_INTERFACE:
    case SYNCH_FRAME:
    default:
      UEPSTAX =UEPSTAX & ~0x04;        /*清除SETUP标志*/
      UEPSTAX =UEPSTAX | 0x20;         /*中止请求设置*/               
      while (!UEPSTAX & 0x08);
      UEPSTAX =UEPSTAX &~ 0x20;
      UEPSTAX =UEPSTAX &~ 0x08;
      break;
    }
    UEPSTAX = UEPSTAX &~0x80;
}
////////////*get_descriptor*///////////////
void usb_get_descriptor (void)
{
  unsigned char   data_to_transfer;
  unsigned  int  wLength;
  unsigned char   descriptor_type;
  unsigned char   string_type;                      
  string_type = UEPDATX;            /* 读取 wValue的低位 */
  descriptor_type = UEPDATX;        /* 读取 wValue的高位 */
  switch (descriptor_type)          /*判断描述符类型*/
  {
    case DEVICE:                            /*设备描述符�*/                
    {
      data_to_transfer = sizeof (usb_device_descriptor);
      pbuffer = &(usb_device_descriptor.bLength);//指向设备描述符结构首地址
      break;
    }

    case CONFIGURATION:                       /*配置*/
    {
      data_to_transfer = sizeof (usb_configuration);
      pbuffer = &(usb_configuration.cfg.bLength);
      break;
    }

    case REPORT:                            /*报表*/
    {
      data_to_transfer = 0x3b;
      pbuffer = &(usb_configuration.rep[0]);
      break;
    }

    case HID:                              /*HID*/
    {
      data_to_transfer = sizeof(usb_configuration.hid);
      pbuffer = &(usb_configuration.hid.bLength);
      break;
    }
    case STRING:                          /*STRING*/
    {
      switch (string_type)
      {
        case 0x00:
        {
          data_to_transfer = sizeof (usb_language);
          pbuffer = &(usb_language.bLength);
          break;
        }
        case 0x01:
        {
          data_to_transfer = sizeof (usb_manufacturer);
          pbuffer = &(usb_manufacturer.bLength);
          break;
        }
        case 0x02:
        {
          data_to_transfer = sizeof (usb_product);
          pbuffer = &(usb_product.bLength);
          break;
        }
        case 0x03:
        {
          data_to_transfer = sizeof (usb_serial_number);
          pbuffer = &(usb_serial_number.bLength);
          break;
        }
        default:
        {
          UEPSTAX =UEPSTAX & ~0x04;
          UEPSTAX=UEPSTAX | 0x20; 
          while ((!(UEPSTAX & 0x08)) && (UEPSTAX & 0x04));
          UEPSTAX &= ~0x08;
          UEPSTAX &= ~0x20;
          UEPSTAX &= ~0x80;
          return;
        }
      }
      break;
    }

    default:
    {
      UEPSTAX &= ~0x04;
      UEPSTAX |= 0x20;
      while ((!(UEPSTAX & 0x08)) && (UEPSTAX & 0x04));
      UEPSTAX &= ~0x08;
      UEPSTAX &= ~0x20;
      UEPSTAX &= ~0x80;
      return;
    }
  }

  ACC = UEPDATX;                   
  ACC = UEPDATX;
  ((unsigned char*)&wLength)[1] = UEPDATX;   /* 读取要传输的长度 */
  ((unsigned char*)&wLength)[0] = UEPDATX;
  if (wLength > data_to_transfer)     /* 读取的长度大于描述符长度时 */      
  {
                   
  }
  else
  {
    data_to_transfer = (unsigned char)wLength;       /*传送需要的数据长度 */
  }
  UEPSTAX &= ~0x04 ;                    
  UEPSTAX |= 0x80;                            
  while (data_to_transfer > 32)/*传送的长度大于控制端口的长度时*/
  {
    pbuffer = usb_send_ep0_packet(pbuffer, 32);/*传送一次端口长度的数据*/
    data_to_transfer -= 32;
    while ((!(UEPSTAX & 0x42)) && (!(UEPSTAX & 0x01)));/*传输没有完成*/
    UEPSTAX=UEPSTAX & 0x01;
    if ((UEPSTAX & 0x42))               
    {
      UEPSTAX &= ~0x10;
      UEPSTAX &= ~0x02;
      return;
    }
  }
  /* 传送最后一次数据 */
  pbuffer = usb_send_ep0_packet(pbuffer, data_to_transfer);
  data_to_transfer = 0;
  while ((!(UEPSTAX & 0x42)) && (!(UEPSTAX & 0x01)));
  UEPSTAX=UEPSTAX & 0x01;
  if ((UEPSTAX & 0x42))                  /* if cancel from USB Host */
  {
    UEPSTAX &= ~0x10;
    UEPSTAX &= ~0x02;
    return;
  }
}
//////-----void usb_get_configuration (void)------////////
void usb_get_configuration (void)
{
  UEPSTAX &= ~0x04;
  UEPSTAX |= 0x80;
  UEPDATX = 0;
  UEPSTAX |= 0x10;
  while (!(UEPSTAX & 0x01));
  UEPSTAX=UEPSTAX & ~0x01;
  while (!(UEPSTAX & 0x42));
  UEPSTAX &= ~0x02;
  UEPSTAX &= ~0x80;
}
/////////////----set_address-----//////////
void usb_set_address (void)                 /*设置设备地址子程序*/
{
  unsigned char address;
  address = UEPDATX;                    /* 获得系统分配的设备地址 */
  UEPSTAX &= ~0x04;                     /*清除SETUP标志*/
  UEPSTAX |= 0x10;                       /* 返回0字节状态字 */
  USBCON |= 0x01;                         /*设置地址使能*/
  while (!(UEPSTAX & 0x01));
  UEPSTAX=UEPSTAX & ~0x01;
  USBADDR = (0x80 | address);              /*配置设备地址*/
}
////------set_config-------////////
void usb_set_configuration (void)
{
  unsigned char configuration_number;
  configuration_number = UEPDATX;   /* 读取配置数*/
  UEPSTAX &= ~0x80;
  UEPSTAX &= ~0x04;                   /*清除SETUP标志*/
  if (configuration_number <= 1)
  {
    usb_configuration_nb = configuration_number;
  }
  else
  {
    UEPSTAX |= 0x20;            /*中止请求设置*/ 
    while (!UEPSTAX & 0x08);
    UEPSTAX &= ~0x20;
    UEPSTAX &= ~0x08;
    return;
  }

  UEPSTAX |= 0x10;        /* 返回0字节状态字*/
  while (!UEPSTAX & 0x01);
  UEPSTAX &= ~0x01;
  /* 设备端口配置 */
  UEPNUM = 1;
  UEPCONX = 0x87;
  UEPRST = 0x01;
  UEPRST = 0x00;
}
////-----usb_hid_set_report (void)------//////
void usb_hid_set_report (void)/*设置报表请求程序*/
{
  UEPSTAX &= ~0x04;
  while(!(UEPSTAX & 0x42));
  UEPSTAX &= ~0x02;
  UEPSTAX |= 0x10;                         
  while(!(UEPSTAX & 0x01));
  UEPSTAX &= ~0x01;
}

unsigned char * usb_send_ep0_packet (unsigned char* tbuf, unsigned char data_length)
{
unsigned char i;

  UEPNUM=0;
  for (i = data_length; i != 0 ; i--, tbuf++)
  { 
  UEPDATX=*tbuf; 
  }
  UEPSTAX |= 0x10;
  return tbuf;
}
//////////////////////////////////////
unsigned char ReadEp(unsigned char EpNum,unsigned char *Data)
{
	unsigned char data i=0;
	UEPNUM=EpNum;
	while(i<UBYCTLX)
	{
		Data[i++]=UEPDATX;
	}	
	UEPSTAX&=~(0x46);
	return(i);
}

void WriteEp(unsigned char EpNum,unsigned char nLength,unsigned char *Data)
{
	unsigned char data i=0;
	UEPNUM=EpNum;
	UEPSTAX|=0x1<<7;
	while(nLength--) UEPDATX=Data[i++];	
	UEPSTAX|=0x1<<4;
	while(!(UEPSTAX&0x1)) ;
	UEPSTAX&=(~(0x1));
}
void singletonge_set()
{
int freq_data1,freq_data2,freq_data3,i;
int freq_data4,freq_data5,freq_data6;
  freq_data1=Bufin[1];
  freq_data2=Bufin[2];
  freq_data3=Bufin[3];
  freq_data4=Bufin[4];
  freq_data5=Bufin[5];
  freq_data6=Bufin[6];
/*设置工作模式*/
   P1=0x00;
   P2=0x1F;
   P0=0x00;//单一频率工作模式
   P1=0x02;
   for(i=0;i<100;i++);
   P1=0x00;
    /*设置输出频率*/
   P2=0x04;
   P0=freq_data1;
   P1=0x02;
   for(i=0;i<100;i++);
   P1=0x00;
   P2=0x05;
  P0=freq_data2;
   P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x06;
  P0=freq_data3;
  P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x07;
  P0=freq_data4;
  P1=0x02;
 for(i=0;i<100;i++);
  P1=0x00;
  P2=0x08;
  P0=freq_data5;
  P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x09;
  P0=freq_data6;
  P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  /*UPCLOCK*/
  for(i=0;i<100;i++);
  P1=0x03;
 for(i=0;i<100;i++);
   P1=0x02;
}
void unrampedfsk_set()
{
  int i;
  int freq1_data1,freq1_data2,freq1_data3,freq1_data4;
  int freq1_data5,freq1_data6,freq2_data1,freq2_data2;
  int freq2_data3,freq2_data4,freq2_data5;
  int freq2_data6,fsk_gap;
 freq1_data1= Bufin[1]; freq1_data2= Bufin[2]; freq1_data3= Bufin[3]; freq1_data4= Bufin[4]; freq1_data5= Bufin[5]; freq1_data6= Bufin[6]; freq2_data1= Bufin[7]; freq2_data2= Bufin[8]; freq2_data3= Bufin[9]; freq2_data4= Bufin[10]; freq2_data5= Bufin[11]; freq2_data6= Bufin[12]; fsk_gap= Bufin[13];
/*设置工作模式*/
     P1=0x00;
     P2=0x1F;
      P0=0x02;//UNRAMPED FSK工作模式，外部触发UPCLOCK
     P1=0x02;
     for(i=0;i<100;i++);
      P1=0x00;
/*设置频率1和频率2*/
     P2=0x04;
     P0=freq1_data1;
      P1=0x02;
     for(i=0;i<100;i++);
   P1=0x00;
   P2=0x05;
   P0=freq1_data2;
   P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x06;
  P0=freq1_data3;
  P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x07;
  P0=freq1_data4;
  P1=0x02;
  for(i=0;i<100;i++);
  P1=0x00;
  P2=0x08;
  P0=freq1_data5;
P1=0x02;
  for(i=0;i<100;i++);
P1=0x00;
P2=0x09;
P0=freq1_data6;
P1=0x02;
  for(i=0;i<100;i++);
P1=0x00;
//频率字2
P2=0x0A;
   P0=freq2_data1;
    P1=0x02;
    for(i=0;i<100;i++);
P1=0x00;
P2=0x0B;
P0=freq2_data2;
P1=0x02;
  for(i=0;i<100;i++);
P1=0x00;
P2=0x0C;
P0=freq2_data3;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x0D;
P0=freq2_data4;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x0E;
P0=freq2_data5;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x0F;
P0=freq2_data6;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*UPCLOCK*/
for(i=0;i<100;i++);
P1=0x03;
for(i=0;i<100;i++);
P1=0x02;
/*FSK*/
while(1)
{
P1=0x02;
for(i=0;i<100;i++);
P1=0x0A;
for(i=0;i<100;i++);
}
}
void chirp()
{
int i;
int freq_data1,freq_data2,freq_data3;
int freq_data4,freq_data5,freq_data6;
int  freq_delt1, freq_delt2, freq_delt3;
int  freq_delt4, freq_delt5, freq_delt6; 
int  freq_ramp1, freq_ramp2, freq_ramp3;
freq_data1= Bufin[1]; freq_data2= Bufin[2]; freq_data3= Bufin[3]; freq_data4= Bufin[4]; freq_data5= Bufin[5]; freq_data6= Bufin[6]; freq_delt1= Bufin[7]; freq_delt2= Bufin[8]; freq_delt3= Bufin[9]; freq_delt4= Bufin[10]; freq_delt5= Bufin[11]; freq_delt6= Bufin[12]; freq_ramp1= Bufin[13]; freq_ramp2= Bufin[14]; freq_ramp3= Bufin[15];
/*设置工作模式*/
P1=0x00;
P2=0x1F;
P0=0x06;//chirp工作模式，外部触发UPCLOCK，CLR ACC1
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
       /*设置频率字*/
P2=0x04;
P0=freq_data1;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x05;
P0=freq_data2;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x06;
P0=freq_data3;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x07;
P0=freq_data4;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x08;
P0=freq_data5;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x09;
P0=freq_data6;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*设置间隔频率*/
P2=0x10;
P0=freq_delt1;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x05;
P0=freq_delt2;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x12;
P0=freq_delt3;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x13;
P0=freq_delt4;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x14;
P0=freq_delt5;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x15;
P0=freq_delt6;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*ramped*/
P2=0x1A;
P0=freq_ramp1;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x1B;
P0=freq_ramp2;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x1C;
P0=freq_ramp3;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*UPCLOCK*/
while(1)
{
for(i=0;i<100;i++);
P1=0x03;
for(i=0;i<100;i++);
P1=0x02;
for(i=0;i<100;i++);
}
}
void bpsk()
{
int i;

int phase1_data1,phase1_data2;
int phase2_data1,phase2_data2;
int bpsk;
 phase1_data1= Bufin[1]; phase1_data2= Bufin[2]; phase2_data1= Bufin[3]; phase2_data2= Bufin[4]; bpsk =Bufin[11]; 

/*工作模式*/
P1=0x00;
P2=0x1F;
P0=0x08;//BPSKp工作模式，外部触发UPCLOCK，
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*相位设定*/
P2=0x00;
P0=phase1_data1;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x01;
P0= phase1_data2;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x02;
P0= phase2_data1;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
P2=0x03;
P0= phase2_data2;
P1=0x02;
for(i=0;i<100;i++);
P1=0x00;
/*UPCLOCK*/
for(i=0;i<100;i++);
P1=0x03;
for(i=0;i<100;i++);
P1=0x02;
/*BPSK*/
/*UPCLOCK*/
while(1)
{
P1=0x02;
for(i=0;i<bpsk;i++);
P1=0x0A;
for(i=0;i<bpsk;i++);
}
}

