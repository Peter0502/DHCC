drop  table tmp_hold;
create table tmp_hold(
	ac_id number(9),
	ac_seqn number(9),
	acct_no varchar2(20),
	bal number(14,2) 
)tablespace users;
