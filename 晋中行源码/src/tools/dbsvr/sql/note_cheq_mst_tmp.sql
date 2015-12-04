drop  table note_cheq_mst_tmp;
create table note_cheq_mst_tmp(
	ac_id number(9),
	ac_seq number(6),
	note_type varchar2(3),
	br_no varchar2(5),
	beg_note_no varchar2(16),
	end_note_no varchar2(16),
	book_sts varchar2(50),
	grp_ind varchar2(1),
	cnt number(6),
	sell_date number(9) 
)tablespace users;
