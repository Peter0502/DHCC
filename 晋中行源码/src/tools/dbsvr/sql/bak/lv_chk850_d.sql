      /**	С����ܺ˶Ա��ĵǼǲ�**/
drop index lv_chk850_d_idx;
drop table lv_chk850_d;
create table lv_chk850_d(
   lw_flg       varchar2(1),  /* ���������ʶ 1����2���� */
   zc_jd_lx     varchar2(1),  /* ����ڵ����� */
   zc_date      number(8),  /** ��������    **/
   zc_cj        varchar2(2),  /* ����� */
   pkgno        varchar2(3),  /* �����ͺ� */
   pack_sts   	varchar2(2),  /*��״̬01-�Ѿܾ�,02-�ѷ���,03-������,04-���Ŷ�,
                                     05-������,06-�ѳ���,10-�ѻ�ִ,11-�Ѿܸ�,
                                     12-�ѳ���,13-��ֹ��,14-�ѳ���,15-��ִ��,
                                     99-�޴�ҵ�� */
   filler       varchar2(60)  /** ��ע            **/
)tablespace users;
create unique index lv_chk850_d_idx on lv_chk850_d(lw_flg,zc_date,pkgno,pack_sts)tablespace indx;
