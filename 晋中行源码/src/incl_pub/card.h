#ifndef CARDHEADCODE
#define CARDHEADCODE "621223"
#endif

#ifndef ICCARDHEADCODE
#define ICCARDHEADCODE "621780"
#endif

#ifndef ICCARDMDMCODE
#define ICCARDMDMCODE "0024" 
#endif

#ifndef ICCARDNOTETYPE
#define ICCARDNOTETYPE "024"
#endif

#ifndef CARDNOTETYPE
#define CARDNOTETYPE "020"
#endif

#ifndef CARD_UNION_AREA
#define CARD_UNION_AREA "81"
#endif

#ifndef CARD_FILL_AREA
#define CARD_FILL_AREA "01"
#endif

#ifndef CARD_CVN_KEYA
#define CARD_CVN_KEYA "1234567890ABCDEF"
#endif

#ifndef CARD_CVN_KEYB
#define CARD_CVN_KEYB "FEDCBA0987654321"
#endif

#ifndef CARDNO_CREATE_TYPE
#define CARDNO_CREATE_TYPE 1 /* 1-从序号表中根据卡种类产生0-由柜面人员定卡号*/
#endif

#ifndef CVN_ENDDATE
#define CVN_ENDDATE	"4912"
#endif

#ifndef CVN_SERVCODE
#define CVN_SERVCODE	"120"
#endif


#ifndef CARD_AREA
#define CARD_AREA	"0355"
#endif

#ifndef CARD_MSR2_LEN
#define CARD_MSR2_LEN 34
#endif

#ifndef CARD_MSR3_LEN
#define CARD_MSR3_LEN 96
#endif

#ifndef CARD_NO_LEN
#define CARD_NO_LEN 17
#endif

#ifndef PARM_CARD_QS
#define PARM_CARD_QS 13 /* 卡存放太原（上海清算）款项 */
#endif

#ifndef PARM_CARD_OPEN_FEE
#define PARM_CARD_OPEN_FEE 14 /* 开卡行手续费支出科目/帐户参数 */
#endif

#ifndef PARM_CARD_USER_FEE
#define PARM_CARD_USER_FEE 15 /* 垫付用户手续费支出科目/帐户参数 */
#endif

/* 卡使用标志 1- 已经启用 */
#ifndef CARD_USE_FLAG_ON
#define CARD_USE_FLAG_ON '1'
#endif
/* 卡使用标志 0- 未启用 */
#ifndef CARD_USE_FLAG_OFF
#define CARD_USE_FLAG_OFF '0'
#endif

#ifndef CARD_ATM_PER_DAY_MAX_TX_AMT
#define CARD_ATM_PER_DAY_MAX_TX_AMT   20000.0
#endif

#ifndef CARD_ATM_PER_DAY_TRAN_MAX_TX_AMT
#define CARD_ATM_PER_DAY_TRAN_MAX_TX_AMT   50000.0
#endif

#ifndef CARD_ATM_PER_DAY_MAX_TX_CNT
#define CARD_ATM_PER_DAY_MAX_TX_CNT   5
#endif

#ifndef CARD_ATM_TX_FREE_CNT
#define CARD_ATM_TX_FREE_CNT   5
#endif

#ifndef CARD_ATM_QY_FREE_CNT
#define CARD_ATM_QY_FREE_CNT   5
#endif

#ifndef CARD_POS_QY_FREE_CNT
#define CARD_POS_QY_FREE_CNT   50000
#endif

#ifndef CARD_POS_TX_FREE_CNT
#define CARD_POS_TX_FREE_CNT   50000
#endif

#ifndef CARD_TRADE_CASH
#define CARD_TRADE_CASH   1
#endif

#ifndef CARD_TRADE_TRAN
#define CARD_TRADE_TRAN   2
#endif

#ifndef CARD_TRADE_QURY
#define CARD_TRADE_QURY   3
#endif

#ifndef PARM_CARD_AC      /* 应付卡年费*/
#define PARM_CARD_AC 46
#endif

#define KEY_TYPE_CVNA 1
#define KEY_TYPE_CVNB 2

#ifndef  DR_FLAG
#define  DR_FLAG   "1" /*借*/
#endif 
#ifndef  CR_FLAG
#define  CR_FLAG   "2" /*贷*/
#endif

/* 同一按照??标准36位处理 */
struct  str_cardmsr2_area{
	char cardno[CARD_NO_LEN+1]; /* 卡号17位 */
	char filler1[2]; /* = */
	char endday[5];  /* 4912 */
	char servercode[4]; /* 120 */
	char userdef[9];   /* 1+0355+cvn*/
};
/* 统一按照太商标准101位处理 */
struct  str_cardmsr3_area{
	char format     [3 ]; /* 99 */
	char cardno     [CARD_NO_LEN+1]; /* 卡号 17位 */
	char filer1     [2 ]; /* = */
	char cuntrycode [4 ]; /* 国家代号156 */
	char courcode   [4 ]; /* 货币代号156 */
	char exponent   [2 ]; /* 金融指数 0 */
	char cycauthamt [5 ]; /* 周期授权额5000 */
	char cycavbal   [5 ]; /* 本周期余额5000 */
	char cycbegin   [5 ]; /* 周期启示 2000 */
	char cyclength  [3 ]; /* 周期长度 01 */
	char pswdtime   [2 ]; /* 密码输入次数 3*/
	char authprivy  [7 ]; /* 写磁道控制码 000000*/
	char changectl  [2 ]; /* 交换控制符 1 */
	char pan_ta_sr  [3 ]; /* PAN的TA和SR 20 */
	char san1_ta_sr [3 ]; /* SAN-1的TA和SR 00*/
	char san2_ta_sr [3 ]; /* SAN-1的TA和SR 00*/
	char expirdate  [5 ]; /* 实效日期4912 */
	char cardseqno  [2 ]; /* 卡序号 1 */
	char secrecysign[2 ]; /* 卡保密号 = */
	char san1       [12]; /* san_1 cardserno 共11位人行最大12 */
	char filler2    [2 ]; /*字段分割符号 = */
	char san2       [10]; /* san_2 cardserno 共9位人行最大12 */
	char filler3    [2 ]; /*字段分割符号 = */
	char carryflag  [2 ]; /* 传送标志 1*/
	char descnt     [7 ]; /* 加密校验数 000000*/
	char filler4    [7 ]; /* 卡附加域 卡种类(1)+地区码(0355)+二磁道存在标志(1)*/
};

