drop index sum_tax_1;
drop  table sum_tax;
create table sum_tax(
	"date" number(9),
	br_no varchar2(5),
	prdt_no varchar2(3),
	cls_amt number(16,2),
	intst number(16,2),
	tax number(16,2),
	cnt number(9) 
)tablespace users;
create	 index sum_tax_1 on sum_tax( "date",br_no,prdt_no)tablespace indx;
