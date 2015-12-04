drop index intst_idx1;
drop  table intst_hst;
create table intst_hst(
	br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	prdt_no varchar2(3),
	cur_no varchar2(2),
	bic_bal number(16,2),
	bic_acm number(16,2),
	fct_intst number(12,2),
	intst_tax number(12,2),
	aic_bal number(16,2),
	aic_acm number(16,2),
	ic_date number(9),
	intst_date number(9),
	ac_ind varchar2(1),
	pt_ind varchar2(1),
	rate number(8,2) 
)tablespace users;
create	 index intst_idx1 on intst_hst( br_no,ac_id,intst_date)tablespace indx;
