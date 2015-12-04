drop  table ass_mger_ach_stat;
create table ass_mger_ach_stat(
	"date" number(9),
	br_no varchar2(5),
	mang_no varchar2(4),
	save_amt number(16,2),
	ln_amt number(16,2),
	ln_prvd_amt number(16,2),
	ln_norm_amt number(16,2),
	ln_over_amt number(16,2),
	ln_stgn_amt number(16,2),
	ln_die_amt number(16,2),
	intst_pay_intst number(16,2),
	intst_lo_intst number(16,2) 
)tablespace users;
