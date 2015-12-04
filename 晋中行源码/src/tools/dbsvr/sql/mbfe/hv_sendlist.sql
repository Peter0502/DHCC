drop index hv_sendlist_idx;
drop  table hv_sendlist;
create table hv_sendlist(
	br_no varchar2(7),       /*      机构号 */                      
	tel varchar2(6),         /*  操作员号*/                         
	trace_no number(9),      /*      流水号 */                      
	ref_no varchar2(20),     /*  报文参考号*/                       
	cmtno varchar2(3),       /*cmt号*/                              
	cmt_title varchar2(30),  /*交易描述*/                           
	orderno varchar2(8),     /*交易序号*/                           
	or_br_no varchar2(12),   /*发起行行号*/                    
	ac_br_no varchar2(12),   /*接收行行号*/                    
	pay_ac_no varchar2(32),  /*付款人账号*/                    
	pay_name varchar2(60),   /*付款人名称*/                    
	cash_ac_no varchar2(32), /*收款人账号*/                    
	cash_name varchar2(60),  /*收款人名称*/                    
	tx_amt number(15,2),     /*交易金额*/                      
	resp_flg varchar2(1),    /*是否收到CMT910报文0-没有,1-有*/ 
	resp_no varchar2(8),     /*应答码*/                        
	resp_title varchar2(60), /*响应描述*/                      
	brf varchar2(30)         
)tablespace users;                                       
create	 unique index hv_sendlist_idx on hv_sendlist( ref_no)tablespace indx;
