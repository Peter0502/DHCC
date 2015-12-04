drop index cash_mst_1;
drop  table cash_mst;
create table cash_mst(
	br_no varchar2(5),
	tel varchar2(4),
	cur_no varchar2(2),
	ac_no varchar2(10),
	bal number(16,2),
	ys_bal number(16,2),
	sts varchar2(1) 
)tablespace users;
create	 unique index cash_mst_1 on cash_mst( br_no,tel,cur_no)tablespace indx;
