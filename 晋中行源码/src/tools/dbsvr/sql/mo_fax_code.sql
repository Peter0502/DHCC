drop  table mo_fax_code;
create table mo_fax_code(
	bank_no varchar2(5),
	bank_name varchar2(30),
	posi_name varchar2(10),
	phone varchar2(12),
	tele_code varchar2(6),
	zip varchar2(6),
	clr_seq_no varchar2(6),
	clr_jour_no varchar2(6),
	clr_lst_occ_date varchar2(6),
	clr_seq_mode varchar2(1),
	clr_jour_mode varchar2(1),
	clr_ctl varchar2(3) 
)tablespace users;
