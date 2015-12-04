      /**	С����ܺ˶Ա��ĵǼǲ�**/
drop index lv_chk842_d_idx;
drop table lv_chk842_d;
create table lv_chk842_d(
   zc_jd_lx      	varchar2(1),  /* ����ڵ����� */
   zc_date       	number(8),  /* ��������    */
   zc_cc        	varchar2(2),  /* ����� */
   pkgno       		varchar2(3),  /* �����ͺ� */
   wb_cnt       	number(8)  ,  /* �������� */
   wb_tx_amt     	number(22) ,  /* �������ϼ� */
   lb_cnt       	number(8)  ,  /* �������� */
   lb_tx_amt     	number(22) ,  /* �������ϼ� */
   dz_wb_cnt     	number(8)  ,  /* ������������*/
   dz_wb_tx_amt   	number(22) ,  /* �����������ϼ� */
   dz_lb_cnt     	number(8)  ,  /* ������������ */
   dz_lb_tx_amt   	number(22) ,  /* �����������ϼ� */
   pack_sts    		varchar2(2),  /* ��״̬01-�Ѿܾ�,02-�ѷ���,03-������,04-���Ŷ�,
                                       05-������,06-�ѳ���,10-�ѻ�ִ,11-�Ѿܸ�,
                                       12-�ѳ���,13-��ֹ��,14-�ѳ���,15-��ִ��,
                                       99-�޴�ҵ�� */
   filler      		varchar2(60)  /** ��ע            **/
)tablespace users;
create unique index lv_chk842_d_idx on lv_chk842_d(zc_jd_lx,zc_date,zc_cc,pkgno,pack_sts)tablespace indx;

