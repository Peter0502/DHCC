drop index ln_mst_bk_1;
drop  table ln_mst_bk;
create table ln_mst_bk(
	opn_br_no varchar2(5),
	prdt_no varchar2(3),
	bal number(16,2),
	in_lo_intst number(16,2),
	out_lo_intst number(16,2),
	cmpd_lo_intst number(16,2),
	gage_amt number(16,2),
	cnt number(9) 
)tablespace users;
create	 unique index ln_mst_bk_1 on ln_mst_bk( opn_br_no,prdt_no)tablespace indx;
