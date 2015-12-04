drop index ind_404_325;
drop  table tmp_ck;
create table tmp_ck(
	acct_no varchar2(20),
	accu_amt number(14,2),
	flag varchar2(1),
	ac_no varchar2(15),
	lx number(12,2),
	ac_id number(9),
	ac_sdeqn number(9) 
)tablespace users;
create	 unique index ind_404_325 on tmp_ck( acct_no)tablespace indx;
