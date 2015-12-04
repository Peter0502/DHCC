drop  table mo_put_intst;
create table mo_put_intst(
	prdt_no varchar2(3),
	tx_date number(9),
	acc_ind varchar2(1),
	opn_brno varchar2(5),
	intst_acm number(16,2),
	rate_val number(8,5),
	intst number(16,2) 
)tablespace users;
