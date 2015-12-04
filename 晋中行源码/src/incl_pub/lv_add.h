#ifndef __LV_ADD_H__
#define __LV_ADD_H__
/*************一些附加域结构声明************/    
/**汇兑业务-----(无附加域)**/
/**委托收款(划回)业务**/
typedef struct{
	char wtpzday[8];	/***委托凭证日期****/	
	char wtpzno [8];	/***委托凭证号码****/	
	char pztype [2];	/***凭证种类号	****/	
}WTSK_ADD_AREA;
/**托收承付(划回)业务**/
typedef struct{
	char tspzday[8 ];	/***托收凭证日期 ***/
	char tspzno [8 ];	/***托收凭证号码 ***/
	char pcamt  [15];	/***赔偿金金额	 ***/
	char jfamt  [15];	/***拒付金额	 ***/
	char oamt   [15];	/***原托金额	 ***/
	char zfamt  [15];	/***支付金额	 ***/
	char dfamt  [15];	/***多付金额	 ***/
}LVTSCF_ADD_AREA;
/**国库资金贷记划拨业务----同(国库资金借记划拨业务)**/
typedef struct{
	char infono  [8 ];	/**信息流水号      **/
	char amt     [18];	/**明细汇总金额    **/
	char upgkno  [10];	/**上报国库代码    **/
	char downgkno[10];      /**接收国库代码    **/
	char yslevel [1 ];	/**预算级次        **/
	char adjflag [1 ];	/**调整期标志      **/
	char ystype  [1 ];      /**预算种类        **/
	char rptday  [8 ];	/**报表日期        **/
	char rptno   [3 ];	/**报表序号        **/
	char dtlcnt  [3 ];	/**明细条数,固定为1**/
	char orgtype [10];	/**征收机关大类    **/
	char ysacno  [20];	/**预算科目代码    **/
	char dtlamt  [18];	/**发生额          **/
}GKDJ_ADD_AREA;
#define GKJJ_ADD_AREA GKDJ_ADD_AREA
/**定期贷记业务-----(无附加域)**/
/**贷记退汇业务-----(无附加域)**/
/**定期借记业务-----(无附加域)**/
/**其他借记业务-----(无附加域)**/
/**其他贷记业务-----(无附加域)**/
/**一般通用信息业务**/	
typedef struct{
	char title  [60]; 	/**标题         **/
	char content[255];	/**正文内容	**/
	char fjname [60]; 	/**附件文件名   **/
	char fjlen  [8 ];  	/**附件长度	**/
}TYXX_ADD_AREA;  
/**缴税----(无附加域)**/
/**实时扣税附加数据声明----同(批量扣税)**/
typedef struct{
	char orgno  [11];	/**征收机关代码	**/
	char wtday  [8 ];	/**委托日期	**/
	char wssrno [8 ];       /**交易流水	**/
	char ksday  [8 ];	/**扣税日期	**/
	char spno   [18];	/**税票号码	**/
	char filler1[60];	/**备注1     	**/
	char filler2[60];	/**备注2     	**/
	char filler3[60];	/**备注3     	**/
}SSKS_ADD_AREA;
#define PLKS_ADD_AREA SSKS_ADD_AREA
/****非税收入附加数据声明********/
typedef struct{
	char payunit[60];	/**付费单位  **/
	char payterm[16];	/**委托日期  **/
	char paytype[2];	/**收入种类  **/
}FSSR_ADD_AREA;
/****直接支付附加数据声明********/
typedef struct{
	char gkno    [10];	/**国库代码		**/
	char agacgtno[32];	/**代理人帐号		**/
	char agbrno  [12];	/**代理银行代码		**/
	char ystype  [1 ];	/**预算种类		**/
	char adjflag [1 ];	/**调整期标志		**/
	char zeroamt [18];	/**零余额汇总发生额	**/
	char hdamt   [18];	/**小额现金汇兑发生额	**/
	char vocnum  [8 ];	/**凭证编号		**/
	char vocday  [8 ];	/**凭证日期		**/
	char dtlcnt  [3 ];	/**明细条数,规定为1	**/
	char ysorgno [15];	/**预算单位代码		**/
	char gnacno  [20];	/**功能分类科目代码	**/
	char jjacno  [20];	/**经济分类科目代码	**/
	char acttype [1 ];	/**帐户性质		**/
	char dtlamt  [18];	/**明细金额		**/
}ZJZF_ADD_AREA;
#define SQZF_ADD_AREA ZJZF_ADD_AREA
#define SQTH_ADD_AREA ZJZF_ADD_AREA
#define ZJTH_ADD_AREA ZJZF_ADD_AREA
/****通存通兑附加数据声明********/
typedef struct{
	char txamt      [15];	/**交易金额	 **/
	char agentfee   [15];	/**代理行手续费  **/
	char payacttype [1 ];	/**付款人帐号类型**/
	char cashacttype[1 ];	/**收款人帐号类型**/
	char pswdtype   [1 ];	/**密码类型	 **/
	char pswdmethod [2 ];	/**密码验证码算法**/
	char pswdcode   [512];	/**密码验证码	 **/
}TCTD_ADD_AREA;


/****支票截留附加数据声明********/
typedef struct{
	char cpday      [8];	/**出票日期	    ****/
	char cpno       [20];	/**出票号码	    ****/
	char payno      [12];	/**付款行号	    ****/
	char cpactno    [32];	/**出票人帐号	    ****/
	char cashactno  [32];	/**收款人帐号	    ****/
	char amt        [15];	/**金额		    ****/
	char use        [60];	/**用途		    ****/
	char bscnt      [2];	/**背书人数,固定为0 ****/
	char bschecktype[2];	/**校验模式 	    ****/
	char bscheckpswd[512];/**校验密码 	    ****/
	char bsbitmaplen[8];  /**图像数据长度,为0 ****/
}ZPJL_ADD_AREA;


/*附加域*/
typedef struct{
    /*通用信息业务，目前没有固定的附加数据域格式*/
    char    body;
}TYXXYW_ADD;

/*代收/付业务*/
typedef struct{
    char    unit     [60]; /*收付单位*/
    char    belongcyc[16]; /*所属周期*/
    char    sftype   [2];  /*收付种类*/
}SFYW_ADD;


/**支票圈存申请业务**/
typedef struct{
	char cpday      [8 ];  /**出票日期	   **/
	char cpno       [12];  /**支票号码	   **/
	char payno      [12];  /**付款行行号	   **/
	char cpactno    [32];  /**出票人账号	   **/
	char cashactno  [32];  /**收款人账号	   **/
	char amt        [15];  /**金额	           **/
	char use        [60];  /**用途	           **/
	char bscnt      [2 ];  /**背书人数	   **/
	char bschecktype[2 ];  /**校验模式	   **/
	char bscheckpswd[512]; /**校验密码	   **/      
	char bsbitmaplen[8 ];  /**票据图像数据长度 **/
}QCSQ_ADD_AREA;


/**支票圈存回应**/
typedef struct{
	char hytype [2 ];  /**回应标识  **/
	char cpday  [8 ];  /**出票日期  **/
	char zpactno[12];  /**支票号码  **/
	char payno  [12];  /**付款行行号**/
	char cpactno[32];  /**出票人账号**/
	char amt    [15];  /**金额      **/
}QCYD_ADD_AREA;


/*客户帐户查询*/
typedef struct{
    char    acttype     ;  /*帐号类型*/
    char    actno  [32] ;  /*帐号*/
    char    pswdtp [2] ;  /*密码类型*/
    char    pswdlen[8]   ;  /*密码长度*/
    char    passwd [8];  /*密码*/
    char    qtype       ;  /*查询类型*/
    char    reason [60] ;  /*附言*/
}ZHCX_ADD;

/*客户帐户查询* 密码在附加域里面插入的是16位的 add by wangyongwei20070920*/
typedef struct{
    char    acttype     ;  /*帐号类型*/
    char    actno  [32] ;  /*帐号*/
    char    pswdtp [2] ;  /*密码类型*/
    char    pswdlen[8]   ;  /*密码长度*/
    char    passwd [16];  /*密码*/
    char    qtype       ;  /*查询类型*/
    char    reason [60] ;  /*附言*/
}ZHCX_ADD_PWD16;


/*客户帐户查询应答*/
typedef struct{
    /*char    oqtype       ; 原查询类型*/
    /*char    addlength[8] ; 附加数据长度*/
    /*char    actno    [32]; 帐号*/
    /*char    name     [60]; 户名*/
    /*附加数据，变长*/
    
    /***change by liuyue 20060919******/
    char opackday	[8];	/*原批量包日期*/
    char opackid	[8];	/*原批量包序号*/
    char owtday		[8];	/*原委托日期  */
    char oorderno	[8];	/*原信息序号  */
    char actno		[32];	/*帐号	      */
    char qrytype	    ;	/*原查询类型  */
    char resptype	    ;	/*应答标识    */
    char otxamt		[18];	/*当前余额    */
    char actStat	[2] ;	/*当前帐户状态*/
    char content	[60] ;	/*当前帐户状态*/
}ZHCXYD_ADD;

/**证书查询申请**/
typedef struct{
	char content[60];  		/**附言    	**/
	char jslen[8];   		/**证书长度	**/
	/****NEWYX2****/
	char jsdaa[64];   		/**证书内容	**/
}ZSSQ_ADD_AREA;

/**证书查询应答**/
typedef struct{
	char result[1 ];  	/**检查结果	**/
	char brf   [60];  	/**备注	    	**/   
}ZSYD_ADD_AREA;

/****缴费附加数据声明********/
typedef struct{
	char paywssrno[20];	/**收费单位流水号**/
	char payterm  [16];	/**所属期间	***/
	char jftype   [1 ];	/**缴费类型	***/
	char filler   [60];	/**收费附言	***/
}JF_ADD_AREA;
/****国库资金国债兑付贷记附加数据声明********/
typedef struct{
	char infono  [8 ];	/**信息流水号       **/
	char amt     [18];	/**明细汇总金额     **/
	char upgkno  [10];	/**上报国库代码     **/
	char downgkno[10];	/**接收国库代码     **/
	char rptday  [8 ];	/**报表日期         **/
	char rptno   [3 ];	/**报表序号         **/
	char dtlcnt  [3 ];	/**明细条数,固定为1 **/
	char orgtype [12];	/**银行大类         **/
	char bjtype  [12];	/**本金代码         **/
	char bjamt   [18];	/**本金金额         **/
	char lxtype  [12];	/**利息代码         **/
	char lxamt   [18];	/**利息金额         **/
}GZDJ_ADD_AREA;
#define GZJJ_ADD_AREA GZDJ_ADD_AREA


/****支票截留附加数据声明********/
typedef struct{
	char cpday[8];		/**出票日期****/
	char cpno[12];		/**出票号码****/
	char payno[12];		/**付款行号****/
	char cpactno[32];	/**出票人帐号**/
	char cashactno[32];	/**收款人帐号**/
	char cpamt[15];		/**金额***/
	char use[60];		/**用途***/
        char tsfkday[8];/**提示付款日期**/ /*add by lwb 20150417 系统升级时缺少这个 */
	/*NEWYX2:没有这个字段char tsfkday[8];**提示付款日期**/
}ZPJL1_ADD_AREA;
typedef struct{
   char cpday[8];/**出票日期****/
   char cptype[3];/**支票类型***/
   char cpno[12];/**出票号码****/
   char payno[12];/**付款行号****/
   char cpactno[32];/**出票人帐号**/
   char cashactno[32];     /**收款人帐号**/
   char cpamt[15];/**金额***/
   char use[60];/**用途***/
   char tsfkday[8];/**提示付款日期**/
}ZPJL11_ADD_AREA;

typedef struct{
 	char bscnt[2];/**背书人数,固定为0 ****/
 	char bslist[10][60];    /**背书人列表**/
}ZPJL2_ADD_AREA;
typedef struct{
 	char bscheckpswd[512];/**校验密码 ****/
 	char bsbitmaplen[8];/**图像数据长度,为0 ****/
 	char bsbitmaplen2[8];/**图像数据长度,为0 ****/
}ZPJL3_ADD_AREA;
/****通用截留附加数据声明 HSYX BEGIN ********/
typedef struct{
        char cpday[8];          /**出票日期****/
        char cpno[20];          /**出票号码****/
        char payno[12];         /**付款行号****/
        char dlpayno[12];       /**代理付款行号****/
        char cpamt[15];         /**出票金额****/
        char use[60];           /**用途***/
}TYJL1_ADD_AREA;
#define TYJL2_ADD_AREA ZPJL2_ADD_AREA
typedef struct{
        /***char bschecktype[2];校验密码 ****/
        char bscheckpswd[512];/**校验密码 ****/
}TYJL3_ADD_AREA;
typedef struct{
        char ptype[2];          /**票据种类**/
        char tsday[8];          /**提示付款日期**/
        char mac[20];           /**密押**/
        char stopday[8];        /**到期日**/
        char xyno[20];          /**承兑协议编号**/
        char htno[20];          /**交易合同号码**/
        char cdday[8];          /**承兑日期**/
        char cdname[60];        /**承兑人**/
        char reqname[60];       /**申请人**/
        char reqactno[32];      /**申请人帐号**/
        char paybrname[60];     /**付款人开户银行**/
        char cashbrname[60];    /**收款人开户银行**/
        char cpname[60];        /**出票人全称    **/
        char cpactno[32];       /**出票人帐号    **/
        char paybrname2[60];    /**付款行名称    **/
        char content[60];       /**附言**/
}TYJL4_ADD_AREA;
typedef struct{
        char bsbitmaplen[8];/**图像数据长度,为0 ****/
        char bsbitmaplen2[8];/**图像数据长度,为0 ****/
}TYJL5_ADD_AREA;
/****通用截留附加数据声明 HSYX END ********/
/**用来接收通用截留附加域文件内容的结构体 wangyongwei***/
typedef struct{
	   char cpday[8];          /**出票日期****/
       char cpno[20];          /**出票号码****/
       char payno[12];         /**付款行号****/
       char dlpayno[12];       /**代理付款行号****/
       char cpamt[15];         /**出票金额****/
       char use[60];           /**用途***/
	   char ptype[2];          /**票据种类**/
       char tsday[8];          /**提示付款日期**/
       char mac[20];           /**密押**/
       char stopday[8];        /**到期日**/
       char xyno[20];          /**承兑协议编号**/
       char htno[20];          /**交易合同号码**/
       char cdday[8];          /**承兑日期**/
       char cdname[60];        /**承兑人**/
       char reqname[60];       /**申请人**/
       char reqactno[32];      /**申请人帐号**/
       char paybrname[60];     /**付款人开户银行**/
       char cashbrname[60];    /**收款人开户银行**/
       char cpname[60];        /**出票人全称    **/
       char cpactno[32];       /**出票人帐号    **/
       char paybrname2[60];    /**付款行名称    **/
       char content[60];       /**附言**/
}TYJL6_ADD_AREA;
/*新客户帐户查询应答 NEWYX 20070404*/
typedef struct{
    char opackday	[8];	/*原批量包日期*/
    char opackid	[8];	/*原批量包序号*/
    char owtday		[8];	/*原委托日期  */
    char oorderno	[8];	/*原信息序号  */
    char actno		[32];	/*帐号	      */
    char qrytype	    ;	/*原查询类型  */
    char resptype	    ;	/*应答标识    */
    char curcd      [3];    /* 币种 */
    char otxamt		[15];	/*当前余额    */
    char actStat	[2]    ;	/*当前帐户状态*/
    char content    [60];/* 附言 */
}NEW_ZHCXYD_ADD;

typedef struct{
	char	actype[1];		/**收款帐号类型**/
	char	pactype[1];		/**付款帐号类型**/
	char	ct_ind[1];	/**现金转帐标志**/
}PAY_IN_CXTC_AREA;			/**通存通兑**/
/****最新储蓄通存附加数据声明********/
typedef struct{
	char payacttype [1 ];	/**付款人帐号类型**/
	char cashacttype[1 ];	/**收款人帐号类型**/
	char ct_ind   [1 ];	/**通存业务模式	 **/
}CXTC_ADD_AREA;

/****新通兑附加数据声明********/
typedef struct{
	char payacttype [1 ];	/**付款人帐号类型**/
	char cashacttype[1 ];	/**收款人帐号类型**/
	char ct_ind     [1 ];   /* 通兑业务模式 */
	char pswdmethod [2 ];	/**密码验证码算法**/
	char pswdlength [8 ];	/**密码类型	 **/
	char pswdcode   [512];	/**密码验证码	 **/
}TD_ADD_AREA;

/* end END NEWYX by Liuhuafeng 20070404 */
/***END 通用截留附加域接收结构体 *****/

/**************附加域结构声明结束************/

#endif
