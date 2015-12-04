drop index hv_zf_idx;
drop  table hv_zf;
create table hv_zf(
	br_no          varchar2(7),   /*交易行行号*/
	tx_date        number(9),     /*交易日期*/
	wt_date        number(9),     /*委托日期*/
	tx_time        number(9),     /*交易时间*/
	orderno        varchar2(8),   /*人行的交易序号*/
	cur_no         varchar2(3),   /*货币符号*/ 
	note_type      varchar2(3),   /*凭证类型 */
	note_no        varchar2(16),  /*凭证号 */  
	tx_amt         number(15,2),  /*交易金额 */
	tx_chrg_ind    varchar2(1),   /*手续费标志 0.现金 1.现金加急 2.转帐 3.转帐加急 4.不收*/
	chrg_amt       number(15,2),  /*手续费 */
	pay_qs_no      varchar2(12),  /*发起清算行行号(帐号) accountno */
	or_br_no       varchar2(12),  /*发起行行号--退汇交易的原接收行 */
	pay_opn_br_no  varchar2(12),  /*付款人开户行行号 */
	pay_ac_no      varchar2(32),  /*付款人帐号 */      
	pay_name       varchar2(60),  /*付款人姓名--退汇交易的原收款人户名 */
	pay_addr       varchar2(60),  /*付款人地址 */    
	cash_qs_no     varchar2(12),  /*接收行清算行号 */
	ac_br_no       varchar2(12),  /*接收行行号--退汇交易的原发起行 */
	cash_opn_br_no varchar2(12),  /*收款人开户行行号 */
	cash_ac_no     varchar2(32),  /*收款人帐号 */      
	cash_name      varchar2(60),  /*收款人姓名--退汇交易的原付款人户名 */
	cash_addr      varchar2(60),  /*收款人地址 */  
	yw_type        varchar2(2),   /*业务种类 */    
	sendco         varchar2(4),   /*发报中心代码 */
	receco         varchar2(4),   /*收报中心代码 */
	cmtno          varchar2(3),   /*CMT编号 */     
	beg_sts        varchar2(1),   /*存放stat原始状态用于时候监督用 */
	operlevel      varchar2(1),   /*优先级别0-一般1-紧急2-特急(救灾战备款等支付)*/
	hv_sts         varchar2(1),   /*状态 请参阅hv_define.h程序 */
	tel            varchar2(6),   /*操作员员 */
	trace_no       number(9),     /*流水号*/   
	hv_osts        varchar2(1),   /*退汇交易用到的原支付来帐交易状态 */
	checkflag      varchar2(1),   /*对帐检查标志*/
	hv_prt_ind     varchar2(10),  /*打印标志，0未打印，1已打印*/
	resend_ind     varchar2(1),   /*拒绝重发标志0未重新发送，1表示已经重新发送旧数据,2，表示手工退回支行，M表示重新发送的新数据 */
	resend_date    number(9),     /*拒绝重新发送日期 */
	o_orderno      varchar2(8),   /*原支付交易序号--退汇交易用的 */
	o_tx_date      number(9),     /*原支付交易日期--退汇交易用的 */
	o_wt_date      number(9),     /*原支付交易委托日期--退汇交易用的 */ 
	o_cmtno        varchar2(3),   /*原cmtno--退汇交易用的 */
	lw_ind         varchar2(1),   /*来往标识 1-往 2-来 */
	addid          varchar2(8),   /*附加域 id --addit表*/
	refno          varchar2(20),  /*报文参考号 */
	auth_tel       varchar2(6),   /*授权操作员*/
	auth_trace_no  number(9),     /*授权操作员流水号*/      
	chk            varchar2(6),   /*复核操作员*/
	chk_trace_no   number(9),     /*复核操作员流水号*/      
	send_tel       varchar2(6),   /*发送操作员*/
	send_tel_trace_no number(9),  /*发送操作员流水号*/
	dealerr_date      number(9),  /*错帐处理日期*/
	dealerr_tel       varchar2(6),/*错帐处理柜员*/
	resp1             varchar2(8),/*应答码*/
	hv_brf            varchar2(60)/*备注 */ 
)tablespace users;
create	 unique index hv_zf_idx on hv_zf( or_br_no,wt_date,orderno,cmtno)tablespace indx;
