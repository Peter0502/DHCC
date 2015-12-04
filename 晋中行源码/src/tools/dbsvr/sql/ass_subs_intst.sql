drop  table ass_subs_intst;
create table ass_subs_intst(
	brno varchar2(5),
	"date" number(9),
	beg_date number(9),
	end_date number(9),
	acm number(19,2),
	rate number(8,5),
	deal_ind varchar2(1),
	dc_ind varchar2(1) 
)tablespace users;
