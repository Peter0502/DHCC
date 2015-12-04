drop  table cif_wrk_hst_inf;
create table cif_wrk_hst_inf(
	cif_no number(9),
	wrk_no number(6),
	industry_no varchar2(3),
	vocation_no varchar2(3),
	position_no varchar2(3),
	beg_date number(9),
	wrk_ind varchar2(1),
	end_date number(9),
	unit_name varchar2(50),
	income number(16,2) 
)tablespace users;
