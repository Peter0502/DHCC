drop index cif_cop_inf_1;
drop  table cif_cop_inf;
create table cif_cop_inf(
	cif_no number(9),
	reg_date number(9),
	reg_country varchar2(3),
	reg_fund number(16,2),
	cur_no varchar2(2),
	crd_no varchar2(20),
	units_license varchar2(20),
	g_tax_no varchar2(20),
	d_tax_no varchar2(20),
	credit_no varchar2(16),
	license varchar2(20),
	eco_attri varchar2(1),
	ent_attri varchar2(1),
	credit varchar2(1),
	region varchar2(1),
	supervior varchar2(50),
	director varchar2(50),
	belong_bank varchar2(1),
	belong_stat varchar2(1),
	mana_orga_moda varchar2(1),
	bd_corp_orga_moda varchar2(1),
	corp_size varchar2(1),
	corp_act_mode varchar2(1),
	paicl_up_capital number(16,2),
	corp_mana_sts varchar2(1),
	ap_qualife varchar2(20),
	artificial_person varchar2(20),
	ap_id varchar2(20),
	cif_scopes varchar2(2) 
)tablespace users;
create	 unique index cif_cop_inf_1 on cif_cop_inf( cif_no)tablespace indx;
