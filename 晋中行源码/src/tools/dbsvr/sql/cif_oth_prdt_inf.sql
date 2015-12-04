drop  table cif_oth_prdt_inf;
create table cif_oth_prdt_inf(
	cif_no number(9),
	inf_no number(6),
	prdt_code varchar2(2),
	unit_type varchar2(2),
	unit_no varchar2(6),
	ac_no varchar2(24),
	pro_cur_no varchar2(2),
	balance number(12,2),
	mon_pay number(12,2) 
)tablespace users;
