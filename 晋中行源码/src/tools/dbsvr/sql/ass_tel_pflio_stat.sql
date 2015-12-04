drop  table ass_tel_pflio_stat;
create table ass_tel_pflio_stat(
	br_no varchar2(5),
	tel varchar2(5),
	prdt_code varchar2(3),
	"date" number(9),
	trad_code varchar2(4),
	ct_ind varchar2(1),
	cnt number(9),
	amt number(16,2) 
)tablespace users;
