drop index td_mst_bk_1;
drop  table td_mst_bk;
create table td_mst_bk(
	opn_br_no varchar2(5),
	prdt_no varchar2(3),
	bal number(16,2),
	cnt number(9) 
)tablespace users;
create	 unique index td_mst_bk_1 on td_mst_bk( opn_br_no,prdt_no)tablespace indx;
