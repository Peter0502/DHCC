drop index lv_c503r_idx;	/**ҵ��Ȩ�޿��Ʊ�**/
drop table lv_c503r;
create table lv_c503r(
	bakcode        varchar2(12),	/**�����߻�CCPC����**/
	sendcnt        number(6),	/**����Ȩ����Ŀ**/
	sendgrant      varchar2(2000),	/**����Ȩ��**/
	recvcnt        number(6),	/**����Ȩ����Ŀ**/
	recvgrant      varchar2(2000),	/**����Ȩ��**/
	upd_date       varchar2(8),	/**��������**/
	upd_node       varchar2(4)	/**���½��**/
)tablespace users;
create unique index lv_c503r_idx on lv_c503r(bakcode);

