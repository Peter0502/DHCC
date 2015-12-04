/******************************************************************************/
/*  table name   : mob_server_type   (短信 服务/收费 类型表)                         */
/*  create by    : houyi      20100516                                        */
/******************************************************************************/
drop table mob_server_type;
drop table mob_server_type_idx;
create table mob_server_type(
	server_type			varchar2(4),		/**服务类型**/
	server_name			varchar2(50),  /**服务名称**/
	fee_term			varchar2(1),   /**收费期限0.实时收,1按月收**/
	fee_standard			number(16,2),      /**收费标准**/
	server_sts			varchar2(1)    /**服务状态0废止1正常**/
)tablespace users;

