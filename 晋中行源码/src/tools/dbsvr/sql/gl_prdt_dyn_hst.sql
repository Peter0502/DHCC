drop index gl_prdt_dyn_hst_1;
drop  table gl_prdt_dyn_hst;
create table gl_prdt_dyn_hst(
	"date" number(9),
	prdt_cod varchar2(3),
	br_no varchar2(5),
	cr_bal number(16,2),
	ac_cnt number(9),
	lcd_bal number(16,2),
	l_ac_cnt number(9),
	d_opn_cnt number(9),
	d_cls_cnt number(9),
	rdd_cnt number(9),
	rcd_cnt number(9),
	rdd_amt number(16,2),
	rcd_amt number(16,2),
	cdd_cnt number(9),
	ccd_cnt number(9),
	cdd_amt number(16,2),
	ccd_amt number(16,2),
	spe_ind varchar2(3) 
)tablespace users;
create	 unique index gl_prdt_dyn_hst_1 on gl_prdt_dyn_hst( prdt_cod,br_no,"date")tablespace indx;
