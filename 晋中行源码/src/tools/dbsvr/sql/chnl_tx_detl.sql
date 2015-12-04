/*��С�����ˮ�ǼǱ���������������Ӧ��ҵ����Ϣ��*/
drop index chnl_tx_detl_1;
drop index chnl_tx_detl_2;
drop  table chnl_tx_detl;
create table chnl_tx_detl(
  tx_code		 varchar(6)    not null,
  syscode  		 varchar(6)    not null,
  chnl_type      varchar(1)    not null,  /*�������ͣ�1:������ 2:ũ���� 3:����֧�� 4:��������ƽ̨ 5:����ũ����*/
  tx_date        number(9)       not null,  /*��������*/                                     
  tx_time        varchar(26)   not null,  /*����ʱ��*/                                     
  trace_no       number(9)       not null,  /*������ˮ*/                                     
  trace_cnt      number(9)       not null,  /*������ˮ�ʴ�*/                                 
  pt_date        number(9)       not null,  /*ƽ̨����*/                                     
  pt_trace_no    number(9)       not null,  /*ƽ̨��ˮ*/                                     
  wt_date        number(9)       not null,	/*ί������*/                                   
  orderno		 varchar(12)     not null,	/*֧�����*/                                     
  or_br_no	   	 varchar(14)   	not null,	/*�������к�*/                                     
  sndrcvflag	 varchar(4)      not null,	/*������ʾ*/                                   
  ac_no          varchar(32)           ,  /*�����˺� �����˻�*/
  ac_name        varchar(60)           ,  /*�����˻����� add by xk*/                      
  add_ind        varchar(1)            ,  /*���˺�������־ 0�� 1��*/                     
  ct_ind         varchar(1)            ,  /*��ת��־ 1�� 2ת */                          
  tx_amt         number(18,2)          ,  /*���׽��*/                                   
  df_br_no       varchar(12)           ,  /*�Է����к�*/                                 
  df_ac_no       varchar(32)           ,  /*�Է��˺�*/                                   
  df_name        varchar(60)           ,  /*�Է�����*/                                     
  df_id_type     varchar(3)            ,  /*�Է�id*/                                       
  df_id_no       varchar(20)           ,  /*�Է�֤������*/                                 
  txnum          varchar(4)            ,  /*ҵ�����ͱ��*/                                 
  yw_type        varchar(5)            ,  /*ҵ������*/                                   
  dz_ind         varchar(2)      not null,	/*����״̬*/                                   
  filler1        varchar(60)           ,  /*��ע1*/                                      
  filler2        varchar(60)           ,  /*��ע2*/                                      
  filler3        varchar(60)           ,  /*��ע3*/                                      
  filler4        varchar(60)              /*��ע4*/
);
create  index chnl_tx_detl_1 on chnl_tx_detl(tx_date,trace_no,wt_date,orderno,or_br_no,sndrcvflag);
create  unique index chnl_tx_detl_2 on chnl_tx_detl(chnl_type,pt_date,pt_trace_no,sndrcvflag);
create  index chnl_tx_detl_3 on chnl_tx_detl(ac_no);

