drop index ecds_recv_reg_idx;
drop table ecds_recv_reg;
create table ecds_recv_reg(
ecds_tx_date	number(9),       /*����        */
ecds_trace_no	varchar2(28),    /*������ˮ��  */
trace_no	number(9),           /*������ˮ��  */
tx_date	number(9),             /*���Ľ�������*/
drftcls	varchar2(1),           /*Ʊ�ݷ���    */
drfttyp	varchar2(4),           /*Ʊ������    */
drftnb	varchar2(30),          /*Ʊ�ݺ�      */
issedt	number(9),             /*��Ʊ��      */
duedt	number(9),               /*������      */
tag_name	varchar2(30),        /*��������    */
sts	varchar2(1),               /*����״̬    */
check_flag	varchar2(1),                   /*���˱�־    */
bf_flag   varchar2(1)		   /**��־**/
)tablespace users;                                           
create unique index ecds_recv_reg_idx on ecds_recv_reg(tx_date,ecds_trace_no)tablespace indx;
