#ifndef INCLUDE_LV_DEFINE
#define INCLUDE_LV_DEFINE
#include "lv_add.h"
#include "lv_parad_c.h"
#include "lv_param_c.h"
#include "lv_chk842_d_c.h" 
#include "lv_chk842_m_c.h"
#include "lv_chk850_d_c.h"
#include "lv_chk850_m_c.h"
#include "lv_chk851_d_c.h"
#include "lv_chk851_m_c.h"
#include "lv_chk852_m_c.h"
#include "lv_chk852_d_c.h"
#include "lv_lbctl_c.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_sysctl_c.h"
#include "lvpack.h"
#include "lvnet.h"
#include "exitlvpack.h"

/**常量定义**/
#ifndef TLRLENGTH
#define TLRLENGTH 6
#endif
#ifndef __FL__
#define __FL__ __FILE__,__LINE__
#endif

#ifndef LV_WAIT_QS
#define LV_WAIT_QS "41403"
#endif

#ifndef LVLZGD
#define LVLZGD "41202"
#endif

#ifndef BOOL
#define BOOL int
#define TRUE      1
#define FALSE     0
#endif
#ifndef DB_NOTFOUND
#define DB_NOTFOUND 100
#endif
#ifndef DB_OK
#define DB_OK 0
#endif

#ifndef PARM_LVGZ          /* BEPS  小额挂账专用账号 */
#define PARM_LVGZ 8
#endif


#define LV_CZ_QSBRNO    "10001"    /**清算中心机构号**/
#define LV_QSBRNO    "313175000011"    /**清算中心参与者号**/
/**wyw **/
#ifndef PARM_CZ_LVLZGZ
#define PARM_CZ_LVLZGZ 10
#endif

/**业务类型编号txnum和包编号pkgno定义**/
/**普通贷记业务的所有业务类型编号**/
#define PKGNO_PTDJ    "001"   /*普通贷记包*/
#define TXNUM_HD      "00100" /*汇兑-----------无*/
#define TXNUM_WT      "00101" /*委托收款划回*/
#define TXNUM_TSCF    "00102" /*托收承付划回*/
#define TXNUM_GKDJ    "00103" /*国库资金贷记划拨*/
#define TXNUM_QTDJ    "00106" /*其他贷记业务---无*/
#define TXNUM_JS      "20001" /*缴税-----------无*/
#define TXNUM_GZDJ    "20005" /*国债贷记*/
#define TXNUM_FSSR    "20002" /*非税收入*/
#define TXNUM_JF      "30002" /*交费*/
/**普通借记业务的所有业务类型编号**/
#define PKGNO_PTJJ    "002"   /*普通借记包*/
#define PKGNO_PTJJHZ  "008"   /*普通借记回执包*/
#define TXNUM_GKJJ    "00113" /*国库资金借记划拨*/
#define TXNUM_QTJJ    "00119" /*其他借记业务------无*/
#define TXNUM_ZJZF    "20103" /*财政直接支付*/
#define TXNUM_SQZF    "20104" /*财政授权支付*/
#define TXNUM_ZJTH    "20003" /*财政直接支付退回*/
#define TXNUM_SQTH    "20004" /*财政授权支付退回*/
#define TXNUM_ZPJL    "30102" /*支票截留业务*/
#define TXNUM_GZJJ    "20105" /*国债借记*/
#define TXNUM_TYJL    "30103" /*通用截留业务*/
#define TXNUM_ZPJLHZ  "30104" /*支票截留业务回执*/
#define TXNUM_TYJLHZ  "30105" /*通用截留业务回执*/
/**贷记退汇业务所有业务类型编号**/
#define PKGNO_DJTH    "007"    /*贷记退汇包*/
#define TXNUM_DJTH    "00108"  /*贷记退汇业务------无*/
/**定期贷记业务**/
#define PKGNO_DQDJ    "005"    /*定期贷记包*/
#define TXNUM_DQDJ    "00104"  /*定期贷记业务------无*/

/**定期借记业务**/
#define PKGNO_DQJJ    "006"    /*定期借记包*/
#define PKGNO_DQJJHZ  "011"    /*定期借记回执包*/
#define TXNUM_DQJJ    "00114"  /*定期借记业务------无**/
#define TXNUM_PLKS    "20102"  /*批量扣税*/
/**实时贷记业务**/
#define PKGNO_SSDJ    "003"    /*实时贷记包*/
#define PKGNO_SSDJHZ  "009"    /*实时贷记回执包*/
#define TXNUM_TC      "30001"  /*通存业务*/
/**实时借记**/
#define PKGNO_SSJJ    "004"    /*实时借记包*/
#define PKGNO_SSJJHZ  "010"    /*实时借记回执包*/
#define TXNUM_SSKS    "20101"  /*实时扣税*/
#define TXNUM_TD      "30101"  /*通兑业务*/
#define TXNUM_DGTD    "XXXXX"  /*NEWYX:删除。对公通兑业务*/
/**通用信息**/
#define PKGNO_TYXX    "012"    /*通用信息包*/
#define TXNUM_TYXX    "00500"  /*通用信息业务*/
#define TXNUM_DF      "40501"  /*代付业务*/
#define TXNUM_DS      "40502"  /*代收业务*/
/**实时信息**/
#define PKGNO_SSXX    "013"    /*实时信息报文*/
#define TXNUM_ZPQC    "30501"  /*支票圈存*/
#define TXNUM_ZPYD    "30504"  /*支票圈存应答*/
#define TXNUM_ZSCX    "30505"  /*证书查询*/
#define TXNUM_ZSYD    "30506"  /*证书查询应答*/
#define TXNUM_ZHCX    "30502"  /*客户账户查询*/
#define TXNUM_CXYD    "30503"  /*客户账户查询应答*/
/**回执状态或退汇原因**/
#define RET_SUCCESS    "00"    /*成功*/
#define RET_NOACTNO    "01"    /*帐号不符*/
#define RET_NONAME     "02"    /*帐号户名不符*/
#define RET_NOMONEY    "03"    /*账户余额不足支付*/
#define RET_PSWDERR    "10"    /*账户密码错误*/
#define RET_ACTCLSERR  "11"    /*帐号状态错误*/
#define RET_HASCANCEL  "20"    /*业务已撤销*/
#define RET_OTHER      "90"    /*其他错误*/
/**委托收款凭证种类**/
#define WTVOC_CDHP     "01"    /*银行承兑汇票*/
#define WTVOC_DQCD     "02"    /*定期存单*/
#define WTVOC_PZGZ     "03"    /*凭证式国债*/
#define WTVOC_HQCZ     "04"    /*异地活期存折*/
#define WTVOC_OTHER    "99"    /*其他*/
/**交易状态**/
#define STAT_INVALID        '0' /*无效状态*/
#define STAT_WZLR           '1' /*往账录入*/
#define STAT_WZCHECK1       '2' /*往账复核，挂帐 --无效，20051130 by lhf*/
#define STAT_WZCHECK2       '3' /*往账复核，记帐*/
#define STAT_WZSEND         '4' /*往账发送*/
#define STAT_WZREJECT       '5' /*往账拒绝*/
#define STAT_WZSUCCESS      '6' /*往账扎差*/
#define STAT_WZINQUEUE      '7' /*往账排队*/
#define STAT_WZCANCEL_WAIT  '8' /*往账撤销等待*/
#define STAT_WZCANCEL       '9' /*往账撤销*/
#define STAT_WZBACK_WAIT    'A' /*往账退回、冲正、止付等待*/
                                /**退回--普通、定期贷记
                                ** 冲正--实时借记、贷记
                                ** 止付--普通、定期借记
                                ****************************/
#define STAT_WZBACK         'B'  /*往账退回*/
#define STAT_WZCZ           'C'  /*往账已冲正*/
#define STAT_WZZF           'D'  /*往账已止付*/
#define STAT_WZEXCEPT       'G'  /*往账异常挂帐*/

#define STAT_LZRECV1        'H'  /*临时接收,不记帐*/
#define STAT_LZRECV2        'I'  /*正式接收，正常记帐*/
#define STAT_LZNMAC_ERR     'J'  /*全国押错 -挂支行*/
#define STAT_LZLMAC_ERR     'K'  /*地方押错 -挂支行*/
#define STAT_LZAMAC_ERR     'L'  /*全国押和地方押都错 -挂支行*/
#define STAT_LZGZ_QS        'M'  /*信息不符，清算中心挂帐*/
#define STAT_LZGZ_BR        'N'  /*信息不符，支行挂帐*/
#define STAT_LZBACK_WAIT    'O'  /*来账退回等待*/
#define STAT_LZBACK         'P'  /*来账退回*/
#define STAT_YES_NOTSEND    'Q'  /*已做同意应答且未发送*/
#define STAT_YES_SEND       'R'  /*已做同意应答且已发送*/
#define STAT_NO_NOTSEND     'S'  /*已做不同意应答且未发送*/
#define STAT_NO_SEND        'T'  /*已做不同意应答且已发送*/
#define STAT_LZCZ           'U'  /*来账已冲正*/
#define STAT_LZZF           'V'  /*来账已止付*/
#define STAT_LZFK           'W'  /*来账已付款*/
#define STAT_LZEXCEPT       'Z'  /*来账异常挂帐*/
/**收付标志**/
#define SFFLAG_CASH         '0'  /*收*/
#define SFFLAG_PAY          '1'  /*付*/
/**查询帐号类型**/
#define QACTTYPE_ACTNO      '0'  /*银行帐号*/
#define QACTTYPE_CRCARD     '1'  /*贷记卡*/
#define QACTTYPE_CRECARD    '2'  /*信用卡*/
#define QACTTYPE_OTHER      '9'  /*其他*/
/**查询密码类型*/
#define QPSWDCD_ACTNO       '0'  /*账户密码*/
#define QPSWDCD_CARD        '1'  /*卡密码*/
#define QPSWDCD_TX          '3'  /*交易密码*/ 
#define QPSWDCD_QUERY       '4'  /*查询密码*/
#define QPSWDCD_OTHER       '9'  /*其他密码*/
/**账户查询类型*/
#define QTYPE_AVBAL         '1'  /*查询余额*/
/**扎差节点类型**/
#define ZCNODE_NPC          '1'  /*NPC扎差*/
#define ZCNODE_CCPC         '2'  /*CCPC扎差*/
/**辖内分发标志**/
#define FFFLAG_NO           '0'  /*不分发*/
#define FFFLAG_YES          '1'  /*分发*/
/**补发标志**/
#define BFFLAG_NORMAL       '0'  /*正常*/
#define BFFLAG_BF           '1'  /*补发*/
/**包状态，自定义状态*/
#define PSTAT_INVALID       '0'  /*无效*/
#define PSTAT_PACKING       '1'  /*正在组包*/
#define PSTAT_PACKED        '2'  /*已组包*/
#define PSTAT_SEND          '3'  /*已发送*/
#define PSTAT_WRESP         '4'  /*往包已应答*/
#define PSTAT_RECEIVE       '5'  /*已接收*/
#define PSTAT_LRESP         '6'  /*已应答*/
/**包状态，按人行要求*/
#define PACKSTAT_REJECT     "01" /*已拒绝,检查未通过*/
#define PACKSTAT_SEND       "02" /*已发送,检查通过*/
#define PACKSTAT_SUCCESS    "03" /*已扎差*/
#define PACKSTAT_INQUEUE    "04" /*已排队*/
#define PACKSTAT_QS         "05" /*已清算*/
#define PACKSTAT_CANCEL     "06" /*已撤销*/
#define PACKSTAT_RECEIPT    "10" /*已回执*/
#define PACKSTAT_REJECTPAY  "11" /*已拒付*/
#define PACKSTAT_CZ         "12" /*已冲正*/
#define PACKSTAT_STOPPAY    "13" /*已止付*/
#define PACKSTAT_TIMEOUT    "14" /*已超时*/
/****批量服务状态(定时拼包程序)Add by SSH,2005.12.12****/
#define SERVSTAT_STOP		'0'	/****服务停止***/
#define SERVSTAT_START		'1'	/****服务启动***/
/**退回止付申请类型**/
#define REQTYPE_SINGLE      '0'  /*单笔*/
#define REQTYPE_WHOLE       '1'  /*整包*/
/**退汇止付申请的应答类型**/
#define RESP_CAN            '1'  /*允许*/
#define RESP_CANNOT         '2'  /*不允许*/
/**扎差类型**/
#define ZCTYPE_WORKING      '1'  /*日间扎差*/
#define ZCTYPE_WORKEND      '2'  /*日终扎差*/
/**借贷标志，按人行标准**/
#define CRDB_DR             '0'  /*借*/
#define CRDB_CR             '1'  /*贷*/
/**操作码**/
#define OPCD_LR_QUERY       "11" /*录入之查询*/
#define OPCD_LR             "10" /*录入*/
#define OPCD_CHECK          "30" /*复核*/
#define OPCD_SEND_QUERY     "41" /*发送之查询*/
#define OPCD_SEND           "40" /*发送*/
#define OPCD_RESP           "50" /*应答*/
#define OPCD_RECEIVE        "60" /*接收*/
#define OPCD_INACT_QUERY    "72" /*错账处理之查询*/
#define OPCD_INACT_MODIFY   "71" /*错账处理之修改*/
#define OPCD_INACT          "70" /*错账处理之入帐*/
#define OPCD_REV_RECEIVE    "80" /*冲来帐*/
#define OPCD_REV_SEND       "90" /*从往帐*/
/***********收取手续费标志****************************/
#define FEEFLAG_CASH    '0'  /**现金**/
#define FEEFLAG_CASHJJ  '1'  /**现金加急**/
#define FEEFLAG_ACT     '2'  /**转帐**/
#define FEEFLAG_ACTJJ   '3'  /**转帐加急**/
#define FEEFLAG_NO      '4'  /**不收**/
#define TX_SEND         0    /**发送交易**/
#define TX_RECEIVE      1    /**接收交易**/
/******包检查c8返回结果****/
#define PCHK_NEW    0 /***新包***/
#define PCHK_EXISTS 1 /***已存在**/
#define PCHK_UPDATE 2 /***已更新**/
/******打印控制标志*********/
#define PRTFLG_INVALID		'0'	/**无此分录***/
#define PRTFLG_HASNOT		'1'	/**分录未打印**/
#define PRTFLG_HAS			'2'	/**分录已打印***/
#define PRTFLG_CANCEL1		'3'	/**正分录未打，冲红未打**/
#define PRTFLG_CANCEL2		'4'	/**正分录未打，冲红已打**/
#define PRTFLG_CANCEL3		'5'	/**正分录已打，冲红未打**/
#define PRTFLG_CANCEL4		'6'	/**正分录已打，冲红已打**/
/****打印分录包的最大条数****/
#define MAX_FLCNT    3 /**这个值只做控制用，不能更改**/
/*****序号类型定义****/
#define LV_ACTYPE 	"96"		/****小额用序号类型****/
#define LV_FLGACTYPE    "95"		/****报文标识号类型****/
#define LV_ADDID_IND 	"94"       	/****小额附加域序号****/
#define LV_PACKID_IND 	"93"       	/****小额拼包序号****/

/**下面开始交换区数据类型定义**/
#define ATYPE_PAY_IN       "1000"  /*支付交易输入区*/
#define ATYPE_PAY_OUT1     "1001"  /*支付交易一般输出区*/
#define ATYPE_PAY_OUT2     "1002"  /*支付交易分录输出区*/
#define ATYPE_PAY_OUT3     "1003"  /*支付交易手续费输出区*/
#define ATYPE_NOTPAY_IN    "2000"  /*非支付交易输入区*/
#define ATYPE_NOTPAY_OUT   "2001"  /*非支付交易输出区*/
#define ATYPE_CMT_IN       "3000"  /*cmt输入区*/
#define ATYPE_CMT_OUT      "3001"  /*CMT输出区*/
#define BASE_EXCHANGE_BLOCK    char type[4]; \
                               char length[4];

#define LV_RECV_BRNO "9999999"
#define LV_RECV_TLRNO "9999"

#define _lv_errmsg lv_setf(__LINE__, __FILE__); lv_errmsg
typedef struct{
	int errline;
	char errfm[128];
	int rtcode;
	char lv_fd123[1000];
	char brno[8];
	char tlrno[8];
	char tbsdy[8];
	char lv_date[9];
	char txamt[16];
	char aptype;
	char txno[2];
	char wssrno[7];
	char sqlbuf[512];
	char errmsg[300];
}t_lvca  ;
t_lvca lvca;
#define GETHEADTAG_DATE(pk, s, lvd)   {GETHEADTAG(pk, s, lvca.lv_date); lvd = apatoi(lvca.lv_date, 8);}
#define lv_pkg_batch_date(pk, s, lvd) {lv_pkg_batch(pk, s, lvca.lv_date); lvd = apatoi(lvca.lv_date, 8);}
typedef struct {/**交换区基类型**/
    BASE_EXCHANGE_BLOCK
}BASE_EXCHANGE_AREA;
#define PAY_IN_FIX_BLOCK \
    char    F_crdb   [1];/**借贷标志0借1贷**/ \
    char    F_opcd   [2];/**操作代码    10－录入之录入11－录入之查询 \
                           20－修改之修改21－修改之查询30－复核 \
                           40－发送之发送41－发送之查询50－应答 \
                           60－接收70－错账处理之入帐 \
                           71－错账处理之修改72－错账处理之查询 \
                           80--冲来帐 90--冲往帐\
                         **/ \
    char    F_pkgno      [3 ]  ;/**Package编号    **/ \
    char    F_orderno    [8 ]  ;/**支付交易序号   **/ \
    char    F_txnum      [5 ]  ;/**业务类型编号   **/ \
    char    F_orbrno     [12]  ;/**发起行行号     **/ \
    char    F_acbrno     [12]  ;/**接收行行号     **/ \
    char    F_txamt      [15]  ;/**交易金额       **/ \
    char    F_payno      [12]  ;/**付款人开户行   **/ \
    char    F_payqsno    [12]  ;/**付款人清算中心 **/ \
    char    F_payactno   [32]  ;/**付款人帐号     **/ \
    char    F_payname    [60]  ;/**付款人名称     **/ \
    char    F_payaddress [60]  ;/**付款人地址     **/ \
    char    F_userpswd   [8]   ;/**储户密码       **/ \
    char    F_cashqsno   [12]  ;/**收款人清算中心**/ \
    char    F_cashno     [12]  ;/**收款人开户行行号**/ \
    char    F_cashactno  [32]  ;/**收款人帐号     **/ \
    char    F_cashname   [60]  ;/**收款人名称     **/ \
    char    F_cashaddress[60]  ;/**收款人地址     **/ \
    char    F_ywtype     [12]  ;/**业务种类       **/ \
    char    F_content    [60]  ;/**附言           **/ \
    char    F_wtday      [8]   ;/**委托日期       **/ \
    char    F_operlevel  [1]   ;/**优先级         **/ \
    char    F_authno[TLRLENGTH];/**授权操作员号   **/ \
    char    F_authpswd[8]      ;/**授权操作员密码 **/ \
    char    F_voctype [2]      ;/**凭证种类       **/ \
    char    F_vocnum  [12]     ;/**凭证号码       **/ \
    char    F_packday [8]      ;/**包日期,定期用  **/ \
    char    F_packid  [8]      ;/**包id,定期用    **/ \
    char    F_feeflag [1]      ;/**手续费标志     **/ \
    char    F_feeamt  [15]     ;/**手续费金额     **/ \
    /**以上字段构成标准输入部分，后面的内容随交易不同而选择性赋值**/ \
    char    F_respday [8] ;/**期待应答日期  **/ \
    char    F_ptype   [2] ;/**票据类型      **/ \
    char    F_pdate   [8] ;/**票据日期      **/ \
    char    F_pnum    [8] ;/**票据号码      **/ \
    char    F_ptxamt  [15];/**赔偿金额      **/ \
    char    F_jtxamt  [15];/**拒付金额      **/ \
    char    F_owtday  [8] ;/**原委托日期    **/ \
    char    F_otxnum  [5] ;/**原业务编号    **/ \
    char    F_oorderno[8] ;/**原支付交易序号**/ \
    char    F_retcode [2] ;/**退汇原因代码  **/ \
    char    F_rcpstat [2] ;/**业务回执状态  **/ \
    char    F_resp1   [8] ;/**Mbfe应答码    **/ \
    char    F_resp2   [8] ;/**Ccpc应答码    **/ \
    char    F_packend [1] ;/**包结束标志    **/ \
    char    F_lvsts   [1] ;/**交易状态      **/ \
    char    F_lw_ind  [1] ;/**来往标识	    **/ \
    char    F_dfxyno  [60];/**多方协议号    **/ \
    char    F_o_or_br_no [12];/**原发起行行号  **/ \
    char    F_o_ac_br_no [12];/**原接收行行号  **/
typedef struct {
    BASE_EXCHANGE_BLOCK
    PAY_IN_FIX_BLOCK
}PAY_IN_FIX_AREA;
typedef struct {/**支付交易输入包交换区**/
    BASE_EXCHANGE_BLOCK
    /**输入部分**/
    PAY_IN_FIX_BLOCK
    char    F_actype  [1] ;/**通存通兑账号类型***/ 
    char    F_pswdtype[1] ;/**通存通兑密码类型**/ 
    char    F_pswd    [8] ;/**通存通兑密码****/
    
    char    F_payunit[60] ;/**付费(税)单位**/
    char    F_payterm[16] ;/**付费(税)期间**/
    char    F_paytype[2]  ;/**付费(税)种类**/
    /**表中取的内容**/
    char    T_pkgno      [3]    ;/**Package编号     **/
    char    T_inday      [8]    ;/**进入系统日期    **/
    char    T_jzday      [8]    ;/**记帐日期        **/
    char    T_wtday      [8]    ;/**委托日期        **/
    char    T_packday    [8]    ;/**打包日期        **/
    char    T_orderno    [8]    ;/**支付交易序号    **/
    char    T_ywdtlid    [8]    ;/**业务序号，定期业务用**/
    char    T_txtime     [14]   ;/**交易时间        **/
    char    T_txnum      [5]    ;/**业务类型编号    **/
    char    T_orbrno     [12]   ;/**发起行行号      **/
    char    T_payqsactno [12]   ;/**发起清算行行号  **/
    char    T_sendco     [4]     ;/**发报中心       **/
    char    T_acbrno     [12]   ;/**接收行行号      **/
    char    T_cashqsactno[12]   ;/**接收清算行行号  **/
    char    T_receco     [4]    ;/**收报中心        **/
    char    T_txamt      [15]   ;/**交易金额        **/
    char    T_payno      [12]   ;/**付款人开户行    **/
    char    T_payactno   [32]   ;/**付款人帐号      **/
    char    T_payname    [60]   ;/**付款人名称      **/
    char    T_payaddress [60]   ;/**付款人地址      **/
    char    T_cashno     [12]   ;/**收款人开户行    **/
    char    T_cashactno  [32]   ;/**收款人帐号      **/
    char    T_cashname   [60]   ;/**收款人名称      **/
    char    T_cashaddress[60]   ;/**收款人地址      **/
    char    T_ywtype     [12]   ;/**（原）业务种类  **/
    char    T_content    [60]   ;/**附言            **/
    char    T_respday    [8]    ;/**应答日期        **/
    char    T_rcpstat    [2]    ;/**业务回执状态    **/
    char    T_resp1      [8]    ;/**Mbfe应答码      **/
    char    T_resp2      [8]    ;/**Ccpc应答码      **/
    char    T_voctype    [2]    ;/**凭证种类        **/
    char    T_vocnum     [12]   ;/**凭证号码        **/
    char    T_ptype      [2]    ;/**票据类型        **/
    char    T_pdate      [8]    ;/**票据日期        **/
    char    T_pnum       [8]    ;/**票据号码        **/
    char    T_ptxamt     [15]   ;/**赔偿金额        **/
    char    T_jtxamt     [15]   ;/**拒付金额        **/
    char    T_actype     [1]    ;/**通存通兑帐号类型**/
    char    T_pswdtype   [1]    ;/**通存通兑密码类型**/
    char    T_pswdcode   [128]  ;/**通存通兑密码交换码**/
    char    T_payunit    [60]   ;/**付费(税)单位    **/
    char    T_payterm    [16]   ;/**付费(税)期间    **/
    char    T_paytype    [2]    ;/**付费(税)种类    **/
    char    T_brno       [7]    ;/**交易机构        **/
    char    T_inputno[TLRLENGTH];/**录入操作员    **/
    char    T_checkno[TLRLENGTH];/**复核操作员    **/
    char    T_authno [TLRLENGTH];/**授权操作员    **/
    char    T_sendno [TLRLENGTH];/**发送操作员    **/
    char    T_respno [TLRLENGTH];/**应答操作员    **/
    char    T_recvno [TLRLENGTH];/**接收操作员号  **/
    char    T_dealno [TLRLENGTH];/**错账处理操作员号**/
    char    T_inwssrno   [6]    ;/**录入流水号    **/
    char    T_checkwssrno[6]    ;/**复核流水号    **/
    char    T_sendwssrno [6]    ;/**发送流水号    **/
    char    T_respwssrno [6]    ;/**应答流水号    **/
    char    T_recvwssrno [6]    ;/**接收操作流水号**/
    char    T_dealwssrno [6]    ;/**错账处理流水号**/
    char    T_stat       [1]    ;/**交易状态      **/
    char    T_begstat    [1]    ;/**初始交易状态  **/
    char    T_operlevel  [1]    ;/**优先级        **/
    char    T_feeflag    [1]    ;/**手续费标志    **/
    char    T_feeamt     [15]   ;/**手续费金额    **/
    char    T_checkflag  [1]    ;/**对账标志      **/
    char    T_brprtflag  [64]   ;/**支行打印标志  **/
    char    T_qsprtflag  [64]   ;/**清算中心打印标志**/
    char    T_packid     [8]    ;/**所属包        **/
    char    T_owtday     [8]    ;/**原委托日期    **/
    char    T_otxnum     [5]    ;/**原业务编号    **/
    char    T_retcode    [2]    ;/**退汇原因代码  **/
    char    T_ostat      [1]    ;/**原交易状态    **/
    char    T_oorderno   [8]    ;/**原支付交易序号**/
    char    T_addid      [8]    ;/**附加数据域id  **/
    char    T_lw_ind     [1]    ;/**来往标识      **/
    char    T_filler     [60]   ;/**备注          **/
    char    T_dfxyno     [60]   ;/**多方协议号    **/
    char    T_o_or_br_no [12];
    char    T_o_ac_br_no [12];
    char    T_o_pack_date[8];
    char    T_o_packid   [8];
    /**自动获取内容**/
    char    A_tbsdy      [8]    ;/**当前日期**/
    char    A_wssrno     [6]    ;/**当前操作员流水号**/
    char    A_stat       [1]    ;/**状态,这个字段对来账有效，
                                在做检查时赋值，初值为正常接收**/
    char    A_sendco     [4]    ;/**发报中心**/
    char    A_payqsactno[12]    ;/**发起清算行行号**/
    char    A_receco     [4]    ;/**收报中心**/
    char    A_cashqsactno[12]   ;/**接收清算行**/        
    char    A_acno       [7]    ;/**客户科目**/
    char    A_avbal1     [15]   ;/**走账前余额**/
    char    A_avbal2     [15]   ;/**走账后余额**/
    char    A_oldrespday [8]    ;/**保存原来账期待应答日期**/
    char    A_brno       [7]	;/**保存来帐算出的机构号****/
    char    A_acttype    [1]	;/**帐号类型,为了节省来帐检查时间,见check3****/
}PAY_IN_AREA;            
typedef struct{
    BASE_EXCHANGE_BLOCK
    /**固定部分**/
    char txname[20]            ;/**交易名称            **/           
    char txday[8]              ;/**交易日期,小额日期   **/
    char brno[7]               ;/**交易机构            **/
    char orderno[8]            ;/**支付交易序号        **/
    char txtype[12]            ;/**业务类型,Txnum的描述**/
    char orbrno[12]            ;/**发起行行号            **/
    char payno[12]             ;/**付款人开户行        **/
    char acbrno[12]            ;/**接收行                **/
    char cashno[12]            ;/**收款人开户行        **/
    char txamt[15]             ;/**交易金额            **/
    char payactno[32]          ;/**付款人帐号            **/
    char payaddress[60]        ;/**付款人地址            **/
    char payname[60]           ;/**付款人户名            **/
    char cashactno[32]         ;/**收款人帐号            **/
    char cashname[60]          ;/**收款人户名            **/
    char cashaddress[60]       ;/**收款人地址            **/
    char ywtype[12]            ;/**业务种类,电汇要把代码转换为描述**/
    char content[60]           ;/**附言    **/
    char tlrno[TLRLENGTH]      ;/**操作员    **/
    char authno[TLRLENGTH]     ;/**授权员    **/
    char wssrno[TLRLENGTH]     ;/**流水号    **/
    /**补充部分**/
    char owtday[8]   ;/**原委托日期    退汇用        **/
    char otxtype[12] ;/**原业务类型    Txnum的描述   **/
    char oorderno[8] ;/**原支付交易序号              **/
    char retdtl[12]  ;/**退汇原因    Retcode的描述 **/
    char feeflag[1]  ;/**手续费标志                  **/
    char feeamt[15]  ;/**手续费金额                  **/
    char ptype[2]    ;/**票据种类    委托          **/
    char pdate[8]    ;/**票据日期                  **/
    char pnum[8]     ;/**票据号码                  **/
    char ptxamt[15]  ;/**赔偿金额                  **/
    char jtxamt[15]  ;/**拒付金额                  **/
    char cashqsactno[12]; /* 收款行清算行号 */
}PAY_OUT1_AREA;
typedef struct{            /**分录输出包***/
    BASE_EXCHANGE_BLOCK
    char dracno1[12]        ;/**借方科目**/
    char cracno1[12]        ;/**贷方科目**/
    char dracno2[12]        ;/**借方科目**/
    char cracno2[12]        ;/**贷方科目**/
    char dracno3[12]        ;/**借方科目**/
    char cracno3[12]        ;/**贷方科目**/        
}PAY_OUT2_AREA;
typedef struct{            /**手续费输出包***/
    BASE_EXCHANGE_BLOCK
    char feeamt[15]            ;/**手续费金额***/
    char dracno[12]            ;/**借方科目****/
    char cracno[12]            ;/**贷方科目****/
}PAY_OUT3_AREA;
/****定期贷记明细定义*******/
typedef struct{
	char orderno[8];		/**支付交易序号**/
	char ywdtlid[8];		/**业务序号**/
	char acbrno[12];		/**收款行行号**/
	char cashno[12];		/**收款人开户行行号**/
	char cashactno[32];		/**收款人帐号**/
	char cashname[60];		/**收款人名称**/
	char cashaddress[60];	/**收款人地址**/
	char txamt[15];			/**交易金额**/
	char content[60];		/**附言**/
	char addlen[8];			/**附加数据长度**/
	char zero;				/**结尾0****/
}DQDJ_DTL_AREA;

/****定期借记明细定义*******/
typedef struct{
	char orderno[8];		/**支付交易序号**/
	char ywdtlid[8];		/**业务序号**/
	char acbrno[12];		/**付款行行号**/
	char payno[12];			/**付款人开户行行号**/
	char payactno[32];		/**付款人帐号**/
	char payname[60];		/**付款人名称**/
	char payaddress[60];	/**付款人地址**/
	char txamt[15];			/**交易金额**/
	char protno[60];		/**多方协议号**/
	char content[60];		/**附言**/
	char addlen[8];			/**附加数据长度**/
	char zero;				/**结尾0****/
}DQJJ_DTL_AREA;

/**函数声明**/
/**根据前台tis包对pay_in赋值**/
int lv_tis_to_pay_in(PAY_IN_AREA *wp_pay_in);
/**根据pay_in输入内容产生一条新的支付记录*/
int lv_newrec_from_pay_in(PAY_IN_AREA *wp_pay_in);
/**根据pay_in表数据区反填输入区内容**/
int lv_reset_pay_in(PAY_IN_AREA *wp_pay_in);
/**根据pay_in输入内容提取一条支付记录**/
int lv_fetchrec_to_pay_in(PAY_IN_AREA *wp_pay_in);
/**根据pay_in中的内容更新数据库表,并插入明细**/
int lv_updrec_from_pay_in(PAY_IN_AREA *wp_pay_in,int insflg);
/**通用检查**
int lv_pay_in_check0(PAY_IN_AREA *wp_pay_in);
*/
/****行号检查******************************
int lv_pay_in_check1(PAY_IN_AREA *wp_pay_in);
*/
/****付款帐号检查******************************
int lv_pay_in_check2(PAY_IN_AREA *wp_pay_in);
*/
/****收款帐号检查*****************************
int lv_pay_in_check3(PAY_IN_AREA *wp_pay_in);
*/
/****要素合法性检查*************************
int lv_pay_in_check4(PAY_IN_AREA *wp_pay_in);
*/
/****授权关系检查**************************
int lv_pay_in_check5(PAY_IN_AREA *wp_pay_in);
*/
/****要素一致性检查**************************
int lv_pay_in_check6(PAY_IN_AREA *wp_pay_in);
*/
/****记录存在性检查**************************
int lv_pay_in_check7(PAY_IN_AREA *wp_pay_in);
*/
/****包检查******************************
int lv_pay_in_check8(PAY_IN_AREA *wp_pay_in,struct wd_lv_wbctl_area *wp_lv_wbctl,struct wd_lv_lbctl_area *wp_lv_lbctl);
*/
/**根据pay_in内容产生一条新的回执记录*/
/***只填充输入域，表数据可以调用newrec函数来生成***
int lv_hz_from_pay_in(PAY_IN_AREA *wp_pay_in,PAY_IN_AREA *wp_pay_in_hz);
*/
/**根据pay_in内容产生一条新的退汇记录*/
/***只填充输入域，表数据可以调用newrec函数来生成***
int lv_th_from_pay_in(PAY_IN_AREA *wp_pay_in,PAY_IN_AREA *wp_pay_in_th);
*/
/****根据输入内容拼一般输出包****
int lv_set_pay_out1(PAY_IN_AREA  *wp_pay_in,PAY_OUT1_AREA *wp_pay_out1);
int lv_set_pay_out2(PAY_IN_AREA  *wp_pay_in,PAY_OUT2_AREA *wp_pay_out2);
*/
/****根据输入区域内容拼手续费输出包**
int lv_set_pay_out3(PAY_IN_AREA  *wp_pay_in,PAY_OUT3_AREA *wp_pay_out3);
*/
/****一般输出包****
int lv_pay_putmsg1(PAY_OUT1_AREA *wp_pay_out1,char flag);
*/
/****分录输出包****
int lv_pay_putmsg2(PAY_OUT2_AREA *wp_pay_out2,char flag);
*/
/****手续费输出包****
int lv_pay_putmsg3(PAY_OUT2_AREA *wp_pay_out3,char flag);
*/
/****设置服务状态****
int lv_setprg(char *prgname,char prgstat,int pid );
*/
/****得到服务状态****
int lv_getprgstat(char *prgname);
*/
/****得到服务pid***
int lv_getprgpid(char *prgname);
*/


/*
char *lv_getday();**得到小额系统日期**/    
/**判断一笔交易是发送交易还是接收交易**
int lv_get_orient(PAY_IN_AREA *wp_pay_in);
*/
/****得到支付交易数据交换区的自动信息****
int lv_pay_in_autoinfo(PAY_IN_AREA *wp_pay_in);
*/
/****得到下一个包id,与交易机构无关****
int lv_get_packid();
*/
/****得到下一个支付交易序号,与交易机构相关***
int lv_get_orderno();
*/
/****得到交易的描述信息******
char *lv_pay_in_txname(PAY_IN_AREA *wp_pay_in);
*/
/****得到退汇描述代码**************
char* lv_fetch_retdtl(PAY_IN_AREA *wp_pay_in);
*/
/****判断一个内部帐号（取付款帐号）是否允许透支**
BOOL lv_account_cantz(PAY_IN_AREA *wp_pay_in);
*/
/****计算手续费金额********
int lv_count_feeamt(PAY_IN_AREA *wp_pay_in);
*/
/****算密码交换码****
char * lv_des_pswdcode(char *);
*/
/****逆算密码交换码****
char * lv_undes_pswdcode(char *);
*/
/****得到业务类型的描述******
char *lv_pay_in_txtype(char *txnum);
*/
/****得到业务种类的描述******
char *lv_pay_in_ywtype(char *pywtype);
*/
/***
char* pcLvGetRecvno();
char *pcLvGetDay();
char *pcLvGetDay_work();
char *pcPayinTxname(PAY_IN_AREA *wp_payin);
char* pcPayinRetdtl(PAY_IN_AREA *wp_payin);
char *pcLvPayinTxtype(char *txnum);
char *pcLvPayinYwtype(char *pywtype);
char * pcLvDesPswdCode(char *code);
char * pcUndesPswdCode(char *code);
char *Rtn_info_SqlBuf(int val);
char *Rtn_Tran_SqlBuf(int val);
******************/



/**************************************************************************************/
/*非支付交易结构体定义开始*/
typedef struct{
    char    optype [ 2];    /*操作类型*/
    char    pkgno  [ 3];    /*package编号*/
    char    cmtno  [ 3];    /*cmt编号*/
    char    orderno[ 8];    /*支付交易序号*/
    char    txnum  [ 5];    /*业务类型编号*/
    char    acbrno [12];    /*接收行行号*/
    char    rettype    ;    /*退回类型0-单笔1-整包*/
    char    respcode   ;    /*应答码1-同意2-不能*/
    char    opackday[8];    /*原批量包日期*/
    char    opkgno  [3];    /*原包类型号*/
    char    opackid [8];    /*原批量包id*/
    char    otxnum  [5];    /*原业务类型编号*/
    char    owtday  [8];    /*原委托日期*/
    char    oorderno[8];    /*原支付交易序号*/
    char    oorbrno [12];   /*原发起行行号*/
    char    oacbrno [12];   /*原接收行行号*/
    char    otxtype;        /*原交易种类*/
    char    cour     [3];   /*货币符号*/
    char    otxamt  [15];   /*原交易金额*/
    char    content[255];   /*内容*/
    char    refno[20];     /*报文参考号*/
    
    char    unit[60];        /*收付单位*/
    char    belongcyc[16];   /*所属周期*/
    char    sftype[2];       /*收付种类*/
    char    pdate[8];        /*出票日期*/
    char    pnum[20];        /*支票号码*/
    char    paybrname[60];   /*付款行名称*/
    char    actno[32];       /*出票人帐号，帐户查询业务时用做帐号*/
    char    name[60];        /*收款人名称，帐户查询业务时用作户名*/
    char    reason[60];      /*用途*/
    char    bsnum[2];        /*背书人数*/
    char    passwd[60];      /*支付密码*/
    char    datalen[8];      /*数据长度*/
    char    acttype;         /*帐号类型*/
    char    pswdcd;          /*密码类型*/
    char    pascode[128];    /*密码验证码*/
    char    qtype;           /*查询类型，查询应答时作为原查询类型*/

    /*自动获取信息*/
    char    orbrno     [12]; /*发起行行号*/
    char    payqsactno [12]; /*发起清算行行号*/
    char    sendco     [4];  /*发报中心*/
    char    cashqsactno[12]; /*接收清算行行号*/
    char    receco     [4];  /*收报中心*/
    char    wssrno     [6];  /*流水号*/
    char    txday      [8];  /*交易日期*/
    char    stat;            /*状态*/
    char    datatype;        /*附加数据类型0－内容1－文件*/
    char    addid      [8];  /*附加数据id*/
    /****Add by SSH,2006.2.8****/
    /***非支付交易的字段***/
    char    qorderno[8];     /****申请序号****/
    char    qwtday[8];       /****申请日期****/
    char    qorbrno[12];     /****申请行行号****/
    char    racbrno[12];     /****应答行行号****/
    char    rwtday[12];      /****应答日期****/
    char    rorderno[12];    /****应答序号****/
	/** add by LiuHuafeng 20060307 **/
	char    totamt[15];   /* 包中的总笔数 */
	char    totcnt[8];    /* 包中的总金额 */
}NOTPAY_IN_AREA;

/*非支付交易输出部分*/
typedef struct{
    char    filename [16];        /*交易名称*/
    char    txday    [8];         /*交易日期*/
    char    brno     [7];		  /*交易机构*/
    char    orderno  [8];         /*支付交易序号*/
    char    txtype   [12];        /*业务类型，txnum的描述*/
    char    orbrno   [12];        /*发起行行号*/
    char    acbrno   [12];        /*接收行行号*/
    char    content  [255];       /*内容*/
    char    wssrno   [6];         /*流水号*/
    char    sfflag;               /*收付标志0－收1－付*/
    char    unit     [60];        /*收付单位*/
    char    belongcyc[16];        /*费用周期*/
    char    pdate    [8];         /*出票日期*/
    char    pnum     [20];        /*支票号码*/
    char    paybrname[60];        /*付款行名称*/
    char    bsnum    [2];         /*背书人数*/
    char    datalen  [8];         /*数据长度*/
    char    reason   [60];        /*用途*/
    char    sftype   [2];         /*收付种类*/
    char    acttype;              /*帐户类型*/
    char    actno    [32];        /*帐号*/
    char    name     [60];        /*户名*/
    char    pswdcd;               /*密码类型*/
    char    passwd   [128];       /*密码验证码*/
    char    qtype;                /*查询类型*/
    char    respday  [8];         /*应答日期*/
    char    qcontent [60];        /*查询附言*/
    char    opackday [8];         /*原批量包日期*/
    char    opackid  [8];         /*原批量包id*/
    char    opkgno   [3];         /*原包类型号*/
    char    otxtype  [12];        /*原业务类型，otxnum的描述*/
    char    oorderno [8];         /*原支付交易序号*/
    char    avbal    [15];        /*当前余额*/
}NOTPAY_OUT_AREA;

/*非支付交易标准输入部分，前台传过来的tis结构*/
typedef struct{
    char    optype   [2];   /*操作类型*/
    char    pkgno    [3];   /*package编号*/
    char    cmtno    [3];   /*cmt编号*/
    char    orderno  [8];   /*支付交易序号*/
    char    txnum    [5];   /*业务类型编号*/
    char    acbrno   [12];  /*接收行行号*/
    char    rettype     ;   /*退回类型0-单笔1-整包*/
    char    respcode    ;   /*应答码1-同意2-不能*/
    char    opackday [8];   /*原批量包日期*/
    char    opkgno   [3];   /*原包类型号*/
    char    opackid  [8];   /*原批量包id*/
    char    otxnum   [5];   /*原业务类型编号*/
    char    owtday   [8];   /*原委托日期*/
    char    oorderno [8];   /*原支付交易序号*/
    char    oqorderno[8];   /*原查询书号*/
    char    oqday    [8];   /*原查询日期*/
    char    oorbrno  [12];  /*原发起行行号*/
    char    oacbrno  [12];  /*原接收行行号*/
    char    otxtype     ;   /*原交易种类*/
    char    cour     [3];   /*货币符号*/
    char    otxamt   [15];  /*原交易金额*/
    char    reason   [255]; /*附言*/
}NOTPAY_IN_COMM;

/*增加一个自动赋值部分*/
typedef struct{
    char    orbrno     [12];
    char    payqsactno [12];
    char    sendco     [4];
    char    cashqsactno[12];
    char    receco     [4];
    char    wssrno     [6];
    char    txday      [8];
    char    stat          ;
}NOTPAY_IN_AUTO;


/*非支付交易标准输出部分*/
typedef struct{
    char    filename[16];       /*交易名称*/
    char    txday   [8];        /*交易日期，小额日期*/
    char    brno    [7];		 /*交易机构*/
    char    orderno [8];        /*支付交易序号*/
    char    txtype  [12];       /*业务类型，txnum的描述*/
    char    orbrno  [12];       /*发起行行号*/
    char    acbrno  [12];       /*接收行行号*/
    char    content [255];      /*内容*/
    char    wssrno  [6];        /*流水号*/
}NOTPAY_OUT_COMM;

/*cmt报文输出公共部分*/
typedef struct{
    char    opackday[8];  /*原批量包日期*/
    char    opackid [8];  /*原批量包id*/
    char    oorderno[8];  /*原支付交易序号*/
    char    opkgno  [3];  /*原包类型号*/
    char    otxtype [12]; /*原业务类型，otxnum的描述*/
}NOTPAY_OUT_CMT;



/*支票圈存*/
typedef struct{
    char    pdate    [8];   /*出票日期*/
    char    pnum     [20];  /*支票号码*/
    char    paybrname[60];  /*付款行名称*/
    char    payactno [32];  /*出票人帐号*/
    char    cashname [60];  /*收款人名称*/
    char    reason   [60];  /*用途*/
    char    bsnum    [2];   /*背书人数*/
    char    passwd   [60];  /*支付密码*/
    char    piclen   [8];   /*票据图像数据长度*/
}ZPQC_ADD;


/*已清算对帐记录清单*/
typedef struct{
    char    zclx    [1]; /*轧差节点类型*/
    char    zcday   [8]; /*轧差日起*/
    char    zccc    [2]; /*轧差场次*/
    char    pkgno  [3]; /*包类型号*/
    char    wbcnt   [8]; /*往包总数*/
    char    wbtxamt [22];/*往包金额合计*/
    char    lbcnt   [8]; /*来包总数*/
    char    lbtxamt [22];/*来包金额合计*/
}FINISH_QS;
/*其他状态对帐记录清单*/
typedef struct{
    char    pkgno  [3]; /*包类型号*/
	char    packstat[2]; /* 包处理状态 */
    char    wbcnt   [8]; /*往包总数*/
    char    wbtxamt [22];/*往包金额合计*/
    char    lbcnt   [8]; /*来包总数*/
    char    lbtxamt [22];/*来包金额合计*/
}UNFINISH_QS;

/*已清算对帐明细记录头清单*/
typedef struct{
	char    lwbs    [1]; /* 来往帐标识 1往帐,2来帐*/
    char    zcjdlx  [1]; /*轧差节点类型*/
    char    zcday   [8]; /*轧差日起*/
    char    zccc    [2]; /*轧差场次*/
    char    pkgno  [3]; /*包类型号*/
    char    cnt     [8]; /*往包总数*/
}MX_FINISH_QS_HEAD;
/*已清算对帐明细记录体清单*/
typedef struct{
	char    payqsactno  [12]; /* 包发起清算行*/
    char    packday  [8]; /*包委托日期*/
    char    packid   [8]; /*包序号*/
    char    dtlcnt  [8]; /*包总笔数*/
    char    totamt  [22]; /*包总金额*/
    char    qsday   [8]; /*清算日期*/
}MX_FINISH_QS_BODY;

/*已清算对帐明细记录清单*/
typedef struct{
	char    lwbs    [1]; /* 来往帐标识 1往帐,2来帐*/
	char    pkgno  [3]; /*包类型号*/
	char    packstat[2]; /* 包处理状态 */
	char    cnt     [8]; /*往包总数*/
}MX_UNFINISH_QS_HEAD;

/*其他状态应答明细清单*/
typedef struct{
	char    payqsactno  [12]; /* 包发起清算行*/
	char    packday  [8]; /*包委托日期*/
	char    packid   [8]; /*包序号*/
	char    dtlcnt  [8]; /*包总笔数*/
	char    totamt  [22]; /*包总金额*/
}MX_UNFINISH_QS_BODY;

typedef struct{
	char    lwbs    [1]; /* 往/来账标识 */
	char    zcjdlx  [1]; /* 轧差节点类型*/
	char    zcday   [8]; /* 轧差日期 */
	char    zccc    [2]; /* 轧差场次 */
	char    pkgno   [3]; /* 包类型号 */
}MXHD_850_QS;
typedef struct{
	char    lwbs    [1]; /* 往/来账标识 */
	char    pkgno   [3]; /* 包类型号 */
	char    packstat[2]; /* 包处理状态 */
}MXHD_850_QT;

/*added by 卿苏德 2006-02-16*/
typedef struct{
	char countno[19]; /*18位帐号*/
	char name[61]; /*户名*/
	char leftvalue[17]; /*余额*/
}HT_INTERFACE;

/*liuyue 20060912**由于小额的非支付采用了大额的123域,下面几个字段需要另外处理***/
typedef struct{
	char opackid  [8];
	char opack_date[8];
	char otxnum    [5];
	char opkgno    [3];
	char reqtype   [1];
}LV_NOTPAY_ADD;

#define        TXNUM_LEN             5
#define        NOTPAY_IN_COMM_LEN    sizeof(NOTPAY_IN_COMM)
#define        NOTPAY_IN_AUTO_LEN    sizeof(NOTPAY_IN_AUTO)
#define        NOTPAY_OUT_COMM_LEN   sizeof(NOTPAY_OUT_COMM)
#define        SFYW_ADD_LEN          sizeof(SFYW_ADD)
#define        ZPQC_ADD_LEN          sizeof(ZPQC_ADD)
#define        ZHCX_ADD_LEN          sizeof(ZHCX_ADD)
#define        ZHCXYD_ADD_LEN        sizeof(ZHCXYD_ADD)
#define        NOTPAY_OUT_CMT_LEN    sizeof(NOTPAY_OUT_CMT)
/*报文部分由公共部分和附加部分共同组成*/
/*非支付交易结构体定义结束*/
/*******************************************************************************************/
/** 定义小额业务一些科目 *******/
#define  LV_NEXTDAY_GZ_HANDFEE_INCOME   HV_SUB03_HAND_INCOME /* 下额换日后手续费收入挂帐科目 */
#define  LV_NEXTDAY_GZ_RECV   HV_QSGZ_ACNO   /* 小额换日后来帐挂帐科目 */
#define  LV_NEXTDAY_GZ_SEND   HV_QSGZ_ACNO   /* 小额换日后往帐挂帐科目 */

/*** 定义lv_pay_in_check2函数检查帐户状态返回信息 **/
#define ACT_OK          0   /* 帐户状态正常 */
#define ACT_ERR         1   /* 帐号不存在 */
#define ACT_NAME_ERR    2   /* 帐户户名错误 */
#define ACT_AVBAL_LOW   3   /* 帐户余额不足 */
#define ACT_PASSWD_ERR 10   /* 帐户密码错误 */
#define ACT_FZN        11   /* 状态错误 */
#define ACT_OTHER_ERR  -1   /* 其他错误 */
/*** 定义小额往帐业务被拒绝是否挂账还是原路冲回 ***/
#define REJECT_ACMR_FLAG  1 /* 1 原路冲回 其他是挂账*/
/* 定义待清算的过渡科目 */
#define WAIT_QS_SUBJECT "4140300" /* 待清算过渡科目*/
/*#define LV_SUB02_SEND "4110500"  小额支付往帐*/
#define LV_SUB02_SEND "41102" /*小额支付往帐*/
#define LV_SUB02_RECEIVE "4120500" /* 小额支付来帐*/
#define LV_SUB02_HAND_INCOME   "51108"   /*小额手续费收入*/
#define LV_SUB02_HAND_PAYOUT   "52705"   /*小额手续费支出*/
#define LV_QSGZ_ACNO  "2610500"   /*小额挂账科目*/
char* lv_getrecvno();
/****HSYX BEGIN :验印结果****/
#define CHY_NOTCHECK '0'
#define CHY_CHECKSUCCESS '1'
#define CHY_CHECKFAIL    '2'
/****HSYX END :验印结果****/

/*********定义小额的营业状态*************/
#define LV_STAT_PREPARE		'0' 	/*营业准备*/
#define LV_STAT_WORK		'1'		/*日间处理*/
#define LV_STAT_WARN		'2'		/*截至准备*/
#define LV_STAT_STOP		'3'		/*业务截至*/
#define LV_STAT_QSWIN		'4'		/*清算窗口*/
#define LV_STAT_WORKEND		'5'		/*日终处理开始*/
#define LV_STAT_CHECKERR	'6'		/*日终对账不平*/
#define LV_STAT_CHECKRIGHT	'7'		/*日终对账已经平*/
#define LV_STAT_UNKNOWN		'F'		/*未知状态*/

/***********************************************
    有关华腾接口的定义 2006-3-4 20:38 卿苏德
***********************************************/
#define HT_SENDBUF_LEN			151
#define HT_RECVBUF_LEN    	147

struct str_czzf_area{
	char dwbh[4];/**财政支付单位编号**/
	char capital;/**财政支付资金性质**/
	char finvoc[20];/**财政支付凭证号**/
	char vocnum[12];
	char voctype[2];
};
/*****************************************
* 查找支付业务原始包/明细基本信息结构体  *
* add by LiuHuafeng 20060309             *
*****************************************/
typedef struct{
	char    findtype   [1];/*0单笔 1 整个包 */
	char    packid     [8];/* findtype=1 时必须填 */
	char    packday    [8];
	char    wtday      [8];/*必须填 */
	char    orbrno     [12];/*发起行 */
	char    orderno    [8]; /* 支付交易序号 */
	char    pkgno      [3]; /*findtype=1时候必须填写 包类型 */
	char    lwbs       [1]; /*0往,1来,与人民银行统一 */
/* 以上是根据关键字找包/或者明细 */
	char    totcnt     [8];
	char    totamt     [15];
	char    payactno   [32];
	char    payname    [60];
	char    cashactno  [32];
	char    cashname   [60];
	char    acbrno     [12];
	char    payqsactno [12];
	char    cashqsactno[12];
	char    wssrno     [6];
	char    txday      [8];
	char    stat       [1];
	char    begstat    [1];
	char    rcpstat    [2];
}QUERY_PAY_INFO;

typedef struct{
	char pack_date[8];
	char packid[8];
	char respCode[8];
	char flg[1];/*结束标志*/
}CMT920_8583_JZ;

struct str_lv_852_mx_dtl{
char lw_ind    [ 1];
char pkg_no    [ 3];
char pay_qs_no [12];
char pack_date [ 8];
char packid    [ 8];
};
struct str_lv_852_mx_dtl_print{
char lw_ind    [ 2];
char pkg_no    [ 4];
char pay_qs_no [13];
long pack_date ;
char packid    [ 9];
};

struct str_lv_852_head_print{
       long wt_date ;
       char pay_qs_no [13];
       long dz_date ;
       long dtlcnt ;
};

struct str_lv_852_head{
       char wt_date[9] ;
       char pay_qs_no [13];
       char dz_date[9];
       char dtlcnt [9];
};

#endif /**结束INCLUDE_LV_DEFINE**/
