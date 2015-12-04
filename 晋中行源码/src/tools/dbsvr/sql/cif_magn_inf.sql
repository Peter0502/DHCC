drop  table cif_magn_inf;
create table cif_magn_inf(
	cif_no number(9),
	units_license varchar2(20),
	ap_qualife varchar2(20),
	artificial_person varchar2(20),
	ap_id varchar2(20),
	credit_no varchar2(16),
	own_attri varchar2(1),
	ent_attri varchar2(1),
	credit varchar2(1),
	supervior varchar2(50),
	director varchar2(50),
	financer varchar2(50),
	opn_bank varchar2(50),
	opn_amt number(16,2) 
)tablespace users;
