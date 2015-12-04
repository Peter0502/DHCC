
DROP TABLE hv_zf_erdai;
CREATE TABLE hv_zf_erdai
(
  -- CMT���/��������
	cmtno     	CHAR    ( 20 ) not null ,
  -- ͨ�ż���ʶ��
	commid    	CHAR    ( 20 ) not null ,
  -- ͨ�ż��ο���
	refcommid 	CHAR    ( 20 )          ,
  -- ���ı�ʶ��
	msgno     	CHAR    ( 35 ) not null ,
  -- ��������
	snd_br_no 	CHAR    ( 10 )          ,
  -- ��������
	rcv_br_no 	CHAR    ( 10 )          ,
  -- ί������
	wt_date   	CHAR    (  10 )       not null ,
  -- ���еĽ������
	orderno   	INT            not null ,
  -- ����ʱ��
	tx_time   	CHAR    ( 26 ) not null ,
  -- ҵ������
	yw_kind   	CHAR    (  4 ) not null ,
  -- ҵ������
	yw_type   	CHAR    (  5 ) not null ,
  -- ���ҷ���
	cur_no    	CHAR    (  3 ) not null ,
  -- ���׽��
	tx_amt    	DECIMAL (15,2) not null ,
  -- ֧ȡ��ʽ
	drawtype  	CHAR    (  4 )          ,
  -- ����/֤������/֧������
	drawval   	CHAR    ( 20 )          ,
  -- �����ѱ�־1�ֽ�2ת��3����
	tx_chrg_ind	CHAR    (  1 )          ,
  -- ������
	chrg_amt  	DECIMAL (15,2)          ,
  -- ������ʶSR00-��SR01-��
	lw_ind    	CHAR    (  4 ) not null ,
  -- �����־
	cd_flag   	CHAR    (  4 )          ,
  -- �����������к�(�ʺ�)accountno
	pay_qs_no 	CHAR    ( 14 ) not null ,
  -- �������к�--�˻㽻�׵Ľ�����
	or_br_no  	CHAR    ( 14 ) not null ,
  -- ����������
	or_br_nm  	CHAR    ( 60 )          ,
  -- �����ʺ�
	accno     	CHAR    ( 40 )          ,
  -- �����ʺ�����
	accnm     	CHAR    ( 60 )          ,
  -- �����˿������к�
	pay_opn_br_no	CHAR    ( 14 )          ,
  -- �����˿���������
	pay_opn_br_nm	CHAR    ( 60 )          ,
  -- �������ʺ�
	pay_ac_no 	CHAR    ( 32 ) not null ,
  -- ����������--�˻㽻�׵��տ��˻���
	pay_name  	CHAR    ( 60 ) not null ,
  -- �����˵�ַ
	pay_addr  	CHAR    ( 60 )          ,
  -- �����������к�
	cash_qs_no	CHAR    ( 14 ) not null ,
  -- �������к�--�˻㽻�׵ķ�����
	ac_br_no  	CHAR    ( 14 ) not null ,
  -- ����������
	ac_br_nm  	CHAR    ( 60 )          ,
  -- �տ��˿������к�
	cash_opn_br_no	CHAR    ( 14 )          ,
  -- �տ��˿���������
	cash_opn_br_nm	CHAR    ( 60 )          ,
  -- �տ����ʺ�
	cash_ac_no	CHAR    ( 32 ) not null ,
  -- �տ�������--�˻㽻�׵ĸ����˻���
	cash_name 	CHAR    ( 60 ) not null ,
  -- �տ��˵�ַ
	cash_addr 	CHAR    ( 60 )          ,
  -- �н����1
	mediary1  	CHAR    ( 35 )          ,
  -- �н����1����
	mediaryname1	CHAR    (140 )          ,
  -- �н����2
	mediary2  	CHAR    ( 35 )          ,
  -- �н����2����
	mediaryname2	CHAR    (140 )          ,
  -- ���statԭʼ״̬�����º�ල��
	beg_sts   	CHAR    (  4 )          ,
  -- ���ȼ���
	operlevel 	CHAR    (  4 )          ,
  -- ƾ֤����
	note_type 	CHAR    (  3 )          ,
  -- ���Ӻ���
	note_preno	CHAR    (  4 )          ,
  -- ƾ֤��
	note_no   	CHAR    ( 16 )          ,
  -- �˵��˱�ʶ��
	endtoendid	CHAR    ( 35 )          ,
  -- ��������
	clsdate   	CHAR    (  10 )                ,
  -- ҵ��״̬
	hv_sts    	CHAR    (  4 ) not null ,
  -- �˻㽻���õ���ԭ֧�����ʽ���״̬
	hv_osts   	CHAR    (  4 )          ,
  -- ����״̬
	procstat  	CHAR    (  4 )          ,
  -- ������
	respcode  	CHAR    (  8 )          ,
  -- Ӧ����Ϣ
	rspinfo   	CHAR    (200 )          ,
  -- ҵ����������
	procarty  	CHAR    ( 12 )          ,
  -- �ܾ��ط���־
	resend_ind	CHAR    (  1 )          ,
  -- �ܾ����·�������
	resend_date	CHAR    (  10 )                ,
  -- ��ӡ����
	hv_prt_ind	INT                     ,
  -- ���ж��ʼ��״̬
	checkflag 	CHAR    (  1 )          ,
  -- ���ڶ��ʼ��״̬
	hostflag  	CHAR    (  1 )          ,
  -- ��Ȩ����Ա
	auth_tel  	CHAR    ( 10 )          ,
  -- ��Ȩ����Ա��ˮ��
	auth_trace_no	INT                     ,
  -- ���˲���Ա
	chk       	CHAR    ( 10 )          ,
  -- ���˲���Ա��ˮ��
	chk_trace_no	INT                     ,
  -- ���Ͳ���Ա
	send_tel  	CHAR    ( 10 )          ,
  -- ���Ͳ���Ա��ˮ��
	send_trace_no	INT                     ,
  -- ����ʱ���ļ�������
	host_date_snd	CHAR    (  10 )                ,
  -- ����ʱ���ļ�����ˮ��
	trace_no_snd	INT                     ,
  -- ���׷�������
	channel   	CHAR    ( 10 )          ,
  -- һ������ʶ
	cnapsflag 	CHAR    (  1 )          ,
  -- ��������
	tx_date   	CHAR    (  10 )       not null ,
  -- ����ƽ̨��ˮ��
	rcv_swno  	INT                     ,
  -- ���ļ�������
	host_date 	CHAR    (  10 )                ,
  -- ���ļ�����ˮ��
	trace_no  	INT                     ,
  -- ����ԱԱ
	tel       	CHAR    ( 10 ) not null ,
  -- ���˱�־
	susflag   	CHAR    (  1 )          ,
  -- ���˻���
	gz_br_no  	CHAR    ( 10 )          ,
  -- �����ʺ�
	enteracc  	CHAR    ( 32 )          ,
  -- �����ʻ�����
	enteraccnm	CHAR    ( 60 )          ,
  -- �����ʺſ�������
	enteropenbrn	CHAR    ( 10 )          ,
  -- �ֹ���������
	enterdate 	CHAR    (  10 )                ,
  -- �ֹ�������ˮ
	entertrace	INT                     ,
  -- �ֹ�����ƽ̨��ˮ
	enterswno 	INT                     ,
  -- �ֹ�����ƽ̨����
	enterswdate	CHAR    (  10 )                ,
  -- �ֹ����˹�Ա
	entertel  	CHAR    ( 10 )          ,
  -- �ֹ����˹�Ա��ˮ
	entertel_no	INT                     ,
  -- ���ʴ�������
	dealerr_date	CHAR    (  10 )                ,
  -- ���ʴ����Ա
	dealerr_tel	CHAR    ( 10 )          ,
  -- ���ʴ���������ˮ
	dealerr_hostno	INT                     ,
  -- ���ʴ���ƽ̨��ˮ
	dealerr_swno	INT                     ,
  -- ���˴���ƽ̨����
	dealerr_swdate	CHAR    (  10 )                ,
  -- ���˴���ƽ̨��ˮ
	gzdeal_trace_no	INT                     ,
  -- ���˴���ƽ̨����
	gzdeal_trace_date	CHAR    (  10 )                ,
  -- ���˴��������ˮ
	gzdeal_host_no	INT                     ,
  -- ���˴����������
	gzdeal_host_date	CHAR    (  10 )                ,
  -- ��ע
	hv_brf    	CHAR    (270 )          ,
  -- ��ע2
	hv_brf2   	CHAR    ( 10 )          ,
  -- ����
	remark    	CHAR    (270 )          ,
  -- ����2
	remark2   	CHAR    (150 )          ,
  -- ���ʱ��
	chargeoff 	CHAR    ( 20 )          
) ;

CREATE UNIQUE INDEX hv_zf_erdai_IDX ON hv_zf_erdai ( or_br_no,msgno,lw_ind );
CREATE UNIQUE INDEX hv_zf_erdai_IDX2 ON hv_zf_erdai ( or_br_no,wt_date,orderno,lw_ind );
