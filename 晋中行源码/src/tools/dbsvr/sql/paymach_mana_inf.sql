drop  table paymach_mana_inf;
create table paymach_mana_inf(
	ac_no varchar2(32),
	id_no varchar2(11),
	pr_date number(9),
	oper_no varchar2(6),
	flag varchar2(2) 
)tablespace users;
