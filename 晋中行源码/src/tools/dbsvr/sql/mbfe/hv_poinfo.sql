drop index hv_poinfo_idx;
drop  table hv_poinfo;
create table hv_poinfo(
	po_no      varchar2(20),   /* 汇票号码 */  
	br_no      varchar2(7),    /* 机构号码 */ 
	bank_type  varchar2(1),    /* 行别标志:0-本行汇票 1-他行汇票*/  
	or_br_no   varchar2(12),   /* 发起行行号 */ 
	pay_qs_no  varchar2(12),   /* 支付付款清算行号*/ 
	l_tx_date  number(9),      /* 上次交易日期 */
	td_cnt     number(9),      /* 当日交易笔数 */
	lost_cnt   number(9),      /* 挂失次数每挂失，借挂失增加一 */
	note_type  varchar2(3),    /* 凭证种类 */
	note_no    varchar2(16),   /* 凭证号 */ 
	o_note_type varchar2(3),  /* 原始凭证种类*/
	o_note_no varchar2(16),   /* 原始凭证号*/
	po_bal number(15,2),      /* 金额(出票金额／票面金额)*/
	cash_amt number(15,2),    /* 实际结算金额/兑付金额 */
	fee_amt number(15,2),     /* 手续费金额 */
	rem_amt number(15,2),     /* 多余金额/退回金额 */
	cur_id varchar2(3),       /* 币种 */
	pay_br_no varchar2(12),   /* 签发行行号/付款人开户行号 */
	pay_ac_no varchar2(32),   /* 付款人帐号*/
	pay_name varchar2(60),    /* 付款人户名*/
	pay_br_name varchar2(60), /* 签发行行名*/
	pay_addr varchar2(60),    /* 付款人地址*/
	cash_ac_no varchar2(32),  /* 收款人帐号*/
	cash_br_no varchar2(12),  /* 兑付行行号*/
	cash_br_name varchar2(60),/* 兑付行行名*/
	cash_addr varchar2(60),   /* 兑付行地址*/
	cash_name varchar2(60),   /* 收款人户名*/
	hold_ac_no varchar2(32),  /* 当前持票人帐号 */
	hold_name varchar2(60),   /* 当前持票人名字 */
	l_po_sts varchar2(1),     /* 上次帐户状态 */
	po_sts varchar2(1),       /* 汇票当前状态标志 0-登记 1-签发 9-签发取消 2-汇票移存 3-申请兑付 4-汇票兑付 5-全额结清 6-部分退回 7-未用退回 8-汇票解付 A-解付取消 B-发送 C-申请兑付取消 D-汇票登记取消 E-挂失 F-解挂失 G-逾期未用退回 H-未用解??I-未用冲帐冲帐 */ 
	reg_date number(9),       /* 登记日期*/
	sign_date number(9),      /* 签发日期*/
	pay_date number(9),       /* 提示最后付款日期*/
	pwd varchar2(10),         /* 密押    */
	po_ind varchar2(1),       /* 汇票类别0-可转让汇票1-不可转让汇票2-现金汇票 */
	deal_mode varchar2(1),    /* 转帐现金处理方式 0-现金 1 转账(帐号) 2科目对转*/
	drsubject varchar2(7),    /* 付款科目 */
	crsubject varchar2(7),    /* 资金去向 */
	content varchar2(60),     /* 备注*/
	tx_chrg_type varchar2(1), /* 手续费标志 0.现金 1.现金加急 2.转帐 3.转帐加急 4.不收*/
	checkflag varchar2(1),    /* 对帐检查标志*/
	op_tel varchar2(6),       /* 开户操作员 */
	chk_tel varchar2(6),      /* 复合操作员 */
	pass_tel varchar2(6),     /* 密押操作员 */
	add_pwd_ind varchar2(1),  /* 是否加/核押处理 1-加押/核押 0-未处理 */
	brf varchar2(60)
)tablespace users;
create	 unique index hv_poinfo_idx on hv_poinfo( po_no,pay_br_no)tablespace indx;
