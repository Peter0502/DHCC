drop index cif_shareholder_inf_1;
drop table cif_shareholder_inf;
create table cif_shareholder_inf(
        cif_no number(9),
        shareholder_no number(9),
        name varchar2(20),
        sex varchar2(1),
	type varchar2(1),
        tx_date number(9),
        rate number(8,2),
        id_type varchar2(1),
        id_no varchar2(20),
	br_no varchar2(20),
	ag_cr_no varchar2(20),
        up_date number(8),
	tx_br_no varchar2(6),
	tel varchar2(4)
)tablespace users;
create unique index cif_shareholder_inf_1 on cif_shareholder_inf(cif_no,shareholder_no) tablespace indx;
~

