drop index prdt_jz_dz_1;
drop  table prdt_jz_dz;
create table prdt_jz_dz(
	prdt_no varchar2(3),---ԭ��Ʒ��
	title varchar2(60),----ԭ��Ʒ����
	jz_prdt_no varchar2(3),---���Ӧ��ֵ�Ĳ�Ʒ��
	jz_title varchar2(60)   -----��ֵ��Ʒ����
)tablespace users;
create	 unique index prdt_jz_dz_1 on prdt_jz_dz(prdt_no );
