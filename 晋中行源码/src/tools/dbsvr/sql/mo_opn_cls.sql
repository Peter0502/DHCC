drop  table mo_opn_cls;
create table mo_opn_cls(
	ac_no varchar2(19),
	ac_id number(9),
	ac_seqn number(6),
	cur_no varchar2(2),
	name varchar2(50),
	prdt_no varchar2(3),
	acc_hrt varchar2(5),
	cif_type varchar2(1),
	amt number(16,2),
	opn_br_no varchar2(5),
	tx_date number(9),
	tx_brno varchar2(5),
	trace_no number(9),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4),
	check_no varchar2(20),
	ct_ind varchar2(1),
	wrk_sts varchar2(1) 
)tablespace users;
