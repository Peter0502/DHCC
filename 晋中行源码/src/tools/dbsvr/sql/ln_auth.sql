drop index pact_no;
drop  table ln_auth;
create table ln_auth(
	pact_no varchar2(20),
	name varchar2(60),
	time_lmt number(9),
	amt number(16,2),
	rate number(8,5),
	pay_ac_no varchar2(19),
	pay_ac_id number(9),
	save_ac_no varchar2(19),
	save_ac_id number(9),
	save_mdm_code varchar2(4),
	sts varchar2(1) 
)tablespace users;
create	 unique index pact_no on ln_auth( pact_no)tablespace indx;
