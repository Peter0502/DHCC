      /**	С����ܺ˶Ա��ĵǼǲ�**/
drop table lv_chk850_m;
create table lv_chk850_m(
   in_date       number(8) ,  /** ����ϵͳ����    **/
   pack_date     number(8) ,  /** ί������        **/
   dz_date       number(8) ,  /*  �������� */
   cash_qs_no    varchar2(12),  /** ���ʽ���(�����������к�)  **/
   qs_zt_cnt     number(6),     /* ������״̬���˼�¼��Ŀ */
   qt_zt_cnt     number(6),     /* ����״̬���˼�¼��Ŀ */
   filler        varchar2(60)   /** ��ע            **/
)tablespace users;
