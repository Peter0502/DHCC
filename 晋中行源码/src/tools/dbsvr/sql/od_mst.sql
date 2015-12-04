drop index od_mst_1;
drop  table od_mst;
create table od_mst(
	ac_id number(9),
	ac_seqn number(6),
	opn_br_no varchar2(5),
	prdt_no varchar2(3),
	bal number(16,2),
	drt_bal number(16,2),
	cur_bal number(16,2),
	intst_amt number(16,2),
	intst_bal number(16,2),
	drt_intst_bal number(16,2),
	cash_bal number(16,2),
	ys_bal number(16,2),
	hst_bal number(16,2),
	intst_acm number(19,2),
	drt_intst_acm number(19,0),
	max_bal number(16,2),
	cash_max_bal number(16,2),
	min_pay_amt number(16,2),
	min_pay_ratio number(8,5),
	lst_date number(9),
	hst_cnt number(9),
	hst_pg number(9),
	ac_sts varchar2(1),
	pay_ac_id number(9),
	pay_ac_seqn number(6),
	mac varchar2(16) 
)tablespace users;
create	 unique index od_mst_1 on od_mst( ac_id,ac_seqn)tablespace indx;
