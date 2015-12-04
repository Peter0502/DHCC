drop  table mo_ac_chg_hst;
create table mo_ac_chg_hst(
	ac_id number(9),
	ac_seqn number(6),
	tx_code varchar2(4),
	tx_br_no varchar2(5),
	chg_tab_name varchar2(18),
	chg_fld_name varchar2(18),
	bef_chg_tent varchar2(40),
	aft_chg_tent varchar2(40),
	tx_date number(9),
	tx_time number(9),
	trace_no number(9),
	trace_cnt number(9),
	brf varchar2(20),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
