drop  table ass_mst;
create table ass_mst(
	br_no varchar2(5),
	pact_no number(9),
	sub_br_no varchar2(5),
	sign varchar2(1),
	bal number(16,2),
	ys_bal number(16,2),
	ic_date number(9),
	end_date number(9),
	cls_date number(9),
	lst_date number(9),
	rate number(8,5),
	over_rate number(8,5),
	intst number(16,2),
	intst_acm number(16,2),
	hst_cnt number(9),
	brf varchar2(60),
	sts varchar2(1) 
)tablespace users;
