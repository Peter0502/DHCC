create table draw_loan_hst(
	st_flag		varchar2(1),  /**֧����־ 1:����֧�� 2:ί��֧��  **/
	pact_no		varchar2(20), /**����Э���        **/
	draw_no		varchar2(24), /**��Ȩ֧��Э���    **/
	ac_no		varchar2(20), /**�����Ӧ�Ĵ���˺�**/
	ac_id		number(9),    /**����˻�id        **/
	draw_amt	decimal(16,2),/**֧ȡ���          **/
	approve_tel	varchar2(6),  /**������Ա          **/
	approve_date	number(9),    /**��������          **/
	sts		varchar2(1),  /**״̬ 1.��֧ȡ2.��֧ȡ **/
	tel		varchar2(6),  /**���׹�Ա          **/
	trace_no	number(9),    /**ϵͳ��ˮ          **/
	tx_date		number(9),    /**��������          **/
	ejfno		number(9),    /**��Ա��ˮ          **/
	trust_ac_no	varchar2(19), /**ί���տ��˺�      **/
	trust_name	varchar2(60), /**ί���տ��      **/
	opn_br_name	varchar2(60), /**ί�л�����������  **/
	brf1		varchar2(60), /**��ע1             **/
	brf2		varchar2(60)  /**��ע2             **/
)tablespace users;
create unique index draw_loan_hst_idx on draw_loan_hst (draw_no) tablespace indx;
