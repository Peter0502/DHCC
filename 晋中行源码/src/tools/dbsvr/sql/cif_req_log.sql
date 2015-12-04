drop  table cif_req_log;
create table cif_req_log(
	advice_type varchar2(1),
	cif_no number(9),
	seq_no number(6),
	req_clerk_no varchar2(5),
	request varchar2(40),
	"date" number(9),
	clerk_no varchar2(5) 
)tablespace users;
