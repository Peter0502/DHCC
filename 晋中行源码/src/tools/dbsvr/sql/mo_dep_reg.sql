/**** ���гжһ�Ʊ��Ѻ���Ǽǲ� **/
drop  table mo_dep_reg;
create table mo_dep_reg(
	pact_no varchar2(20),	/** ��Ѻ��Ʊ��� **/
	po_no varchar2(16),	/** ��Ʊ��� **/
	po_amt number(16,2) 	/** Ʊ���� **/
)tablespace users;
