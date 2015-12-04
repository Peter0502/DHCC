drop index note_mst_hst_1;
drop  table note_mst_hst;
create table note_mst_hst(
	tel varchar2(4),
	note_type varchar2(3),
	br_no varchar2(5),
	beg_note_no varchar2(16),
	end_note_no varchar2(16),
	note_cnt number(9),
	tx_date number(9),
	tx_time number(9),
	trace_no number(9),
	trace_cnt number(9),
	auth varchar2(5),
	tw_br_no varchar2(5),
	tw_tel varchar2(5),
	sts_ind varchar2(1) 
)tablespace users;
create	 index note_mst_hst_1 on note_mst_hst( br_no,tx_date,note_type,sts_ind)tablespace indx;
