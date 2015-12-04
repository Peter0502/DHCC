drop index cif_sfhc_inf;
drop  table cif_sfhc_inf;
create table cif_sfhc_inf(
	cif_no number(9),
	name varchar2(20),
	g_bir number(9),
	n_bir number(9),
	sex varchar2(1),
	marriage varchar2(1),
	mar_date number(9),
	edu varchar2(1),
	religion varchar2(1),
	nationality varchar2(3),
	country varchar2(3),
	folk varchar2(2),
	language varchar2(1),
	clerk_no varchar2(5),
	health varchar2(1),
	technical_post varchar2(2),
	headship varchar2(2),
	birth_adr varchar2(40),
	mon_income number(16,2),
	h_mon_income number(16,2),
	home_cnt number(6) 
)tablespace users;
create	 unique index cif_sfhc_inf on cif_sfhc_inf( cif_no)tablespace indx;