drop index mdm_attr_1;
drop  table mdm_attr;
create table mdm_attr(
	mdm_code varchar2(4),
	mdm_name varchar2(20),
	mdm_type varchar2(1),
	ac_num_ind varchar2(1),
	draw_uncon_yn varchar2(1),
	qry_pwd_yn varchar2(1),
	draw_pwd_yn varchar2(1),
	draw_id_yn varchar2(1),
	draw_seal_yn varchar2(1),
	pwd_mach_yn varchar2(1),
	note_type varchar2(3),
	only_ind varchar2(1),
	no_ind varchar2(1),
	prt_ind varchar2(1),
	pg_num number(6),
	pg_line number(6) 
)tablespace users;
create	 unique index mdm_attr_1 on mdm_attr( mdm_code)tablespace indx;
