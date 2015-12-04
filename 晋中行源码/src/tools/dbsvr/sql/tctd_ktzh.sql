drop   index tctd_ktzh_idx;
drop   index tctd_ktzh_idx2;
drop   table tctd_ktzh;
create table tctd_ktzh(
       br_no      varchar2(5),   /*��ͨ����*/
       tctd_br_no varchar2(12),  /*���з������ͨ�е��к�*/
       begin_date number(8),     /*��������*/
       end_date   number(8),     /*ֹ������*/
       tctd_acno  varchar2(24),  /*��ͨ���˺�*/
       tctd_name  varchar2(60),  /*��ͨ�Ļ���*/
       tc_kt_flag varchar2(1),   /*ͨ�濪ͨ�ı�־*0,ֹ�ã�1������*/
       td_kt_flag varchar2(1),   /*ͨ�ҿ�ͨ�ı�־*0.ֹ�ã�1������*/
       cx_kt_flag varchar2(1),   /*��ѯ��ͨ�ı�־*0,ֹ�ã�1������*/
       td_amt_lmt number(17,2),  /*ÿ��ͨ���ۼ��޶�*/
       tctd_xyno  varchar2(60),  /*ͨ��ͨ��Э���*/
       tel        varchar2(4) ,  /*���׹�Ա*/
       trace_no   number(6),     /*������ˮ*/
       filler     varchar2(60)   /*��ע�ֶ�*/
)tablespace users;
create   unique index tctd_ktzh_idx on tctd_ktzh(tctd_acno)tablespace indx;
create   unique index tctd_ktzh_idx2 on tctd_ktzh(tctd_xyno)tablespace indx;
