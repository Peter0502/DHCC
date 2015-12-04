drop index gl_rpt_1;
drop  table gl_rpt;
create table gl_rpt(
	br_no varchar2(5),
	"date" number(9),
	code varchar2(20),
	amt number(16,2) 
)tablespace users;
create	 unique index gl_rpt_1 on gl_rpt( br_no,"date",code)tablespace indx;
