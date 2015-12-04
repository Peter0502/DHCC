drop index ln_reg_1;
drop  table ln_reg;
create table ln_reg(
	ac_id number(9),
	ac_seqn number(6),
	reg_date_beg number(9),
	reg_date_end number(9),
	pay_date number(9),
	type varchar2(1),
	lo_bal number(16,2),
	acm number(19,2),
	pay_bal number(16,2),
	ind varchar2(1),
	sts varchar2(1) 
)tablespace users;
create	 index ln_reg_1 on ln_reg( ac_id,sts,pay_date)tablespace indx;
