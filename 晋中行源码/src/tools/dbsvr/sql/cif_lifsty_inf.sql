drop index cif_lifsty_inf_1;
drop  table cif_lifsty_inf;
create table cif_lifsty_inf(
	cif_no number(9),
	lifsty_no number(6),
	life_type varchar2(2),
	life_detail varchar2(2) 
)tablespace users;
create	 unique index cif_lifsty_inf_1 on cif_lifsty_inf( cif_no,lifsty_no)tablespace indx;
