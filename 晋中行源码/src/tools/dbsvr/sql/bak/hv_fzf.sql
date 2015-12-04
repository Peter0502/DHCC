drop index hv_fzf_idx;
drop  table hv_fzf;
create table hv_fzf(
	otype       varchar2(1),   /* 1-大额，2－小额 */
	or_br_no    varchar2(12),  /* 发起行行号 */
	orderno     varchar2(8),   /* 非支付交易序号*/
	tx_date     number(9),     /* 交易日期 */
	wt_date     number(9),     /* 委托日期 */
	br_no       varchar2(7),   /* 交易行行号*/
	hv_fzf_sts  varchar2(1),   /* 非支付交易状态*/
	cmtno       varchar2(3),   /* 大额cmtno,小额pkgno */
	tx_time     number(9),     /* 交易时间*/
	pay_qs_no   varchar2(12),  /* 发起清算行行号(帐号) accountno */
	ac_br_no    varchar2(12),  /* 接收行行号 */
	cash_qs_no  varchar2(12),  /* 接收行清算帐号 */
	o_wt_date   number(9),     /* 原委托日期--支付交易的委托日期 */
	o_tx_date   number(9),     /* 原交易日期--支付交易的委托日期 */
	o_or_br_no  varchar2(12),  /* 原(支付交易)发起行行号*/
	o_ac_br_no  varchar2(12),  /* 原(支付交易)接收行行号*/
	o_orderno   varchar2(8),   /* 原支付交易序号*/
	o_cur_ind   varchar2(3),   /* 原币种*/
	o_tx_amt    number(15,2),  /* 原交易金额 */
	o_cmtno     varchar2(3),   /* 原cmtno 大额cmtno,小额pkgno */
	o_tx_type   varchar2(1),   /* 原交易种类*/
	sendco      varchar2(4),   /* 发报中心代码 */
	receco      varchar2(4),   /* 收报中心代码 */
	req_date    number(9),     /* 申请日期*/
	req_br_no   varchar2(12),  /* 申请行*/
	req_content varchar2(255), /* 申请内容*/
	req_orderno varchar2(8),   /* 申请序号 */
	res_date    number(9),     /* 应答日期*/
	res_br_no   varchar2(12),  /* 应答行*/
	res_content varchar2(255), /* 应答内容*/
	res_orderno varchar2(8),   /* 应答序号 */
	tx_sts      varchar2(1),   /* 该交易的状态,录入,复核,发送,轧差*/
	res_sts     varchar2(1),   /* 应答码0-撤销 1-不能撤销/0-已退回 1-不能退回
				小额表示为1=允许 2=不允许*/
	addid       varchar2(8),   /* 附加域*/
	refno       varchar2(20),  /* 报文参考号 */
	lw_ind      varchar2(1),   /* 来往标识 */
	hv_prt_ind  varchar2(10),  /* 打印标志，0未打印，1已打印*/
	hv_brf      varchar2(60),  /* 备注 */
	tel         varchar2(6),   /* 操作员 */
	trace_no    number(9),     /* 流水号*/
	send_tel    varchar2(6),   /* 发送操作员 */
	send_trace_no  number(9) , /* 发送流水号 */
	reqtype    varchar2(1),    /* 申请类型 0-单笔 1-整包 */
	o_pack_date   number(9),   /* 原包日期 */
	o_packid   varchar2(8),   /* 原包序号 */
	resp1      varchar2(8),   /* ccpc应答码 */
	otxnum     varchar2(5),   /* 原业务类型 */
	filler     varchar2(50)
)tablespace users;
create	 unique index hv_fzf_idx on hv_fzf( otype,or_br_no,wt_date,orderno,cmtno)tablespace indx;
