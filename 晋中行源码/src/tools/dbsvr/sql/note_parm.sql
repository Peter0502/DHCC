drop index note_parm_1;
drop  table note_parm;
create table note_parm(
	note_type varchar2(3),
	acc_hrt varchar2(5),
	name varchar2(20),
	sell_ind varchar2(1),
	draw_pwd_ind varchar2(1),
	dc_ind varchar2(1),
	ct_ind varchar2(1),
	ctl_no_ind varchar2(1),
	order_ind varchar2(1),
	cur_no varchar2(2),
	unit_amt number(16,2),
	dc_unit_mode varchar2(1),
	use_ind varchar2(2),
	cif_type varchar2(1),
	unit_cnt number(6),
	ocp_cnt number(6),
	buy_unit number(9),
	buy_cnt number(6),
	unit_lmt number(6),
	cnt_lnt number(6),
	head_len number(6) 
)tablespace users;
create	 unique index note_parm_1 on note_parm( note_type)tablespace indx;
