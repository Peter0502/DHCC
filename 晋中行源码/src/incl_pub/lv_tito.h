#ifndef INCLUDE_LV_TITO
#define INCLUDE_LV_TITO
/**下面定义tis包的可变部分**/
struct {/**借记业务*/
	char Respday[8];/*期待应答日期，往账不用*/
	char Ptype[2];	/**票据类型**/
	char Pdate[8];	/**票据日期**/
	char Pnum[8];	/**票据号码**/
	char Ptxamt[15];/**赔偿金额**/
	char Jtxamt[15];/**拒付金额**/
	char Owtday[8];	/**原委托日期**/
	char Otxnum[8];	/**原业务编号**/
	char Oorderno[8];	/**原支付交易序号**/
	char Thretcode[2];	/**退汇原因代码**/
	char Rcpstat[2];	/**回执状态**/
	char Resp1[8];		/**mbfe应答**/
	char Resp2[8];		/*ccpc应答**/
}PAY_IN_DR_AREA;
/****Modified by SSH,2005.7.28,使用相同的tis结构***/
#define PAY_IN_WT_AREA PAY_IN_DR_AREA 
#define PAY_IN_TSCF_AREA PAY_IN_DR_AREA 
#define PAY_IN_DJTH_AREA PAY_IN_DR_AREA 
#define PAY_IN_RECEIPT_AREA PAY_IN_DR_AREA 
#define PAY_IN_RESP_AREA PAY_IN_DR_AREA 
typedef struct{
	char	actype[1];		/**收款帐号类型**/
	char	pactype[1];		/**付款帐号类型**/
	char	pswdtype[1];		/**密码类型**/
	char	pswd[8];		/**密码**/
}PAY_IN_TCTD_AREA;			/**通存通兑**/
typedef struct{
	char	payunit[60];	/**付费(税)单位**/
	char	payterm[16];	/**付费(税)期间**/
	char	paytype[2];		/**付费(税)种类**/
}PAY_IN_FEE_AREA;/**交费、缴税**/



	
	





#endif	/**end INCLUDE_LV_TITO**/
