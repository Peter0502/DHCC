/*���˵ǼǱ�*/
drop  index chnl_compare_acct_1;
drop  table chnl_compare_acct;
create table chnl_compare_acct(
	detail_no      number(9)     not null,  /*��ϸ���:��1��ʼ+1*/
	un_marking     varchar(32)   not null,	/*Ψһ��ʶ*/
	syscode        varchar(5)    not null,  /*ϵͳ���*/
	com_date       varchar(9)    not null,  /*��������*/
	code_type      varchar(6)    not null,  /*��������*/
	wt_date        varchar(9)    not null,  /*ί������*/
	orderno		   number(9)     not null,	/*֧�����*/
	pt_date        number(9)     not null,  /*ƽ̨����*/
	pt_trace_no    number(9)     not null,  /*ƽ̨��ˮ*/
	or_br_no	   varchar(14)   not null,	/*�������к�*/
	sndrcvflag	   varchar(5)    not null,	/*������ʾ*/
	tx_amt         number(18,2)  not null,  /*���׽��*/
	con_marking    varchar(11)           ,	/*���Ʊ�ʶ*/
	bor_lend       varchar(5)            ,  /*�����ʾ*/
	main_acc_no	   varchar(32)           ,  /*���˺�*/
	main_acc_nm	   varchar(60)           ,  /*���˺Ż���*/
	gd_acc_no	   varchar(32)           ,  /*�����˺�*/
	snd_acc_no	   varchar(32)           ,  /*�ڶ��˺�*/
	note_type      varchar(5)            ,  /*ƾ֤����*/
	note_no        varchar(17)           ,  /*ƾ֤����*/
	tx_br_no       varchar(6)            ,  /*���׻���*/
	teller         varchar(7)            ,  /*����Ա��*/
	tx_date        number(9)             ,  /*��������*/
	trace_no       number(9)             ,  /*������ˮ*/
	com_stat       varchar(2)            ,  /*���˱�ʾ 0-���˶��� 1-�������� 2-���� 3-����*/
	batch_detail   varchar(2)            ,  /*���˷�ʽ��0 - ���� 1 - ����*/
	chrgflag       varchar(2)            ,  /*�շѱ�ʶ: 0 - ���� 1 - ��*/
	chrg_ind       varchar(2)            ,  /*�����ѱ�ʶ: 1 -�ֽ�  2 - ת��  3 - ����*/
	chrg_amt       number(18,2)          ,  /*�����ѽ��*/
	chrgacno       varchar(32)           ,  /*�����ѳ���*/
	feeaccno       varchar(32)           ,  /*�����������Ŀ*/
	com_pt_date    number(9)             ,  /*����ƽ̨����*/
	com_swno       number(9)             ,  /*����ƽ̨��ˮ*/
	gz_br_no       varchar(6)            ,  /*���˻���*/
	gz_acct_no     varchar(32)           ,  /*�����˺�*/
	gz_reasen      varchar(3)            ,  /*����ԭ��:*/
	com_flag       varchar(2)            ,  /*���˱�ʶ:0 - ����ƽ 1 - ���˴�*/
	com_result     varchar(60)              /*���˽������*/
);
create  unique index chnl_compare_acct_1 on chnl_compare_acct(un_marking,syscode,com_date);



