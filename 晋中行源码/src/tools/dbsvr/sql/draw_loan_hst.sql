create table draw_loan_hst(
	st_flag		varchar2(1),  /**支付标志 1:自主支付 2:委托支付  **/
	pact_no		varchar2(20), /**贷款协议号        **/
	draw_no		varchar2(24), /**授权支付协议号    **/
	ac_no		varchar2(20), /**贷款对应的存款账号**/
	ac_id		number(9),    /**存款账户id        **/
	draw_amt	decimal(16,2),/**支取金额          **/
	approve_tel	varchar2(6),  /**审批柜员          **/
	approve_date	number(9),    /**审批日期          **/
	sts		varchar2(1),  /**状态 1.待支取2.已支取 **/
	tel		varchar2(6),  /**交易柜员          **/
	trace_no	number(9),    /**系统流水          **/
	tx_date		number(9),    /**交易日期          **/
	ejfno		number(9),    /**柜员流水          **/
	trust_ac_no	varchar2(19), /**委托收款账号      **/
	trust_name	varchar2(60), /**委托收款户名      **/
	opn_br_name	varchar2(60), /**委托户开户行名称  **/
	brf1		varchar2(60), /**备注1             **/
	brf2		varchar2(60)  /**备注2             **/
)tablespace users;
create unique index draw_loan_hst_idx on draw_loan_hst (draw_no) tablespace indx;
