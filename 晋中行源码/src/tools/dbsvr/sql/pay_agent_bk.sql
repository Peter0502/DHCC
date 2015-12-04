drop  table pay_agent_bk;
create table pay_agent_bk(
	unit_no varchar2(4),
	item_no varchar2(20),
	name varchar2(50),
	id_type varchar2(1),
	id_no varchar2(20),
	ac_no varchar2(19),
	trace_no number(9),
	end_date number(9),
	tx_date number(9),
	tram number(16,2),
	remark varchar2(40),
	ckmk_flag varchar2(1),
	sata varchar2(1),
	result varchar2(1),
	result_name varchar2(40),
	cnt varchar2(2) 
)tablespace users;
