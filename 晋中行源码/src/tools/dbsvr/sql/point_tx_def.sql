drop  table point_tx_def;
create table point_tx_def(
	tx_code varchar2(4),
	cif_type varchar2(1),
	beg_date number(9),
	edn_date number(9),
	beg_bal number(16,2),
	edn_bal number(16,2),
	max_point number(6),
	min_point number(6),
	ratio number(6) 
)tablespace users;
