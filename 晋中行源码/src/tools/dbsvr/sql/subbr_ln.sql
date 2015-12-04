drop index subbr_ln_1;
drop  table subbr_ln;
create table subbr_ln(
	up_br_no varchar2(5),
	opn_br_no varchar2(5),
	ex_year number(9),
	ex_month number(9),
	ac_id number(9),
	ac_seqn number(6),
	acc_sts varchar2(1),
	monthb_bal number(16,2),
	bal number(16,2),
	mon_increment number(16,2),
	month_addbal number(16,2),
	season_addbal number(16,2),
	year_addbal number(16,2),
	true_accrual number(16,2),
	accrual_tadd number(16,2),
	jaccrual number(16,2),
	client_mgr varchar2(4) 
)tablespace users;
create	 unique index subbr_ln_1 on subbr_ln( ac_id,ac_seqn)tablespace indx;
