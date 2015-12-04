drop  table gl_tsub;
create table gl_tsub(
	br_no varchar2(5),
	cur_no varchar2(2),
	acc_hrt varchar2(5),
	"date" number(9),
	bal_d varchar2(1),
	bt_dr_bal number(16,2),
	bt_cr_bal number(16,2),
	r_dr_amt number(16,2),
	r_cr_amt number(16,2),
	r_dr_cnt number(9),
	r_cr_cnt number(9),
	r_dr_bal number(16,2),
	r_cr_bal number(16,2),
	term_type varchar2(1),
	spe_ind varchar2(1) 
)tablespace users;
create index gl_tsub_1 on gl_tsub( br_no,cur_no,acc_hrt,"date",term_type) tablespace indx;
create index gl_tsub_2 on gl_tsub( br_no,acc_hrt,"date",term_type) tablespace indx;
create index gl_tsub_3 on gl_tsub( br_no,acc_hrt,term_type) tablespace indx;
