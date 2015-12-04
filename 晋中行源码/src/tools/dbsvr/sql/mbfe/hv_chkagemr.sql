drop index hv_chkagemr_idx;
drop  table hv_chkagemr;
create table hv_chkagemr(
	orderno varchar2(8),        /**支付交易序号(发送产生，接收已经存在) *key*/               
	pay_qs_no varchar2(12),     /**发起清算行行号(帐号) accountno       *key*/               
	br_no varchar2(7),          /* 交易行行号 */                                             
	tx_date number(9),          /* 交易日期   */                                             
	cmtno varchar2(3),          /* CMT编号    */                                             
	o_cmtno varchar2(3),        /* 退汇支付原来的CMT编号    */                               
	tx_time number(9),          /* 交易时间   */                                             
	cur_no varchar2(3),         /* 货币符号   */                                             
	tx_amt number(15,2),        /* 交易金额   */                                             
	other_amt number(15,2),     /* 多余金额 */                                               
	jf_amt number(15,2),        /* 拒付金额/实际结算金额 */                                  
	or_br_no varchar2(12),      /* 发起行行号 */                                             
	pay_no varchar2(12),        /* 付款人开户行行号 */                                       
	pay_ac_no varchar2(32),     /* 付款人帐号 */                                             
	payname varchar2(60),       /* 付款人姓名 */                                             
	payaddress varchar2(60),    /* 付款人地址 */                                             
	cashqsactno varchar2(12),   /* 接收行清算帐号 */                                         
	acbrno varchar2(12),        /* 接收行行号 */                                             
	cashno varchar2(12),        /* 收款人开户行行号 */                                       
	cash_ac_no varchar2(32),    /* 收款人帐号 */                                             
	cash_name varchar2(60),     /* 收款人姓名 */                                             
	cash_addr varchar2(60),     /* 收款人地址 */                                             
	yw_type varchar2(2),        /* 业务种类   */                                             
	sendco varchar2(4),         /* 发报中心代码 */                                           
	receco varchar2(4),         /* 收报中心代码 */                                           
	operlevel varchar2(1),      /* 优先级0-一般1-紧急2-特急(救灾战备款等支付)*/              
	obrno varchar2(12),          /* 对方机构号 没有实际意义 */                               
	voctype varchar2(2),                                                                     
	vocnum varchar2(12),                                                                     
	p_date number(9),           /* 票据日期 */                                               
	p_num varchar2(8),          /* 票据号 */                                                 
	hp_type varchar2(1),        /* 汇票类别 */                                               
	pass_wd varchar2(10),       /* 汇票密押 */                                               
	inst_rate number(8,5),      /* 拆借利率 */                                               
	term number(9),             /* 拆借天数 */                                               
	o_tx_date varchar2(8),      /* 退汇专用原委托日期 */                                     
	ou_tx_date varchar2(8),     /* 退汇专用原退回申请日期 */                                 
	o_orderno_hv varchar2(8),   /* 退汇专用原支付交易序号 */                                 
	o_tx_type varchar2(1),      /* 原交易总类 */                                             
	p_orderno_hv varchar2(8),   /* 撤销专用--撤销的支付交易序号 */                           
	dc varchar2(1),             /* 借贷标志  */                                              
	pay_date number(9),         /* 提示付款日期 */                                           
	intst number(15,2),         /* 融资利息  */                                              
	reason varchar2(60),        /* reason */                                                 
	repson varchar2(1),         /* repson */                                                 
	chk_flg varchar2(1),        /* 是否核对标志 0-未核对，1-核对 */                          
	brf1 varchar2(30),          /* 备注字段1 */                                              
	brf2 varchar2(30)           /* 备注字段2 */                                              
)tablespace users;        
create	 unique index hv_chkagemr_idx on hv_chkagemr( orderno,or_br_no)tablespace indx;
