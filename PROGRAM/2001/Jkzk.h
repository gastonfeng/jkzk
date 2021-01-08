/*--------------------------------------*/
/*  JRT-xx-xx系列太阳能智能充电控制器   */
/*  主程序宏定义文件                    */
/*  文件名: JRT.H                       */
/*  CPU型号:  89C52                     */
/*  程序设计: 冯建涛                    */
/*  开始日期: 1997年1月1日              */
/*  1998年5月增加打印功能               */
/*	2001年5月15日重画线路板，增加蜂鸣器	*/
/*--------------------------------------*/

/*------------------*/
/*  控制器型号定义  */
/*------------------*/
#define MODEL 70

/*----------------------*/
/*      键名定义        */
/*----------------------*/

#define VIEW      	3	//0 112      /*      键空值          */
#define SOLAR    	0	//1 115      /*      蓄电池电压      */
#define BT_V_KEY        1	//2 114      /*      蓄电池充电电流  */
#define BT_I_KEY        4	//4 179      /*      负载电压        */
#define LOAD_V_KEY      2	//3 113      /*      太阳电池        */
#define LOAD_I_KEY      5	//5 178      /*      控制设置        */
#define CONTROL_SET     8	//7 211      /*      负载电流        */
#define REMOTE_SET      6	//6 177      /*      自检            */
#define LOAD_SET        10	//9 209      /*      通讯设置        */
#define TESTSELF        9	//8 210      /*      负载设置        */
#define OK              12	//10 227     /*      历史数据察看    */
#define INC             7	//11 176     /*      加              */
#define DEC             11	//12 208     /*      减              */
#define CANCEL          13	//13 226
#define F1              14  //14 225
#define F2              15  //15 224
#define NO              16
/*------------------------------*/
/*      设置变量范围定义        */
/*------------------------------*/
#if MODEL==70
#define BOOST_MAX       700//680
#define BOOST_MIN       400//540
#define TAPER_MAX       700//680
#define TAPER_MIN       400//520
#define BANK_MAX        6
#define BANK_MIN        1
#define FLOAT_MAX       700//600
#define FLOAT_MIN       400//480
#define BOOST_BELOW_MAX 700//600
#define BOOST_BELOW_MIN 400//480
#define COMP_TEMP_MAX   20
#define COMP_TEMP_MIN   0
#define DELAY_CHANGE_MAX 360
#define DELAY_CHANGE_MIN 6
#define LCD_TEMP_MAX    70
#define LCD_TEMP_MIN    0
#define BT_MIN_MAX      700//480
#define BT_MIN_MIN      400//420
#define BT_MAX_MIN      400//560
#define BT_MAX_MAX      720
#define BT_RELOAD_MIN   400//420
#define BT_RELOAD_MAX   700//600
#define DELAY_DIS_MAX   250
#define DELAY_DIS_MIN   1
#endif

#if MODEL==71
#define BOOST_MAX       340
#define BOOST_MIN       270
#define TAPER_MAX       340
#define TAPER_MIN       260
#define BANK_MAX        6
#define BANK_MIN        1
#define FLOAT_MAX       300
#define FLOAT_MIN       240
#define BOOST_BELOW_MAX 300
#define BOOST_BELOW_MIN 240
#define COMP_TEMP_MAX   20
#define COMP_TEMP_MIN   0
#define DELAY_CHANGE_MAX 360
#define DELAY_CHANGE_MIN 6
#define LCD_TEMP_MAX    70
#define LCD_TEMP_MIN    0
#define BT_MIN_MAX      240
#define BT_MIN_MIN      210
#define BT_MAX_MIN      280
#define BT_MAX_MAX      360
#define BT_RELOAD_MIN   210
#define BT_RELOAD_MAX   300
#define DELAY_DIS_MAX   250
#define DELAY_DIS_MIN   1
#endif

/*----------------------*/
/*      变量缺省值定义  */
/*----------------------*/
#if MODEL==70
#define BOOST_DEFAULT_VALUE     600
#define TAPER_DEFAULT_VALUE     560
#define FLOAT_MAX_DEFAULT_VALUE 560
#define FLOAT_MIN_DEFAULT_VALUE 540
#define DELAY_CHANGE_DEFAULT_VALUE      60
#define BOOST_BELOW_DEFAULT_VALUE       496
#define COMP_TEMP_DEFAULT_VALUE         0
#define BANKS_DEFAULT_VALUE             6
#define LCD_TEMP_DEFAULT_VALUE          35
#define BT_MIN_DEFAULT_VALUE            448
#define BT_MAX_DEFAULT_VALUE            640
#define BT_RELOAD_DEFAULT_VALUE         512
#define DELAY_DISCONNECT_DEFAULT_VALUE  200
#endif

#if MODEL==71
#define BOOST_DEFAULT_VALUE     300
#define TAPER_DEFAULT_VALUE     280
#define FLOAT_MAX_DEFAULT_VALUE 280
#define FLOAT_MIN_DEFAULT_VALUE 270
#define DELAY_CHANGE_DEFAULT_VALUE      60
#define BOOST_BELOW_DEFAULT_VALUE       248
#define COMP_TEMP_DEFAULT_VALUE         0
#define BANKS_DEFAULT_VALUE             6
#define LCD_TEMP_DEFAULT_VALUE          35
#define BT_MIN_DEFAULT_VALUE            224
#define BT_MAX_DEFAULT_VALUE            320
#define BT_RELOAD_DEFAULT_VALUE         256
#define DELAY_DISCONNECT_DEFAULT_VALUE  200
#endif

/*------------------------------*/
/*      EPROM存储序列定义       */
/*------------------------------*/

#define SYSTEM_FLAG     0
#define BOOST           7
#define TAPER           8
#define FLOAT_MAX_S     9
#define FLOAT_MIN_S     10
#define BOOST_BELOW     11
#define COMP_TEMP       12
#define DELAY_CHANGE    13

#define BT_MIN          14
#define BT_RELOAD       15
#define BT_MAX          16
#define DELAY_DISCONNECT 17

#define LOCAL           18
#define HOUR            19
#define MINUTE          20
#define AUTO_UPLOAD     22
#define UP_HOUR         23
#define UP_MIN          24
#define PHONE           25
#define PASSWORD        37

#define CLIP            45

#define BT_V_MIN        62
#define BT_V_MAX        63
#define CHARGE_AH       64
#define DISCHARGE_AH    65


#define DAY             66

#define DATA_INDEX      67
#define DATA            68

/*----------------------*/
/*      光标设置        */
/*----------------------*/
#define ON      1
#define OFF     0

/*----------------------*/
/*      EPROM操作代码   */
/*----------------------*/
#define EWEN    0x01
#define EWDS    0
#define ERAL    0x0300
#define ERASE   0x0200

/*------------------------------*/
/*      A/D操作包含文件         */
/*      文件名: PROCESS.H       */
/*------------------------------*/

#define BT_V            0
#define SR_V0           1
#define SR_V1           2
#define SR_V2           3
#define SR_V3           4
#define SR_V4           5
#define SR_V5           6
#define LOAD_V          7
#define TEMP_BT         8
#define LOAD_I          10
#define BT_I            9
#define SR_I0           0x10
#define SR_I1           0x11
#define SR_I2           0x12
#define SR_I3           0x13
#define SR_I4           0x14
#define SR_I5           0x15

#define LAMP            0x10
#define ALARM_HI        0x20
#define ALARM_LO        0x40
#define UJ              0x80

#define bank0           0x1
#define bank1           0x2
#define bank2           0x4
#define bank3           0x8
#define bank4           0x10
#define bank5           0x20
#define LOAD            0X40
#define FMQ				0x80

#define LCD     1
#define PRINTER 0
