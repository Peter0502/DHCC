drop index hv_chkerror_idx;
drop  table hv_chkerror;
create table hv_chkerror(
	br_no varchar2(7),         /*机构号*/                                   
	cmtno varchar2(3),         /*报文编号*/                                 
	orderno varchar2(8),       /*交易序号(dhcc build)*/                     
	pay_qs_no varchar2(12),    /*发起清算行行号*/                           
	or_br_no varchar2(12),     /*发起行行行号*/                             
	tx_date number(9),         /*交易日期*/                                 
	tx_time number(9),         /*交易时间*/                                 
	tx_amt number(15,2),       /* 处理的金额 */                             
	tx_amt_ccb number(15,2),   /*城市商业银行交易金额City Credit Bank(CCB)*/
	tx_amt_ccpc number(15,2),  /*ccpc交易金额*/                             
	o_sts varchar2(1),         /* 原始状态 */                               
	sts varchar2(1),           /* 状态*/                                    
	chk_flg varchar2(1),       /* 对帐状态 */                               
	title varchar2(30),        /* 描述 */                                   
	prt_flag varchar2(1),      /*打印标志，0未打印，1已打印*/               
	brf varchar2(60)           /*备用字段*/                                 
)tablespace users;           
create	 unique index hv_chkerror_idx on hv_chkerror( orderno,or_br_no,tx_date)tablespace indx;
