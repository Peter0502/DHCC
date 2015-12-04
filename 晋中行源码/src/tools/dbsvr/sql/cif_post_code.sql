drop  table cif_post_code;
create table cif_post_code(
	city_code varchar2(4),
	city_name varchar2(12),
	area_code varchar2(2),
	area_name varchar2(12),
	post_code varchar2(6) 
)tablespace users;
