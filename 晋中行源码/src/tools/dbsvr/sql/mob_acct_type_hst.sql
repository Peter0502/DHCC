/******************************************************************************/
/*  table name   : mob_acct_type_hst   (����ǩԼ��)                           */
/*  table index  : mob_acct_type_hst_idx                                          */
/*  create by    : wangxudong@2010/01/27                                      */
/*  memo         : ���ڱ�ṹ����벻��,���޸ı�ṹ                          */
/*  memo         : ����ǩԼ���׵���ʷ��ѯ���⣬���˺����������Ĳ��������ñ��ֶν�������,wangxd 20101224
                          */
/******************************************************************************/
drop table mob_acct_type_hst;
drop index mob_acct_type_hst_idx;
create table mob_acct_type_hst(
        ac_no           varchar2(20),  /*�˺�(�ƺ����У������˻��ɴ�����ͬ��)*/
        ac_id           number(9),      /*�˻�id������Ϊ0*/
        ac_seqn         number(6),      /**�˻����**/
        ac_type         varchar2(1),   /*�˻�����*/
        server_type     varchar2(4),   /*��������*/
        mobile		varchar2(12),  /*�绰����*/
        ori_tx_date     number(9),      /*ԭ��������*/
        lst_fee_date    number(9),      /*�ϴ��շ�����*/
        lst_tel         varchar2(4),   /*���׹�Ա*/
        sfbz            varchar2(1),   /*�շѱ�־0.���գ�1�շ�*/
        mob_sts         varchar2(1),   /*״̬0-��Լ 1-ǩԼ*/
        phones          varchar2(60),  /**��������**/
        lst_br_no       varchar2(5),   /*ԭǩԼ���׻���*/
        filler          varchar2(50),         
				tx_date		number(9),	/*��������*/
				tx_code		varchar2(4),	/*������*/
				trace_no	number(9),	/*������ˮ��*/
				opr_sts		varchar2(1),	/*������ʶ 0-ǩԼ 1--��Լ 2--�޸� 3--��ͣ 4--����ͣ*/
				br_no		varchar2(5),	/*���׻���*/
				tx_tel		varchar2(6),	/*���׹�Ա*/
				new_ac_no	varchar2(30),	/*�˺Ż�Э���*/
				new_mobile	varchar2(50),	/*����ֻ���*/
				fee		number(16,2),  /*������*/
				auth_tel	varchar2(6),	/*��Ȩ��Ա*/
				filler1		varchar2(15),	/*�����ֶ�1*/
				filler2		varchar2(20),	/*�����ֶ�2*/
				filler3		varchar2(30),	/*�����ֶ�3*/
				filler4		varchar2(50),	/*�����ֶ�4*/
				filler5		varchar2(50),	/*�����ֶ�5*/
				filler6		varchar2(50),	/*�����ֶ�6*/
				memo		varchar2(100),	/*�޸�˵��*/
				sts		varchar2(1)	/*״̬*/
)tablespace users;
create  index mob_acct_type_hst_idx on mob_acct_type_hst ( ac_no,mobile,tx_date,opr_sts ) ;
