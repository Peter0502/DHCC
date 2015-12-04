drop index mdm_ac_rel_1;
drop index mdm_ac_rel_2;
drop  table mdm_ac_rel;
create table mdm_ac_rel(
	opn_br_no varchar2(5),
	ac_no varchar2(19),
	ac_id number(9),
	ac_seqn number(6),
	mdm_code varchar2(4),
	note_no varchar2(16),
	note_sts varchar2(1),
	coll_sts varchar2(1),
	beg_date number(9),
	end_date number(9),
	draw_uncon_yn varchar2(1),
	draw_pwd_chg varchar2(1),
	draw_pwd_yn varchar2(1),
	qry_pwd varchar2(6),
	draw_pwd varchar2(6),
	draw_id_yn varchar2(1),
	id_type varchar2(1),
	id_no varchar2(20),
	draw_seal_yn varchar2(1),
	pwd_mach_yn varchar2(1),
	book_bal number(16,2),
	prt_line number(9),
	main_ind varchar2(1),
	mac varchar2(16) 
)tablespace users;
create	 unique index mdm_ac_rel_1 on mdm_ac_rel( ac_no)tablespace indx;
create	 index mdm_ac_rel_2 on mdm_ac_rel( ac_id)tablespace indx;
