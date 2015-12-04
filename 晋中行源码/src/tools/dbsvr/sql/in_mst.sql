drop index in_mst_1;
drop  table in_mst;
create table in_mst(
	opn_br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	prdt_no varchar2(3),
	bal number(16,2),
	ys_bal number(16,2),
	hst_bal number(16,2),
	sts varchar2(1),
	opn_date number(9),
	ic_date number(9),
	end_date number(9),
	cls_date number(9),
	lst_date number(9),
	intst_knd varchar2(1),
	rate number(8,5),
	over_rate number(8,5),
	intst_acm number(19,2),
	hst_cnt number(9),
	hst_pg number(9),
	hold_amt number(16,2),
	od_ind varchar2(1),
	name varchar2(50),
	cal_code varchar2(10),
	mac varchar2(16) 
)tablespace users;
create	 unique index in_mst_1 on in_mst( ac_id,ac_seqn)tablespace indx;
