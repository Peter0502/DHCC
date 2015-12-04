drop index ecds_recv_reg_idx;
drop table ecds_recv_reg;
create table ecds_recv_reg(
ecds_tx_date	number(9),       /*日期        */
ecds_trace_no	varchar2(28),    /*交易流水号  */
trace_no	number(9),           /*核心流水号  */
tx_date	number(9),             /*核心交易日期*/
drftcls	varchar2(1),           /*票据分类    */
drfttyp	varchar2(4),           /*票据类型    */
drftnb	varchar2(30),          /*票据号      */
issedt	number(9),             /*出票日      */
duedt	number(9),               /*到期日      */
tag_name	varchar2(30),        /*报文类型    */
sts	varchar2(1),               /*接收状态    */
check_flag	varchar2(1),                   /*对账标志    */
bf_flag   varchar2(1)		   /**标志**/
)tablespace users;                                           
create unique index ecds_recv_reg_idx on ecds_recv_reg(tx_date,ecds_trace_no)tablespace indx;
