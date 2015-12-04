drop  table ass_prdt_sale_sta;
create table ass_prdt_sale_sta(
	br_no varchar2(5),
	prdt_code varchar2(3),
	"date" number(9),
	sal_cnt number(9),
	ttl_amt number(16,2) 
)tablespace users;
