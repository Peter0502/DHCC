drop index prdt_jz_dz_1;
drop  table prdt_jz_dz;
create table prdt_jz_dz(
	prdt_no varchar2(3),---原产品号
	title varchar2(60),----原产品名称
	jz_prdt_no varchar2(3),---相对应减值的产品号
	jz_title varchar2(60)   -----减值产品名称
)tablespace users;
create	 unique index prdt_jz_dz_1 on prdt_jz_dz(prdt_no );
