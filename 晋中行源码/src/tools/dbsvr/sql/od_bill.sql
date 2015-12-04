drop index od_bill_1;
drop  table od_bill;
create table od_bill(
	ac_id number(9),
	ac_seqn number(6),
	curr_cnt number(9),
	amt number(16,2),
	bal number(16,2),
	drt_bal number(16,2),
	intst_acm number(19,2),
	drt_intst_acm number(19,2),
	intst_amt number(16,2),
	intst_bal number(16,2),
	drt_intst_bal number(16,2),
	beg_date number(9),
	end_date number(9),
	pay_date number(9),
	tx_date number(9),
	lst_date number(9),
	pay_sts varchar2(1),
	curr_sts varchar2(1) 
)tablespace users;
create	 unique index od_bill_1 on od_bill( ac_id,ac_seqn,curr_cnt)tablespace indx;
