/******************************************************************************/
/*  table name   : mob_server_type   (���� ����/�շ� ���ͱ�)                         */
/*  create by    : houyi      20100516                                        */
/******************************************************************************/
drop table mob_server_type;
drop table mob_server_type_idx;
create table mob_server_type(
	server_type			varchar2(4),		/**��������**/
	server_name			varchar2(50),  /**��������**/
	fee_term			varchar2(1),   /**�շ�����0.ʵʱ��,1������**/
	fee_standard			number(16,2),      /**�շѱ�׼**/
	server_sts			varchar2(1)    /**����״̬0��ֹ1����**/
)tablespace users;

