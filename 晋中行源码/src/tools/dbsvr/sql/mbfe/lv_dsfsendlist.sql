/**        ��������ҵ���Ͱ��Ǽǲ� **/
drop index lv_dsfsendlist_idx;
drop  table lv_dsfsendlist;
create table lv_dsfsendlist(
  packid           varchar2(8)  , /**�����       **/
  orderno          varchar2(8)  , /**������š�   **/
  wt_date          number(9)    , /**��ί������   **/
  tx_type          varchar2(5)  , /**��������     **/
  or_br_no         varchar2(12) , /**�������к�   **/
  ac_br_no         varchar2(12) , /**�������к�   **/
  msg_len          varchar2(8)  , /**��Ϣ����     **/
  add_tag          varchar2(100), /**���������򣬷�ǰ100��**/
  o_packid         varchar2(8)  , /**ԭ���װ���� **/
  o_orderno        varchar2(8)  , /**ԭ�������   **/
  o_wt_date        number(9)    , /**ԭί������   **/
  o_tx_type        varchar2(5)  , /**ԭ��������   **/
  sts              varchar2(2)  , /**����״̬     **/
  content          varchar2(60) , /**����         **/
  re_packid        varchar2(8)  , /**��ִ������   **/
  re_orderno       varchar2(8)  , /**��ִ������� **/
  re_wt_date       number(9)    , /**��ִί������ **/
  re_or_br_no      varchar2(12) , /**��ִ������   **/
  re_ac_br_no      varchar2(12) , /**��ִ���ա��� **/
  re_msg_len       varchar2(8)  , /**��ִ��Ϣ���� **/
  re_addit         varchar2(91) , /**��ִ����������ֻ��91�ֽ�**/
  repstat          varchar2(2)  , /**��ִ״̬     **/
  re_content       varchar2(60)   /**��ִ����     **/
)tablespace users;
create   unique index lv_dsfsendlist_idx on lv_dsfsendlist(wt_date,orderno,or_br_no)tablespace indx;

