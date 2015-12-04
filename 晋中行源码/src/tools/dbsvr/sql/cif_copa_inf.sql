drop index cif_copa_inf_1;
drop table cif_copa_inf;
create table cif_copa_inf(
	cif_no number(9),
	ag_cr_no varchar2(18),
	check_no varchar2(20),
	br_no varchar2(8),
	tel varchar2(6),
	beg_date number(8),
	end_date number(8),
	reg_ad_area_no varchar2(10),
	reg_ad_area varchar2(50),
	or_ag_type varchar2(1),
	or_ag_mst_type varchar2(2),
	data_ex_date varchar2(8),
	eco_ind_type varchar2(6),
	eco_type varchar2(2),
	sc_bs varchar2(50),
	cop_sts varchar2(1),
	con_en_name varchar2(50),
	ent_tel varchar2(20),
	up_br_name varchar2(50),
	up_reg_type varchar2(1),
	up_reg_no varchar2(20),
	up_ag_zz_no varchar2(18),
	up_ag_cr_no varchar2(18),
	tx_date number(8),
	up_date number(8)
)tablespace users;
create unique index cif_copa_inf_1 on cif_copa_inf( cif_no)tablespace indx;
