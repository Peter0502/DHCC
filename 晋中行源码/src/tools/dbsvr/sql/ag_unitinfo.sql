drop  table ag_unitinfo;
create table ag_unitinfo(
	unit_no varchar2(4),
	unit_name varchar2(50),
	opr_attri varchar2(1),
	pay_type varchar2(2),
	bank_acct_no varchar2(25),
	beg_date number(9),
	end_date number(9),
	clr_flag varchar2(1),
	clr_date number(9),
	up_unit varchar2(4),
	auto_flag varchar2(1),
	tx_date number(9),
	opn_date number(9),
	opn_tel varchar2(4),
	opn_br_no varchar2(5),
	unit_stat varchar2(1) 
)tablespace users;
