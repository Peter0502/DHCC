drop  table gl_mst_hst;
create table gl_mst_hst(
	br_no varchar2(5),
	cur_no varchar2(2),
	acc_hrt varchar2(5),
	"date" number(9),
	dc_ind varchar2(1),
	up_acc_hrt varchar2(5),
	dr_bal number(16,2),
	cr_bal number(16,2),
	ldd_bal number(16,2),
	lcd_bal number(16,2),
	rdd_cnt number(9),
	rcd_cnt number(9),
	rdd_amt number(16,2),
	rcd_amt number(16,2),
	cdd_cnt number(9),
	ccd_cnt number(9),
	cdd_amt number(16,2),
	ccd_amt number(16,2) 
)tablespace users;
