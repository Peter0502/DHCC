drop  table up_dd_mst;
create table up_dd_mst(
	br_no varchar2(5),
	ac_no varchar2(19),
	bal number(16,2),
	ys_bal number(16,2),
	intst_acm number(19,2),
	opn_date number(9),
	ic_date number(9),
	lst_date number(9),
	hst_cnt number(9),
	hst_pg number(9),
	name varchar2(50),
	ac_sts varchar2(1) 
)tablespace users;
