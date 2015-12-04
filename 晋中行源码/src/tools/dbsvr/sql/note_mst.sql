drop index note_mst_1;
drop index note_mst_2;
drop  table note_mst;
create table note_mst(
	tel varchar2(4),
	note_type varchar2(3),
	br_no varchar2(5),
	beg_note_no varchar2(16),
	end_note_no varchar2(16),
	sts varchar2(1),
	cnt number(9),
	ys_cnt number(9) 
)tablespace users;
create	 index note_mst_1 on note_mst( tel,note_type,beg_note_no)tablespace indx;
create	 index note_mst_2 on note_mst( br_no,note_type,sts)tablespace indx;
