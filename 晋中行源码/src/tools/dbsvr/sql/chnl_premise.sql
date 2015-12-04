drop  index chnl_premis_idx;
drop  table chnl_premise;
create table chnl_premise(
	pt_date       number(9),	 /* ƽ̨�����뿨��,ƽ̨��ˮΨһȷ��һ��ҵ��*/
	pt_trace_no   number(6),	/* ƽ̨��ˮ���뿨��,ƽ̨����Ψһȷ��һ��ҵ��*/
	ac_no         varchar2(19), 	/* ������ƽ̨����,ƽ̨��ˮ��Ψһȷ��һ��ҵ��*/
	l_premise_no  number(6),    	/* �ϱ�Ԥ��Ȩ��*/
	premise_no    number(6),    	/* Ԥ��Ȩ��*/
	premise_amt   number(16,2), 	/* Ԥ��Ȩ���*/
	finish_amt    number(16,2), 	/* ��ɽ��*/
	sts           varchar2(1),  	/* Ԥ��Ȩ״̬ 0 ����Ԥ��Ȩ 1Ԥ��Ȩ���� 2Ԥ��Ȩ��� 3Ԥ��ȨʧЧ */
	filler        varchar2(30)  	/* ��ע*/
)tablespace users;
create unique index chnl_premis_idx on chnl_premise(pt_date,pt_trace_no,ac_no)tablespace indx;