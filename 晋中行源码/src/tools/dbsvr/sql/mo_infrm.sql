drop  table mo_infrm;
create table mo_infrm(
	ac_id number(9),
	ac_seqn number(6),
	infrm_date number(9),
	prm_draw_date number(9),
	prm_draw_amt number(16,2),
	fct_wd_amt number(16,2),
	occ_date number(9),
	sts varchar2(1),
	name varchar2(50),
	phone varchar2(20),
	wrk_date number(9),
	trace_no number(9),
	id_knd varchar2(1),
	id_no varchar2(20) 
)tablespace users;
