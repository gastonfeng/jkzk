/*==============================*/
/*	Jkzk.C			*/
/*	2000.6.20		*/

/*==============*/
/*  包含头文件  */
/*==============*/
#include <AT89x52.h>
#include <stdarg.h>
#include <intrins.h>
#include "ZK110V.h"

/*==============*/
/*  系统宏定义  */
/*==============*/
#define FREQ    11.0592
#define DELAY_SCALE 100 /*1000/(12/FREQ)*/
#define TIMEBASE    50     /*  定时中断时间,单位ms */
#define T0_INIT     19457   /*0xffff-(TIMEBASE*1000)/(12/FREQ)  */
#define WAITTIME 10
#define COMP_ZERO   200

/*==================*/
/*  系统变量定义    */
/*==================*/

unsigned char load,solar_state;
unsigned char Delay_change,Delay_disconnect;
unsigned char sec,time,min,hour;
unsigned char porta,portb;
idata unsigned char buf[6];               /*  接收缓冲区，这是一个先进先出对列  */
unsigned char point;
unsigned long chargeah,dischargeah;
unsigned int btv_H,btv_L;
unsigned int btv;
unsigned char please;
unsigned int adresult;
bit OutDevice;
bit newday,newhour;                           /*  每天 00:00 改变此标志,存当天数据     */
bit IsSolar;
bit received;
bit TEST;
bit TXEN;
bit DISPLAY;
bit ALARMSOUND;


/*==================*/
/*  输入输出口定义  */
/*==================*/
sbit Do=0x94;
sbit di=0x95;
sbit clk=0x96;
sbit cs=0x97;


sbit RS=0xa0;
sbit rw=0xa1;
sbit e=0xa2;
sbit paen=0xa3;
sbit pben=0xa4;
sbit slk=0xa5;		//display clock
sbit dta=0xa6;		// display data
sbit display_pb=0xa7;	//display button

sbit cs0=0xb2;
sbit wdg=0xb4;
sbit dout=0xb5;
sbit out_en=0xb6;	//P3.6
sbit sk=0xb7;


/*======================*/
/*      函数预说明      */
/*======================*/
lcd_init(void);
/*monitor*/ aprintf(char *format,...) reentrant;
delay(unsigned int times);
/*monitor*/ set_eprom(unsigned char command);
/*monitor*/ unsigned int eprom(unsigned char address);
unsigned char wait_key();
/*monitor*/ write_eprom(unsigned char address,unsigned int Data);
sendstr(unsigned char *Data);
unsigned char key();
sendpacket(unsigned char,unsigned int);
void talk(void);
void outchar(char);

arg(){
//    if(eprom(DAY)>=400)while(1){
//        lcd_init();
//        aprintf("ERROR 601");
//    }
//	wdg=~wdg;
    write_eprom(1,eprom(TAPER));
    write_eprom(2,(eprom(TAPER)*4+eprom(BOOST))/5);
    write_eprom(3,(eprom(TAPER)*3+eprom(BOOST)*2)/5);
    write_eprom(4,(eprom(TAPER)*2+eprom(BOOST)*3)/5);
    write_eprom(5,(eprom(TAPER)+eprom(BOOST)*4)/5);
    write_eprom(6,eprom(BOOST));
}

/*==============*/
/*  系统初始化  */
/*==============*/
init()
{
    unsigned char i;
    out_en=0;
 //   portb=0;
 //   portb|=LOAD;
 //   P0=portb;
 //   porta=0;
 //   paen=0;
 //   pben=0;
 //   port=P0=LAMP;
 //   port_out_en=1;
 //   port_out_en=0;
    porta=portb=P0=LAMP;
    paen=pben=1;
    paen=pben=0;
    load=0;                   
	ALARMSOUND=1;
    for(i=0;i<12;i++)outchar(3);
    write_eprom(BT_V_MIN,10240);
    /*  LCD显*/
    OutDevice=LCD;
    lcd_init();
    /*  系统状态判别    */
#if MODEL==70    
    aprintf("\a   JRT-48-100");
#endif
#if MODEL==71
    aprintf("\a   JRT-24-100");
#endif
#if MODEL==72
	aprintf("\a  JRT-2002-220");
#endif
#if MODEL==73
	aprintf("\a  JRT-2002-110");
#endif
	aprintf("\nSystem Init...");
    delay(8000);
    TXEN=1;
    point=0;
    if(eprom(SYSTEM_FLAG)!=MODEL){
        aprintf("\aEPROM ERROR!\nLoad Default...");
        wait_key();
        write_eprom(BOOST,BOOST_DEFAULT_VALUE);
        write_eprom(TAPER,TAPER_DEFAULT_VALUE);
        write_eprom(FLOAT_MAX_S,FLOAT_MAX_DEFAULT_VALUE);
        write_eprom(FLOAT_MIN_S,FLOAT_MIN_DEFAULT_VALUE);
        write_eprom(DELAY_CHANGE,DELAY_CHANGE_DEFAULT_VALUE);
        write_eprom(BOOST_BELOW,BOOST_BELOW_DEFAULT_VALUE);
        write_eprom(COMP_TEMP,COMP_TEMP_DEFAULT_VALUE);
        write_eprom(BT_MIN,BT_MIN_DEFAULT_VALUE);
        write_eprom(BT_MAX,BT_MAX_DEFAULT_VALUE);
        write_eprom(BT_RELOAD,BT_RELOAD_DEFAULT_VALUE);
        write_eprom(DELAY_DISCONNECT,DELAY_DISCONNECT_DEFAULT_VALUE);
        for(i=PASSWORD;i<PASSWORD+8;i++)write_eprom(i,'8');
        for(i=CLIP;i<CLIP+16;i++)write_eprom(i,128);
        write_eprom(SYSTEM_FLAG,MODEL);
    }
    arg();                                
    /*  定时器0初始化    */
//    IE=0x82;
	IE=0x12;
	IP=0x10;
    SCON=0x70;
    TMOD=0x21;
    TH1=0xfd;
    PCON=0;
    TR0=TR1=1;
    EA=1;
}



/*monitor*/ int ad(unsigned char ch)
{
    union{
        unsigned char i[2];
        unsigned long l;
    }a;
    unsigned int result,sum;
    porta=((porta&0xf0)|ch);
    P0=porta;
    paen=1;
    paen=0;
    sum=0;
    for(a.i[0]=0;a.i[0]<255;a.i[0]++);
    for(a.i[0]=0;a.i[0]<16;a.i[0]++){
    	result=0;
    	sk=0;
    	dout=1;
    	cs0=0;
    	while(!dout);
    	sk=1;
    	sk=0;
    	sk=1;
    	result|=dout;
    	sk=0;
    	for(a.i[1]=0;a.i[1]<11;a.i[1]++){
        	sk=1;
        	result=(result<<1)|dout;
        	sk=0;
        }
    	cs0=sk=dout=1;
    	sum+=result;
    	wdg=~wdg;
    }
    result=sum/16;
//    a.l=128+eprom(CLIP+ch);
//    a.l=result*a.l;
//    result=a.l/1024;
    return result;
}

        /*
monitor ad_get(unsigned char ch)
{
        if(ch==LOAD_V)return ad_get(BT_V)-ad(ch);
        if(ch==TEMP_BT)return (ad(ch)*4-2732-COMP_ZERO);
        if(ch==BT_V)return ad(ch)-(eprom(COMP_TEMP)*(MODEL==71?12:24)*ad_get(TEMP_BT));
        if(ch==LOAD_I)return ad(ch)/2;
        if(ch==BT_I)return ad(ch);
        if(ch==SR_I0)portb=1;ch=BT_I;
        if(ch==SR_I1)portb=2;ch=BT_I;
        if(ch==SR_I2)portb=4;ch=BT_I;
        if(ch==SR_I3)portb=8;ch=BT_I;
        if(ch==SR_I4)portb=0x10;ch=BT_I;
        if(ch==SR_I5)portb=0x20;ch=BT_I;
    if(load!=0)portb|=LOAD;else portb&=~LOAD;
    P0=portb;
    pben=1;
    pben=0;
    return ad(ch);
}       */
/*monitor*/ ad_get(unsigned char ch)
{
	unsigned int tem;
//    EA=0;
    switch(ch){
        case LOAD_V:
//        	tem=ad(ch)/4;
//        	if(btv>tem)return btv-tem;
//        	return 0;
			return (ad(ch)*4)/5;
        case SR_V0:
        case SR_V1:
        case SR_V2:
        case SR_V3:
        case SR_V4:
        case SR_V5:
          portb=portb|0x3f;
          //if(load!=0)portb&=~LOAD;else portb|=LOAD;
          P0=portb;
          pben=1;
          pben=0;
          tem=ad(ch);
//          return btv+1000-((tem)*2)/5-tem/100;
			return 	(tem*4)/5;
        case TEMP_BT:
        	tem=(ad(ch)-2732-COMP_ZERO)/10;
        	if(tem>100)return 0;
        	return tem;
        case BT_V://return ad(ch)/4;
			return (ad(ch)*4)/5;
        case LOAD_I:return ad(ch)/2;                                             
        case BT_I:return ad(ch)/4;
        case SR_I0:portb=(portb|0x3f)&0xfe;ch=BT_I;break;
        case SR_I1:portb=(portb|0x3f)&0xfd;ch=BT_I;break;
        case SR_I2:portb=(portb|0x3f)&0xfb;ch=BT_I;break;
        case SR_I3:portb=(portb|0x3f)&0xf7;ch=BT_I;break;
        case SR_I4:portb=(portb|0x3f)&0xef;ch=BT_I;break;
        case SR_I5:portb=(portb|0x3f)&0xdf;ch=BT_I;break;
        default:return 0;
    }
    //if(load!=0)portb&=~LOAD;else portb|=LOAD;
    P0=portb;
    pben=1;
    pben=0;
//    EA=1;
    if(ch==BT_I)return ad(ch)/4;
	else return ad(ch)/4;
}

rad_get(unsigned char CH)
{
	please=CH;
	for(CH=0;CH<255;CH++){
		if(please==0xff)return adresult;
  		delay(20);
   	}
}

//interrupt [0x0b] void T0_int (void)
void T0_int (void) interrupt 1
{
	code unsigned char LEDTABLE[]={0x3,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x1,0x09,0xff};
    static unsigned char ms;
    static unsigned char next_state;
    unsigned int value;
    unsigned char tt[12];

    /*定时器初值设置*/
    TH0=T0_INIT/256;
    TL0=T0_INIT%256;
	if(please!=0xff){
		adresult=ad_get(please);
		please=0xff;
	}
	if(received)talk();
    /*50mS加一*/
    if((ms++)==(1000/TIMEBASE-1)){

        /*看门狗复位*/
        wdg=~wdg;
        /*1秒*/
        ms=0;
        if(DISPLAY){
                	value=ad_get(BT_I);
//                   outchar(value%10);
                   tt[0]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(
				tt[1]=LEDTABLE[value%10]-1;
                   value/=10;
//                   outchar(value%10);
				tt[2]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(value%10);
				tt[3]=LEDTABLE[value%10];
                   value=btv;
//                   outchar(value%10);
				tt[4]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(value%10+0x80);
				tt[5]=LEDTABLE[value%10]-1;
                   value/=10;
//                   outchar(value%10);
				tt[6]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(value%10);
				tt[7]=LEDTABLE[value%10];
                   value=ad_get(LOAD_I);
//                   outchar(value%10);
				tt[8]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(value%10+0x80);
				tt[9]=LEDTABLE[value%10]-1;
                   value/=10;
//                   outchar(value%10);
				tt[10]=LEDTABLE[value%10];
                   value/=10;
//                   outchar(value%10);
				tt[11]=LEDTABLE[value%10];
  				outchar(tt[0]);
  				outchar(tt[1]);
  				outchar(tt[2]);
  				outchar(tt[3]);
  				outchar(tt[4]);
  				outchar(tt[5]);
  				outchar(tt[6]);
  				outchar(tt[7]);
  				outchar(tt[8]);
  				outchar(tt[9]);
  				outchar(tt[10]);
  				outchar(tt[11]);
                 }

        /*充电流程控制*/
        btv=ad_get(BT_V);
        if(MODEL==72)value=btv-(eprom(COMP_TEMP)*110*ad_get(TEMP_BT))/100;
			else value=btv-(eprom(COMP_TEMP)*(MODEL==71?12:24)*ad_get(TEMP_BT))/100;
        if(solar_state<7){
            if(value<eprom(solar_state-1)){
                if(next_state==solar_state-1){
                    if(Delay_change==0)solar_state-=1;
                }
                else{
                    next_state=solar_state-1;
                    Delay_change=TEST?2:eprom(DELAY_CHANGE);
                }
            }
            else {
                if(value>=eprom(solar_state)){
                    if(next_state==solar_state+1){
                        if(Delay_change==0)solar_state+=1;
                    }
                    else{
                        next_state=solar_state+1;
                        Delay_change=TEST?2:eprom(DELAY_CHANGE);
                    }
                }
                else next_state=0;
            }
        }
        else{
            if(value<(solar_state==13?eprom(BOOST_BELOW):eprom(FLOAT_MIN_S))){
                if(next_state==solar_state+1){
                    if(Delay_change==0)solar_state=solar_state==13?1:solar_state+1;
                }
                else{
                    next_state=solar_state+1;
                    Delay_change=TEST?2:eprom(DELAY_CHANGE);
                }
            }
            else {
                if(value>eprom(FLOAT_MAX_S) && solar_state!=7){
                    if(next_state==solar_state-1){
                        if(Delay_change==0)solar_state-=1;
                    }
                    else{
                        next_state=solar_state-1;
                        Delay_change=TEST?2:eprom(DELAY_CHANGE);
                    }
                }
                else next_state=0;
            }
        }
//        if(value>eprom(DATA))write_eprom(DATA,value);
//        if(value<eprom(DATA+1))write_eprom(DATA+1,value);
  		if(value>btv_H)btv_H=value;
		if(value<btv_L)btv_L=value;
        chargeah+=ad_get(BT_I);
        dischargeah+=ad_get(LOAD_I);
//        write_eprom(DATA+2,chargeah/36000);
//        write_eprom(DATA+3,dischargeah/36000);

        if(!load){
            if(value>eprom(BT_MAX)){
                if(Delay_disconnect==0){
                    porta|=(ALARM_HI|UJ);
                    load=1;
					sendpacket(0xfe,load);
                }
            }
            else{
                if(value<eprom(BT_MIN)){
                    if(Delay_disconnect==0){
                        porta|=(ALARM_LO|UJ);
                        load=2;
						sendpacket(0xfe,load);
                    }
                }
                else{
                    Delay_disconnect=TEST?2:eprom(DELAY_DISCONNECT);
                }
            }
        }
        else{
            if(load==1){
                if(value<eprom(BT_MAX)-185){
                    porta&=~(ALARM_HI|UJ);
                    load=0;
					ALARMSOUND=1;
					sendpacket(0xfe,load);
                }
            }
            else{
                if(value>eprom(BT_RELOAD)){
                    porta&=~(ALARM_LO|UJ);
                    load=0;
					ALARMSOUND=1;
					sendpacket(0xfe,load);
                }
            }
        }
        Delay_change--;
        Delay_disconnect--;

        if((sec++)==59){
            sec=0;
            time++;
			sendpacket(0xfe,load);
            if((min++)==59){
                min=0;
				newhour=1;
                if((hour++)==23){
					hour=0;   
					newday=1;
				}
            }
        }
    }
    value=P0;

    P0=porta;
    paen=1;
    paen=0;
    switch(solar_state){
        case 1:
        case 13:portb=0;break;
        case 2:
        case 12:portb=0x20;break;
        case 3:
        case 11:portb=0x30;break;
        case 4:
        case 10:portb=0x38;break;
        case 5:
        case 9:portb=0x3c;break;
        case 6:
        case 8:portb=0x3e;break;
        case 7:portb=0x3f;break;
        default:solar_state=1;
    }
    if(load!=0){
		if(ALARMSOUND)portb|=FMQ;else portb&=~FMQ;
	}
    if(!IsSolar){
        P0=portb;
        pben=1;
        pben=0;
    }
    P0=value;
}

void talk(void)
{
	unsigned char value;
    	if(buf[1]==1){			//read variable
    		switch(buf[2]){
     			case 0:         //btv
       				sendpacket(0,btv);
       		        break;
       	        case 1:
       	        	sendpacket(1,ad_get(LOAD_V));
       	            break;
       	        case 2:
       	        	sendpacket(2,0);//ad_get(SR_V0));
       	            break;
       	        case 3:
       	        	sendpacket(3,0);//ad_get(SR_V1));
       	            break;
       	        case 4:
       	        	sendpacket(4,0);//ad_get(SR_V2));
       	            break;
       	        case 5:
       	        	sendpacket(5,0);//ad_get(SR_V3));
       	            break;
       	        case 6:
       	        	sendpacket(6,0);//ad_get(SR_V4));
       	            break;
       	        case 7:
       	        	sendpacket(7,0);//ad_get(SR_V5));
       	            break;
       	        case 8:
       	        	sendpacket(8,ad_get(TEMP_BT)+20);
       	            break;
       	        case 9:
       	        	sendpacket(9,(int)solar_state);
       	            break;
       	        case 10:
       	        	sendpacket(10,(int)load);
       	            break;
       	        case 14:    
       	        	sendpacket(14,ad_get(LOAD_I));
       	            break;
       	        case 15:
       	        	sendpacket(15,ad_get(BT_I));
       	            break;
       	        case 0x10:
       	        	sendpacket(0x10,ad_get(SR_I0));
       	            break;
       	        case 0x11:
       	        	sendpacket(0x11,ad_get(SR_I1));
       	            break;
       	        case 0x12:
       	        	sendpacket(0x12,ad_get(SR_I2));
       	            break;
       	        case 0x13:
       	        	sendpacket(0x13,ad_get(SR_I3));
       	            break;
       	        case 0x14:
       	        	sendpacket(0x14,ad_get(SR_I4));
       	            break;
       	        case 0x15:
       	        	sendpacket(0x15,ad_get(SR_I5));
       	            break;
       	        case 0x20:
       	           	sendpacket(0x20,eprom(BOOST));	//BOOST
       	            break;
       	        case 0x21:
       	        	sendpacket(0x21,eprom(TAPER));	//taper
       	            break;
       	        case 0x22:
       	        	sendpacket(0x22,eprom(FLOAT_MAX_S));
       	            break;
       	        case 0x23:
       	        	sendpacket(0x23,eprom(FLOAT_MIN_S));
       	            break;
       	        case 0x24:
       	        	sendpacket(0x24,eprom(BOOST_BELOW));
       	            break;
       	        case 0x25:
       	        	sendpacket(0x25,eprom(COMP_TEMP));
       	            break;
       	        case 0x26:
       	        	sendpacket(0x26,eprom(DELAY_CHANGE));
       	            break;
       	        case 0x27:
       	        	sendpacket(0x27,eprom(BT_MIN));
       	            break;
       	        case 0x28:
       	        	sendpacket(0x28,eprom(BT_RELOAD));
       	            break;
       	        case 0x29:
       	        	sendpacket(0x29,eprom(BT_MAX));
       	            break;
       	        case 0x2a:
       	        	sendpacket(0x2a,eprom(DELAY_DISCONNECT));
       	            break;
       	        case 0x2b:
       	        	sendpacket(0x2b,eprom(BT_V_MIN));
       	            break;
       	        case 0x2c:
       	        	sendpacket(0x2c,eprom(BT_V_MAX));
       	            break;
       	        case 0x2d:
       	        	sendpacket(0x2d,eprom(CHARGE_AH));
       	            break;
       	        case 0x2e:
       	        	sendpacket(0x2e,eprom(DISCHARGE_AH));
       	            break;
       	        case DATA_INDEX:
       	        	for(value=DATA;value<DATA+32*4;value++){
       	        	    sendpacket(value,eprom(value));
       	                wdg=~wdg;
       	            }
       	            break;
       	        default:sendpacket(0xff,0xff); 
       		}
       	}
       	else{                   //write writeable variable
       		switch(buf[2]){
       	        case 0x20:
       	        	write_eprom(BOOST,buf[3]*256+buf[4]);
       	        	sendpacket(0x20,eprom(BOOST));	//BOOST
       	            arg();
       	            break;
       	        case 0x21:
       	        	write_eprom(TAPER,buf[3]*256+buf[4]);
       	        	sendpacket(0x21,eprom(TAPER));	//taper
       	            arg();
       	            break;
       	        case 0x22:
       	        	write_eprom(FLOAT_MAX_S,buf[3]*256+buf[4]);
       	        	sendpacket(0x22,eprom(FLOAT_MAX_S));
       	            break;
       	        case 0x23:
       	        	write_eprom(FLOAT_MIN_S,buf[3]*256+buf[4]);
       	        	sendpacket(0x23,eprom(FLOAT_MIN_S));
       	            break;
       	        case 0x24:
       	        	write_eprom(BOOST_BELOW,buf[3]*256+buf[4]);
       	        	sendpacket(0x24,eprom(BOOST_BELOW));
       	            break;
       	        case 0x25:
       	        	write_eprom(COMP_TEMP,buf[3]*256+buf[4]);
       	        	sendpacket(0x25,eprom(COMP_TEMP));
       	            break;
       	        case 0x26:
       	        	write_eprom(DELAY_CHANGE,buf[3]*256+buf[4]);
       	        	sendpacket(0x26,eprom(DELAY_CHANGE));
       	            break;
       	        case 0x27:
       	        	write_eprom(BT_MIN,buf[3]*256+buf[4]);
       	        	sendpacket(0x27,eprom(BT_MIN));
       	            break;
       	        case 0x28:
       	        	write_eprom(BT_RELOAD,buf[3]*256+buf[4]);
       	        	sendpacket(0x28,eprom(BT_RELOAD));
       	            break;
       	        case 0x29:
       	        	write_eprom(BT_MAX,buf[3]*256+buf[4]);
       	        	sendpacket(0x29,eprom(BT_MAX));
       	            break;
       	        case 0x2a:
       	        	write_eprom(DELAY_DISCONNECT,buf[3]*256+buf[4]);
       	        	sendpacket(0x2a,eprom(DELAY_DISCONNECT));
       	            break;
       	     	default:
       	        	write_eprom(buf[2],buf[3]*256+buf[4]);
       	            sendpacket(buf[2],eprom(buf[2]));
       	            break;
       		}
       	}
		received=0;
    }


/*DELAY()
{
    clk=1;
    clk=0;
} */
  
#define DELAY() clk=1;clk=0;
/*----------------------*/
/*  EPROM设置及擦除程序 */
/*  command=00H,EWDS  */
/*  command=01H,EWEN  */
/*----------------------*/
/*monitor*/ set_eprom(unsigned char command)
{
    unsigned char k;
    EA=0;
    cs=clk=di=0;
    Do=1;
    cs=1;
    while(!Do);
    di=1;
    DELAY();
    if(command==0){
            di=0;
            DELAY();
            DELAY();
        }
        else{
            di=0;
            DELAY();
            DELAY();
            command=0xc0;
        }
    for(k=0;k<8;k++){
        di=command>>7;
        DELAY();
        command<<=1;
    }
    cs=0;
    EA=1;
}

/*--------------------------*/
/*  EPROM写入程序           */
/*  address<256,写单个数据  */
/*--------------------------*/
/*monitor*/ write_eprom(unsigned char address,unsigned int Data)
{
    unsigned char k;
    set_eprom(EWEN);
    EA=0;
    cs=clk=di=0;
    Do=1;
    cs=1;
    while(!Do);
    di=1;
    DELAY();
    di=0;
    DELAY();
            di=1;
    DELAY();
    for(k=0;k<8;k++){
        di=address>>7;
        DELAY();
        address<<=1;
    }
    for(k=0;k<16;k++){
        di=Data>>15;
        DELAY();
        Data<<=1;
    }
    cs=0;
    EA=1;
    set_eprom(EWDS);
}

/*------------------*/
/*  EPROM读出程序   */
/*------------------*/
/*monitor*/ unsigned int eprom(unsigned char address)
{
    unsigned char k,i;
    unsigned int result;
    EA=0;
    for(i=0;i<10;i++){
        cs=clk=di=0;
        Do=1;
        cs=1;
        while(!Do);
        cs=0;
        cs=1;
        di=1;
        DELAY();
        DELAY();
        di=0;
        DELAY();
        for(k=0;k<8;k++){
            di=address>>7;
            DELAY();
            address<<=1;
        }
        if(Do==0)goto GOOD;
    }
    return 0;
GOOD:
    for(k=0;k<16;k++){
        result<<=1;
        DELAY();
        result=result|Do;
    }
    cs=0;
    EA=1;
    return result;
}

/*----------------------*/
/*  液晶显示驱动程序    */
/*  型号:   MDLS-16265B */
/*----------------------*/
/*----------------------*/
/*  液晶显示器判忙程序  */
/*----------------------*/

/*monitor*/ unsigned char busy()
{
    unsigned char ac;
    EA=0;
    P0=0xff;
    RS=0;
    rw=1;
    e=1;
    ac=P0;
    e=0;
    EA=1;
    return ac>>7;
}

/*monitor*/ lcd_opr(unsigned char ch)
{
    while(busy());
    EA=0;
    RS=rw=0;
    P0=ch;
    e=1;
    e=0;
    EA=1;
}

/*--------------------------*/
/*  液晶显示器初始化程序    */
/*--------------------------*/
lcd_init(void)
{
    /*  工作方式设置,8位数据接口,两行显示,5X7点阵   */
    lcd_opr(0x38);
    /*  输入方式选择,AC自动加1,画面不动 */
    lcd_opr(6);
    /*  显示开关控制,开显示,关光标,禁止闪烁 */
    lcd_opr(12);
    lcd_opr(1);
}


/*------------------------------*/
/*  单字符显示,供printf()调用   */
/*------------------------------*/
/*monitor*/ aputchar(unsigned char c)  reentrant
{
//    if(OutDevice==LCD){
        while(busy());
//        EA=0;
        RS=rw=0;
        switch(c){
            case '\n':P0=0xc0;break;
            case '\b':P0=2;break;
            case '\a':P0=1;break;
            default:RS=1;P0=c;
        }
        e=1;
        e=0;
  //      EA=1;
//    }
//    else{
//       while(usy){
//            OutDevice=LCD;
//            aprintf("\aPRINTER NOT\n READY...");
//            if(key()==CANCEL)return 1;
//            OutDevice=PRINTER;
//        }
//        EA=0;
//       P0=c;
//       stb=0;
//       stb=1;
//       EA=1;
//       if(c=='\n')aputchar('\r');
//    }
    return 0;
}

/*------------------*/
/*  格式化输出命令  */
/*------------------*/
/*monitor*/ aprintf (char *format,...) reentrant
{
    unsigned char format_flag,*ptr;                               /*格式标志*/
    unsigned int div_val, u_val, base;
    va_list ap;
    va_start(ap,format);
//    EA=0;
    for (;;){
        while ((format_flag = *format++) != '%'){      /* Until '%' or '\0' */
            if (!format_flag){
//                EA=1;
                return (1);
            }
            if(aputchar (format_flag))return 1;
        }
        switch (format_flag = *format++){
            case 'c':
                format_flag = va_arg(ap,char);
            default:
                if(aputchar (format_flag))return 1;
            continue;
            case 's':
                ptr = va_arg(ap,char *);
            while (format_flag = *ptr++){
                if(aputchar (format_flag))return 1;
            }
            continue;
            case 'e':
                base=10;
                div_val=100;
                u_val=va_arg(ap,unsigned char);
                while(div_val>1&& div_val >u_val){
                    div_val/=10;
                }
                do{
                    aputchar(u_val/div_val+48);
                    u_val%=div_val;
                    div_val/=base;
                }while(div_val);
                continue;
            case 'o':
            case 'd':
                base = 10;
                div_val = 10000;
            u_val = va_arg(ap,int);
                while (div_val > 1 && div_val > u_val){
                    div_val /= 10;
                }
                if(format_flag=='o' && div_val==1)aputchar('0');
            do{
                if(format_flag=='o' && div_val==1)aputchar('.');
                aputchar (u_val / div_val+48);
                u_val %= div_val;
                div_val /= base;
            }
            while (div_val);
        }
    }
}

delay(unsigned int time)
{
    unsigned char j;
    for(;time>0;time--)for(j=0;j<DELAY_SCALE;j++)wdg=~wdg;
}

unsigned char key()
{
//#pragma memory=code
//    code unsigned char key_tab[16]={112,115,114,113,
//                        179,178,177,211,
//                        210,209,227,176,
//                        208,226,225,224};
//    code unsigned char key_tab[16]={227,211,179,115,//115,114,113,112,
//    								226,210,178,114,//179,178,177,176,
//       					            225,209,177,113,//211,210,209,208,
//       		            			224,208,176,112};//227,226,225,224};
//    code unsigned char key_tab[16]={112,176,208,224,
//                        113,177,209,225,
//                        114,178,210,226,
//                        115,179,211,227};
    code unsigned char key_tab[16]={224,208,176,112,
                        225,209,177,113,
                        226,210,178,114,
                        227,211,179,115};

//#pragma memory=default
    unsigned char j,k;
    wdg=~wdg;
    for(j=0;j<4;j++){
        P1=~(1<<j);
        k=P1|0xf;
        if(k!=0xff){
            delay(20);
            if(k==P1|0xf)k=(k&0xf0)+j;
            break;
        }
    }
    P1=0xff;
    for(j=0;j<16;j++){
        if(key_tab[j]==k){
            k=j;
            time=0;
			if(portb>=FMQ)ALARMSOUND=0;
            return k;
        }
    }
    if(display_pb==0){
    	time=0;
		if(portb>=FMQ)ALARMSOUND=0;
    	return 1;
    }
    return NO;
}

unsigned char wait_key()
{
    unsigned char k;
    while(key()!=NO&&time<WAITTIME);
    while((k=key())==NO && time<WAITTIME);
	return k;
}	
	
unsigned int ascanf(char *format,unsigned char lenth)
{
    unsigned char k,i;
    i=0;
    for(;;){
        k=wait_key();
            if(k== OK)return 1;
            if(k== CANCEL && i>0){
                i--;
                format[i]=0;
            }
            if(k<10){
                if(i<(lenth)){
                    format[i]=k+48;
                    i++;
                    for(k=i;k<lenth;k++)format[k]=0;
                }
            }
        aprintf("\n%s            ",format);
    }
}

unsigned int setup(char *message,int a,int max,int min)
{
    unsigned char k;
loop:
        lcd_opr(1);
        aprintf(message,a);
    if((k=wait_key())==NO)return 0xffff;
            if(k== OK)return a;
            if(k== INC){
                if(a<max)a++;
                aprintf(message,a);
                while(key()==INC){
                    delay(500);
                    while(key()==INC){
                        if(a<max)a++;
                        aprintf(message,a);
                        delay(200);
                  }
                }
            }
            if(k== DEC){
                if(a>min)a--;
                aprintf(message,a);
                while(key()==DEC){
                    delay(500);
                    while(key()==DEC){
                        if(a>min)a--;
                        aprintf(message,a);
                        delay(200);
                   }
                }
        }
    goto loop;
}

char password()
{
    unsigned char pass[4],i;
    for(i=0;i<4;i++)pass[i]=0;
    aprintf("\aEnter Password:");
    ascanf(pass,3);
    for(i=0;i<3;i++){
        if(pass[i]!=eprom(PASSWORD+i)){
            aprintf("\bPassword Error!");
            wait_key();
            aprintf("\a");
            return 0;
        }
    }
    return 1;
}

control_set()
{
    unsigned char k;
//    #pragma memory=code
    code unsigned char *prompt[]={"\bMAXIMUM BOOST\nVOLTAGE %oV ",
                    "\bBOOST TAPER AT\nVOLTAGE %oV ",
                    "\bFLOAT MAXIMUM\nVOLTAGE %oV ",
                    "\bFLOAT MINIMUM\nVOLTAGE %oV ",
                    "\bRETURN TO BOOST\nMODE BELOW %oV ",
                    "\bTEMPERATURE COMP\nAT -%dmV/逤/CELL ",
                    "\bSTATE CHANGE\nDELAY %d S  "
                };
    code unsigned int MAX[]={BOOST_MAX,TAPER_MAX,FLOAT_MAX,FLOAT_MAX,BOOST_BELOW_MAX,COMP_TEMP_MAX,DELAY_CHANGE_MAX};
    code unsigned int MIN[]={BOOST_MIN,TAPER_MIN,FLOAT_MIN,FLOAT_MIN,BOOST_BELOW_MIN,COMP_TEMP_MIN,DELAY_CHANGE_MIN};
//    #pragma memory=default
    unsigned int temp[7];
    if(!password())return 0;
    for(k=0;k<7;k++)
        if((temp[k]=setup(prompt[k],eprom(BOOST+k),MAX[k],MIN[k]))==0xffff)return 0;
    aprintf("\aSAVE CHANGE?\nINC=YES OTHER=NO");
    if(wait_key()==INC){
        for(k=0;k<7;k++)write_eprom(BOOST+k,temp[k]);
        arg();
    }
    lcd_opr(1);
}

load_setup()
{
    unsigned char k;
//    #pragma memory=code
    code unsigned char *prompt[]={
        "\bLOW BATTERY AL-\nARM ON AT %oV ",
        "\bRECONNECT LOAD\nABOVE %oV ",
        "\bHIGH BATTERY AL-\nARM ON AT %oV ",
        "\bDELAY BEFORE\nDISCONNECT %d S  "
    };
    code unsigned int MAX[]={BT_MIN_MAX,BT_RELOAD_MAX,BT_MAX_MAX,DELAY_DIS_MAX};
    code unsigned int MIN[]={BT_MIN_MIN,BT_RELOAD_MIN,BT_MAX_MIN,DELAY_DIS_MIN};
//    #pragma memory=default
    unsigned int temp[4];
    if(!password())return 0;
    for(k=0;k<4;k++)
        if((temp[k]=setup(prompt[k],eprom(BT_MIN+k),MAX[k],MIN[k]))==0xffff)return 0;
    aprintf("\aSAVE CHANGE?\nINC=YES OTHER=NO");
    if(wait_key()==INC){
        set_eprom(EWEN);
        for(k=0;k<4;k++)
            write_eprom(BT_MIN+k,temp[k]);
        set_eprom(EWDS);
        arg();
    }
    lcd_opr(1);
}


remote_set()
{
//    #pragma memory=code
//    code unsigned char *prompt[]={"OFF","ON"};
//    #pragma memory=default
    unsigned char /*Phone[1],*/Local/*,Up_hour,Up_min*/;
    bit Upload=1;
//    unsigned char k;
    if(!password())return 0;
/*    Upload=eprom(AUTO_UPLOAD);*/
    Local=eprom(LOCAL);
/*    for(k=0;k<12;k++)Phone[k]=eprom(PHONE+k);
    Phone[12]=0;*/
/*    while(1){
        k=Upload;
        aprintf("\aAuto Upload:\n[%s] ",prompt[k]);
        k=wait_key();
        if(k==NO)return 0;
        if(k==DEC || k==INC)Upload=~Upload;
        if(k==OK)break;
    }*/
/*    if(Upload){*/
        if((Local=setup("\bSit Number:\n%d  ",eprom(LOCAL),255,0))==0xffff)return 0;
/*        if((Up_hour=setup("\bUpload Hour:\n%d ",eprom(UP_HOUR),23,0))==0xffff)return 0;
        if((Up_min =setup("\bUpload Minute:\n%d ",eprom(UP_MIN),59,0))==0xffff)return 0;
        aprintf("\aRemote Phone:\n%s",Phone);
        if(!ascanf(Phone,12))return 0;
    }*/
    aprintf("\aSAVE CHANGE?\nINC=YES OTHER=NO");
    if(wait_key()==INC){
        set_eprom(EWEN);
/*        write_eprom(AUTO_UPLOAD,Upload);*/
        write_eprom(LOCAL,Local);
/*        for(k=0;k<12;k++)write_eprom(PHONE+k,Phone[k]);
        write_eprom(UP_HOUR,Up_hour);
        write_eprom(UP_MIN,Up_min);*/
        set_eprom(EWDS);
        arg();
    }
    lcd_opr(1);
}


char state(unsigned char a)
{
    return ((portb>>a)&1)?'0':'1';
}

solar()
{
//    #pragma memory=code
    code char *prompt[]={"OFF","ON"};
//    #pragma memory=default
    unsigned char bank=0;
    bit change=1;
    lcd_opr(1);
    IsSolar=1;
    while(1){
    	if(change){
//    	 	aprintf("\bBANK%e %oVoc   ",bank+1,rad_get(SR_V0+bank));
			aprintf("\bBANK%e",bank+1);
      		change=0;
       	}
        aprintf("\n[%s]     %oA  ",prompt[state(bank)-'0'],rad_get(SR_I0+bank));
        delay(200);
        switch(key()){
            case INC:bank=(bank+1)%6;change=1;break;
            case DEC:if(bank==0)bank=5;else bank--;change=1;break;
            case SOLAR:change=1;
            case NO:break;
            default:IsSolar=0;lcd_opr(1);return 0;
        }
        if(time>WAITTIME){
                lcd_opr(1);
                IsSolar=0;
                return 0;
        }
    }
}

view()
{
    unsigned char k,address;
    unsigned char i;
    i=k=0;
    for(;;){
		wdg=~wdg;
        address=i*4+DATA;
        aprintf("\aD%e H:%o L:%oV",i,eprom(address),eprom(address+1));
        aprintf("\nC:%d D:%d",eprom(address+2),eprom(address+3));
        switch(wait_key()){
            case INC:i=(i+1)%32;break;
            case DEC:if(i==0)i=31;else i--;break;
			case F1:{
				aprintf("\aClear Histroy!\nAre you sure?");
				if(wait_key()==OK){
					for(i=0;i<128;i++)write_eprom(DATA+i,0);
				}
				i=0;
			}
			break;
            default:lcd_opr(1);return 0;
        }
     }
}

testself()
{
//    #pragma memory=code
    code char *prompt[]={
        "Test Mode",
        "Meter Adjust"
    };
//    #pragma memory=default
    
    unsigned char k=0,l;
    unsigned char i=0;
    while(k!=OK){
        aprintf("\a%s ",prompt[i]);
        k=wait_key();
        if(k==0xff || k==CANCEL)return 0;
        if(k==INC || k==DEC)if(i==0)i=1;else i=0;
    }
    if(i==0){
        TEST=1;
        while(time<60 && key()!=CANCEL){
            aprintf("\bTEST A:%e",solar_state);
            aprintf(" %c",state(0));
            aprintf("%c",state(1));
            aprintf("%c" ,state(2));
            aprintf("%c",state(3));
            aprintf("%c",state(4));
            aprintf("%c  ",state(5));
            aprintf("\nB: %oV T:%d ",btv,rad_get(TEMP_BT)+20);
            delay(500);
        }
        Delay_change=eprom(DELAY_CHANGE);
        Delay_disconnect=eprom(DELAY_DISCONNECT);
        TEST=0;
    }
    else{
        for(i=0;i<16;i++){
            k=NO;
            l=eprom(CLIP+i);
            while(k!=OK){
                aprintf("\aIN %e\n%e  ",i,l);
                k=wait_key();
                if(k==NO || k==CANCEL)return 0;
                if(k==INC)l++;
                if(k==DEC)l--;
            }
            write_eprom(CLIP+i,l);
            lcd_opr(1);
        }
    }
/*    else{
        set_eprom(EWEN);
        for(i=0;i<256;i++){
            tmp=eprom(i);
            write_eprom(i,0);
            if(eprom(i)!=0)goto ERR;
            write_eprom(i,0xff);
            if(eprom(i)!=0xff)goto ERR;
            write_eprom(i,tmp);
            if(eprom(i)!=tmp)goto ERR;
        }
        aprintf("\bEEPROM TEST OK!");
        while(wait_key()!=OK);
        goto next;
ERR:
        aprintf("\bEEPROM TEST \nFAILED!");
        while(wait_key()!=OK);
        return 0;
next:
        for(i=0;i<6;i++){
            EA=0;
            portb=1<<i;
            P0=portb;
            pben=1;
            pben=0;
            while(key()!=CANCEL)aprintf("\bTEST B:  BANK%d\n  %oA  ",i,ad_get(BT_I));
        }
        EA=1;
    }*/
    lcd_opr(1);
}


save()
{
    char i;
    set_eprom(EWEN);
    for(i=30;i>=0;i--){
        write_eprom(DATA+(i+1)*4,eprom(DATA+i*4));
        write_eprom(DATA+(i+1)*4+1,eprom(DATA+i*4+1));
        write_eprom(DATA+(i+1)*4+2,eprom(DATA+i*4+2));
        write_eprom(DATA+(i+1)*4+3,eprom(DATA+i*4+3));
    }
    write_eprom(DATA,btv_H);
    write_eprom(DATA+1,btv_L);
    write_eprom(DATA+2,chargeah/36000);
    write_eprom(DATA+3,dischargeah/36000);
    set_eprom(EWDS);
    chargeah=dischargeah=0;
	btv_H=0;
	btv_L=10000;
    sendstr("AT&FE0V0X0S0=1\r");
}


void outchar(char cc)
{
	unsigned char i;
	for(i=0;i<8;i++){
  		cc=cc>>1;
 		if(CY)dta=0;
  		else dta=1;
  		slk=1;
  		slk=0;
		slk=0;
		slk=0;
		slk=0;
  		slk=1;
  	}
}

main()
{
//    unsigned int tt;
    init();
    chargeah=dischargeah=0;
	btv_H=0;
	btv_L=10000;
//    set_eprom(EWEN);
//    write_eprom(DATA,0);
//    write_eprom(DATA+1,10000);
//    set_eprom(EWDS);
    TEST=0;
    sendstr("AT&F\r");
	delay(100);
	sendstr("ATE0V0X0\r");
	delay(100);
	sendstr("ATS0=2S10=10\r");
//	sendstr("AT&FE0V0X0S0=2S10=10\r");
    goto start;
    while(1){
        if(key()!=NO){
start:
            lcd_init();
            porta|=LAMP;
            P0=porta;
            paen=1;
            paen=0;
            DISPLAY=1;
            while(time<WAITTIME){
                aprintf("\bBATTERY VOLTAGE\n          %oV ",btv);
                switch(key()){
                    case BT_V_KEY:{
                        while(key()==BT_V_KEY);
                        aprintf("\a");
                        while(key()==NO&&time<WAITTIME){
                            aprintf("\bBATTERY VOLTAGE\n          %oV ",btv);
                            delay(500);
                        }
                        lcd_opr(1);
                        break;
                    }
                    case BT_I_KEY:{
                        while(key()==BT_I_KEY);
                        aprintf("\a");
                        while(key()==NO&&time<WAITTIME){
                            aprintf("\bCHARGE CURRENT\n          %oA ",rad_get(BT_I));
                            delay(500);
                        }
                        lcd_opr(1);
                        break;
                    }
                    case LOAD_V_KEY:{
                        while(key()==LOAD_V_KEY);
                        aprintf("\a");
                        while(key()==NO&&time<WAITTIME){
                            aprintf("\bLOAD VOLTAGE\n          %oV ",rad_get(LOAD_V));
                            delay(500);
                        }
                        lcd_opr(1);
                        break;
                    }
                    case LOAD_I_KEY:{
                        while(key()==LOAD_I_KEY);
                        aprintf("\a");
                        while(key()==NO&&time<WAITTIME){
                            aprintf("\bLOAD CURRENT\n          %oA ",rad_get(LOAD_I));
                            delay(500);
                        }
                        lcd_opr(1);
                        break;
                    }
                    case SOLAR:solar();break;
                    case CONTROL_SET:control_set();break;
                    case TESTSELF:testself();break;
                    case REMOTE_SET:remote_set();break;
                    case LOAD_SET:load_setup();break;
                    case VIEW:view();break;
 //                   case F1:ToPrinter();aprintf("\a");break;
 //                   case F2:graph();aprintf("\a");
                }
//	        if(recviced()){
//                tt=getb();
//                if(/*tt==local||*/tt==0xff){
//                	send('K');
//                    aprintf("\aRemote Connect!");
//	                talk();
//	            }
//	        }
            }
            lcd_opr(8);
            porta&=~LAMP;
            P0=porta;
            DISPLAY=0;
            paen=1;
            paen=0;
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
            outchar(0xff);
        }
/*        if(up_load && hour==eprom(UP_HOUR) && min>=eprom(UP_MIN)){
            if(dial()==1){
                talk();
                up_load=0;
            }
        }*/
//        if(recviced()){
//            tt=getb();
//            if(/*tt==local||*/tt==0xff){
//            	send('K');
//            	port|=LAMP;
//            	P0=port;
//            	port_out_en=1;
//            	port_out_en=0;
//            	lcd_init();
//                aprintf("\bRemote Connect!");
//                talk();
//                port&=~LAMP;
//            	P0=port;
//            	port_out_en=1;
//            	port_out_en=0;
//            	lcd_opr(8);
//            }
//        }
		if(newhour){
    		write_eprom(DATA,btv_H);
    		write_eprom(DATA+1,btv_L);
    		write_eprom(DATA+2,chargeah/36000);
    		write_eprom(DATA+3,dischargeah/36000);
			newhour=0;
		}
        if(newday){
            save();
            newday=0;
        }

    }
}

