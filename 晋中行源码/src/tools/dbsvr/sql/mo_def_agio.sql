drop  table mo_def_agio;
create table mo_def_agio(
	tx_code varchar2(4),
	cif_lvl varchar2(1),
	cif_lvl_rate number(8,5) 
)tablespace users;
