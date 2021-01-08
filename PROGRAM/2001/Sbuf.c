/*==================================*/
/*  serial port                     */
/*==================================*/

#include <AT89x52.h>

extern bit received;
extern idata unsigned char buf[6];
extern unsigned char point;
extern bit TXEN;

/*==============================================================*/
/*  command string format:                                      */
/*  begin code,operation code,address code, data code, checksum */
/*  0xaa,      1(read,2-write),1,           0x0,0x0(int),0x0    */
/*==============================================================*/ 
void serial() interrupt 4 using 1
{
    static bit rec;
	if(RI){
		RI=0;
		if(SBUF==0xaa){
			point=0;
			rec=0;
		}
		if(!rec){
			buf[point]=SBUF;
			point++;
			if(point>=6){
				if((unsigned char)(buf[0]+buf[1]+buf[2]+buf[3]+buf[4])==buf[5]){
					received=1;
   					point=0;
					rec=1;
				}
				else{
					point=0;
				}
			}
	    }
	}
//    else{
	if(TI){
    	TI=0;
    	TXEN=1;
    }
}

send(unsigned char cc)
{
	while(!TXEN);
	SBUF=cc;
	TXEN=0;
}
/*  单片机串口通讯基本程序  */
/*  无硬件握手信号          */
/*  中断接收、查询发送      */
/****************************/


/*------------------*/
/*  单字符发送      */
/*------------------*/
//send(unsigned char Data)
//{
//	while(TI);
//    SBUF=Data;
//    delay(10);
//}

/*------------------------------*/
/*  发送一个字符串，直到字符为0 */
/*------------------------------*/
sendstr(unsigned char *Data)
{
    while(*Data!=0){
    	send(*Data);
        Data++;
    }
}

/*----------------------------------*/
/* Packet send                      */
/*  format:                         */
/*  0xaa,address code,data,checksum */
/*----------------------------------*/
sendpacket(unsigned char Code,unsigned int Data)
{
    unsigned char checksum;
    union{
        unsigned int word;
        unsigned char signle[2];
    }t;
    t.word=Data;
    send(0xaa);
    checksum=0xaa;
    send(Code);
    checksum+=Code;
    send(t.signle[1]);
    checksum+=t.signle[1];
    send(t.signle[0]);
    checksum+=t.signle[0];
    send(checksum);
}
    

/*--------------------------*/
/*  调制解调器初始化        */
/*  设置为无回显、数字回应  */
/*--------------------------*/

/*--------------------------*/
/*  串行通讯中断程序        */
/*  接收中断将字符          */
/*  发送中断清除发送标志    */
/*--------------------------*/
/*interrupt [0x23] void SCON_int (void)*/
//void SCON_int(void) interrupt 4
//{
//    if(RI){
//        buf[bufin]=SBUF;
//        bufin>=(sizeof(buf)-1)?bufin=0:bufin++;
//        RI=0;
//    }
//    TI=0;
//}

//unsigned char recviced()
//{
  //  idata unsigned char i;
    //if(bufin!=bufout){
      //  return 1;
        //for(i=bufout;i!=bufin;){
          //  if(buf[i]==13)return 1;
            //i>=(sizeof(buf)-1)?i=0:i++;
//        }
  //  }
//    return 0;
//}

//unsigned int getb()
//{
//    idata unsigned char i;
//    if(bufout!=bufin){
//        i=buf[bufout];
//        bufout>=(sizeof(buf)-1)?bufout=0:bufout++;
//        return i;
//    }
//    else return 0xffff;
//}

/*unsigned char getcode()
{
    unsigned int b1=0xffff,b2=0xffff;
    while((b1=getb())!=0xffff){
	if(b1==13)return b2;
	if((b2=getb())==13)return b1;
    }
} */

/*unsigned char dial()
{
    unsigned char i,phone[13];
    for(i=0;i<13;i++)phone[i]=eprom(PHONE+i);
    bufin=bufout=0;
    sendstr("ATDT");
    sendstr(phone);
    send('\r');
    for(i=0;i<120;i++){
	delay(500);
	if(recviced())break;
    }
    if(getcode()==CONNECT)return 1;
    else return 0;
} */

/*sendw(unsigned int a)
{
    idata union{
    unsigned int a;
    unsigned char c[2];
    }d;
    d.a=a;
    send(d.c[1]);
    send(d.c[0]);
}

unsigned int getw(){
    idata union{
    unsigned int a;
    unsigned char b[2];
    }c;
    idata unsigned int i;
    while((i=getb())==0xffff);
    c.b[0]=i;
    while((i=getb())==0xffff);
    c.b[1]=i;
    return c.a;
}

  talk()
{
    idata unsigned char flag=1;
    idata unsigned int CheckSum;
    idata unsigned int a,b;
//    local=eprom(LOCAL);
    while(flag){
	switch(getb()){
		case NowData:{
                CheckSum=0;
                b=btv;
                CheckSum+=b;
				sendw(b);
                for(a=0;a<6;a++){
                    b=0;
                    CheckSum+=b;
                    sendw(b);
                }
                b=ad_get(LOAD_V);
                CheckSum+=b;
                sendw(b);
                b=ad_get(TEMP_BT);
                CheckSum+=b;
                sendw(b);
                sendw(0); /*in_temp,nouse*/
/*                b=ad_get(LOAD_I);
                CheckSum+=b;
                sendw(b);
                b=ad_get(BT_I);
                CheckSum+=b;
                sendw(b);
                for(a=0;a<6;a++){
                    b=ad_get(SR_I0+a);
                    delay(1000);
                    CheckSum+=b;
                    sendw(b);
                }
                b=eprom(DATA);
                CheckSum+=b;
                sendw(b);
                b=eprom(DATA+1);
                CheckSum+=b;
                sendw(b);
                b=chargeah%65536;
                CheckSum+=b;
                sendw(b);
                b=chargeah/65536;
                CheckSum+=b;
                sendw(b);
                b=dischargeah%65536;
                CheckSum+=b;
                sendw(b);
                b=dischargeah/65536;
                CheckSum+=b;
                sendw(b);
                b=hour*256+min;
                CheckSum+=b;
                sendw(b);
                b=load*256+solar_state;
                CheckSum+=b;
                sendw(b);
                sendw(CheckSum);
			break;
		}
		case HistoryData:{
            CheckSum=0;
			for(a=DATA_INDEX;a<DATA+4*32;a++){
				b=eprom(a);
                CheckSum+=b;
				sendw(b);
			}
            sendw(CheckSum);
			break;
		}
		case SetData:{
            CheckSum=0;
			for(a=7;a<37;a++){
				b=eprom(a);
                CheckSum+=b;
				sendw(b);
			}
            sendw(CheckSum);
			break;
		}
		case SendSetData:{
			set_eprom(EWEN);
			for(a=0;a<30;){
				send(a);
				b=getw();
				if(getw()==b){
					write_eprom(a,b);
					a++;
				}
			}
			set_eprom(EWDS);
			send(30);
			break;
		}
		case NO_CARRIER:flag=0;
	}
    }
}*/
