     /**CMT503 ҵ��Ȩ�޿���֪ͨ�Ǽǲ� **/
drop index lv_c503r_idx;
drop table lv_c503r;
create table lv_c503r
(
	bakcode   varchar2(12),     /**����  **/
	sendcnt   number(6),        /**������Ŀ >=0��ʾ״̬����  **/
	sendgrant varchar2(2000),   /**����  **/
	recvcnt   number(6),        /**������Ŀ  **/
	recvgrant varchar2(2000),   /**����  **/
	upd_date  varchar2(8),      /**��������  **/
	upd_node  varchar2(4),       /**���½ڵ�  **/
)tablespace users;
create unique index lv_c503r_idx on lv_c503r(bakcode)tablespace indx;
