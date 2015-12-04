drop index auth_tel_auth_1;
drop  table auth_tel_auth;
create table auth_tel_auth(
	tel varchar2(4),
	au_al varchar2(3),
	au_xj varchar2(3),
	au_zz varchar2(3),
	au_tc varchar2(3),
	au_de varchar2(3),
	ind varchar2(10) 
)tablespace users;
create	 unique index auth_tel_auth_1 on auth_tel_auth( tel)tablespace indx;
