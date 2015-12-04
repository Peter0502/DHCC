/**        批量代收业务发送包登记簿 **/
drop index lv_dsfsendlist_idx;
drop  table lv_dsfsendlist;
create table lv_dsfsendlist(
  packid           varchar2(8)  , /**包序号       **/
  orderno          varchar2(8)  , /**交易序号　   **/
  wt_date          number(9)    , /**包委托日期   **/
  tx_type          varchar2(5)  , /**交易类型     **/
  or_br_no         varchar2(12) , /**发起行行号   **/
  ac_br_no         varchar2(12) , /**接收行行号   **/
  msg_len          varchar2(8)  , /**信息长度     **/
  add_tag          varchar2(100), /**附加数据域，放前100个**/
  o_packid         varchar2(8)  , /**原交易包序号 **/
  o_orderno        varchar2(8)  , /**原交易序号   **/
  o_wt_date        number(9)    , /**原委托日期   **/
  o_tx_type        varchar2(5)  , /**原交易类型   **/
  sts              varchar2(2)  , /**交易状态     **/
  content          varchar2(60) , /**附言         **/
  re_packid        varchar2(8)  , /**回执包包号   **/
  re_orderno       varchar2(8)  , /**回执交易序号 **/
  re_wt_date       number(9)    , /**回执委托日期 **/
  re_or_br_no      varchar2(12) , /**回执发起行   **/
  re_ac_br_no      varchar2(12) , /**回执接收　行 **/
  re_msg_len       varchar2(8)  , /**回执信息长度 **/
  re_addit         varchar2(91) , /**回执附加数据域，只有91字节**/
  repstat          varchar2(2)  , /**回执状态     **/
  re_content       varchar2(60)   /**回执附言     **/
)tablespace users;
create   unique index lv_dsfsendlist_idx on lv_dsfsendlist(wt_date,orderno,or_br_no)tablespace indx;

