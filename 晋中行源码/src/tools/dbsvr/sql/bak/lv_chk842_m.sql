      /**	С����ܺ˶Ա��ĵǼǲ�**/
drop table lv_chk842_m;
create table lv_chk842_m(
   dz_date       number(8)  ,  /** ����ϵͳ����    **/
   pack_date     number(8)  ,  /** ί������        **/
   cash_qs_no 	 varchar2(12) ,  /** ���ʽ���(�����������к�)  **/
   qs_zt_cnt  	 number(6),    /* ������״̬���˼�¼��Ŀ */
   qt_zt_cnt   	 number(6),    /* ����״̬���˼�¼��Ŀ */
   filler        varchar2(60)    /** ��ע            **/
)tablespace users;
