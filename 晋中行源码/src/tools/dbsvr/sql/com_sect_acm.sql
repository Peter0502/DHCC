drop  table com_sect_acm;
create table com_sect_acm(
	ac_id number(9),
	ac_seqn number(6),
	beg_date number(9),
	end_date number(9),
	rate number(8,5),
	intst_acm number(19,2),
	intst_type varchar2(1),
	sts varchar2(1) 
)tablespace users;
