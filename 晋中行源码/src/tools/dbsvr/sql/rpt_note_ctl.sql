drop  table rpt_note_ctl;
create table rpt_note_ctl(
	no number(9),
	txname varchar2(20),
	wherelist varchar2(200),
	chk_code varchar2(100),
	txbalmd number(9),
	grouplist varchar2(80),
	tw_acchrt varchar2(8),
	summary varchar2(40),
	flag varchar2(10) 
)tablespace users;
