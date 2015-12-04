drop  table note_mst_tmp;
create table note_mst_tmp(
	tel varchar2(4),
	note_type varchar2(3),
	br_no varchar2(5),
	beg_note_no varchar2(16),
	end_note_no varchar2(16),
	sts varchar2(1),
	cnt number(9),
	ys_cnt number(9) 
)tablespace users;
