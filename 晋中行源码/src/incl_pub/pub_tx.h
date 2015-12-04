#ifndef PUB_TX
#define PUB_TX
struct pub_tx		/*通用记帐结构*/
{
	long	trace_no;		/*流水号*/
	long	trace_cnt;		/*流水笔次*/
	long	tx_date;		/*交易日期*/
	long	tx_time;		/*交易时间*/
	char	tx_code[5];		/*交易代码*/
	char	sub_tx_code[5];	/*子交易代码*/
	char	tx_br_no[6];	/*交易机构*/
	char	opn_br_no[6];	/*开户机构号码*/
	char	tty[5];		   	/*前台终端号*/
	char  file_snd[2];    /*批量传送标志*/
	long	old_trace_no;	/*被冲正流水号*/
	char	id_type[3];		/*证件类型*/
	char	id_no[21];		/*证件号码*/
	long	cif_no;			/*客户号*/
	char	cur_no[3];		/*币种*/
	char	crd_no[25];		/*卡号*/
	char	ac_no[25];		/*账号*/
	char	ac_no1[25];		/*账号1*/
	long	ac_id;			/*账户ID*/
	int		ac_seqn;		/*账户序号*/
	char	name[61];		/*户名*/
	char	cif_type[2];      	/*客户类型: 0：公用；1：个人；2：公司；
								3：机构（指政府机关等组织）；
								4：金融同业；5：内部员工；6金融机构；*/
	char	ac_type[2];	        /*账户类型:1-基本户 2-一般户 3-专业户；
								4-临时账户；D-结算户；S-非结算户*/
	char	ac_id_type[2];      /*用于区分哪个主表
						1-存款 2-定期 3-贷款 4-透支 9-内部 其他-不定*/
	char	ac_get_ind[11];      /*第一位1-已读mdm_ac_rel 其他-未读
								  第二位1-已读取相应分户 其他-未读
								  第三位1-已读取相应参数表 其他-未读
								要求此时必须明确ac_id_type标志*/
	char	intst_type[2];		/*计息类型 0不计息1清理积数*/
	char	mdm_code[5];		/*介质编码*/
	char	prdt_code[4];		/*产品代码*/
	char	note_type[4];		/*凭证种类*/
	char	beg_note_no[17];	/*凭证起号*/
	char	end_note_no[17];	/*凭证止号*/
	char	new_note_no[17];	/*新凭证号*/
	char	using_ind[2];		/**使用0190结构的标志0否1是*/
	char  ac_wrk_ind[11];		/*第一位 是否核对介质号码0否1是 9开户不用介质*
								  第二位 零金额是否记流水0否1是2强制不记流水
								  第三位 零金额是否记明细0否1是
								  第四位 是否核对支取方式0否1是
								  第五位 强制零金额开户0否1是
								  第六位 开户检查客户号是否存在0否1是
								  第七位 是否允许他行账号0否1是
								  第八位 自动帐务处理标志0非J活期结息
								  第九位 日终记账0否1是
								  第十位 日终是否强行扣款0否1是（对于非结算户）*/
	char	add_ind[2];		/*增减(借贷):0减(借) 1加(贷)*/
	int		svc_ind;		/*
							1000开户1001存取款1002开账号1003销户1004协定销户
							1005存款结息入帐1006预授权1007预授权完成
							1008协定户存取款1009公积金户取款1010公积金户销户
							1011部提（系统自动将取款时的次标志改为部提）
							1012销户或部提计息，记息
							1201子帐号存取款1202子帐号销户
							3001贷款开户 3010贷款本金存取 
							3011贷款本金结息(表内)3012 贷款本金结息(表外)
							3019 日终还款计划还本 3018 按揭还款计划变更
							3020 贷款表内利息存取 3021 贷款表内利息结息
							3030 贷款表外利息存取 3031 贷款表外利息结息 
							3040 贷款复利利息存取 3041 贷款复利利息结息 
							3050 贷款利息存取     3051 贷款利息结息（相当前3的和）
							3052 日终贷款结息
							3059 日终还款计划还息 
							3060 贷款五级认定
							3061 抵押品注销
							3062 生成按揭还款计划
							3063 贷款结转--转出
							3065 贷款展期 
							3068 按揭还利息	      3069按揭还罚息
							3071 还老系统按揭贷款利息 3072 还老系统按揭贷款罚息
							3091 冲补贷款本金     3092 冲补抵押品金额
							9000内部帐开户9001内部帐存取9002内部帐销户
							9003内部帐计息
							7021 日常交易中柜员凭证使用
							*/
	char	ct_ind[2];		/*现转:1现2转*/
	double	tx_amt1;		/*发生额:资产负债户及贷款本金户发生额*/
	double	tx_amt2;		/*表内欠息发生额；开户时计划发放额*/
	double	tx_amt3;		/*表外欠息发生额*/
	double	tx_amt4;		/*表外复利欠息发生额*/
	char	prt_ind[2];		/*是否打印存折(有折无折标志):1是0否*/
	char	hst_ind[2];		/*入明细帐标志:0不入1日间入2日终单笔入3日终汇总入*/
	char	draw_uncon_yn[2];	
	char	qry_pwd_yn[2];	/*是否有查询密码*/
	char	qry_pwd[7];
	char	draw_pwd_yn[2];
	char	draw_pwd[7];
	char	draw_id_yn[2];
	char	draw_seal_yn[2];
	char	pwd_mach_yn[2];
	char	draw_mach[17];
	long	erro_date;		/**错帐日期**/
	long	mach_date;		/**到期日期**/
	long	open_date;		/**开户日期**/
	char	brf[21];		/*摘要*/
	double	rate;			/*正常利率*/
	double  over_rate;		/*逾期利率*/
	char	reply[5];		/*响应码*/
	char	tel[5];			/*操作员*/
	char	chk[5];			/*复核员*/
	char	auth[5];		/*授权人*/
	long    auth_code;          	/*授权码*/
	char    dc_code[5];         	/*会计代码*/
	char    cal_code[21];         	/*指标体系代码*/
	char    cash_code[11];       	/*现金代码*/
	long	next_intst_date;	/*下次结息日期*/
	long	acm_beg_date;		/*积数起始日期*/
	char	tmp_ind[2];		/*供临时使用的标志位,可随意使用
							  定期开户时存的是否自动转存标志*/
	double	tmp_amt1;		/*供临时使用,可随意使用*/
	double	tmp_amt2;		/*供临时使用,可随意使用*/
	char	no_show[2];		/**隐形流水标志*1隐形；其他否*/
	char	reg_mo[257];	/*登记簿结构(每位表示一个登记簿，在.ec中登记)*/
};
struct S_ln_intst
{
	double 	intst_amt;	/*金额*/
	int		type;	/*类型：
	0本金、1本金利息、2表内欠息、3表内复利、4表外欠息、5表外复利、
	6复利欠息、7复利复利*/
	double	pay_amt;/*归还金额*/
};
struct S_pub_intst
{
	char 	edu_ind[2];	/*教育证明 Y：有 N：无 这里是个旧的错误的说明气死了*/ /*正确的 1:有  其他:无 */
    double  dealval;  	/* 应付利息 */
    double  tax_intst;  /* 应税利息 */
    double  factval;  	/* 实付利息 */
    double  val;      	/* 代扣利息 （利息税）*/
    double  keepval;  	/* 保值利息 */
    int mode;     		/* 活期计息 1--销户计息 2--结息日计息 */
};
#endif
