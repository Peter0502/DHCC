drop  table ass_cp_stat;
create table ass_cp_stat(
	gsp_no varchar2(4),
	"date" number(9),
	br_no varchar2(5),
	prdt_code varchar2(3),
	opn_ac_amt number(16,2),
	cls_ac_amt number(16,2),
	opn_ac_cnt number(9),
	cls_ac_cnt number(9),
	ttl_amt number(16,2),
	ttl_cnt number(9) 
)tablespace users;
