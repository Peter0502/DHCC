
-- С��֧����������
DROP TABLE lv_pkgreg_erdai;
CREATE TABLE lv_pkgreg_erdai
(
  -- ��������
	msgtype   	CHAR    ( 35 ) not null ,
  -- ���׷�������
	channel   	CHAR    ( 10 )          ,
  -- ���׻���
	br_no     	CHAR    ( 10 )          ,
  -- С������
	bepsdate  	CHAR    (  10 )                ,
  -- ����ʱ��yyyymmddhhmmss�������һ��
	tx_time   	CHAR    ( 26 ) not null ,
  -- ί������
	wt_date   	CHAR    (  10 )       not null ,
  -- ֧���������
	orderno   	INT            not null ,
  -- ҵ�����ͱ��
	txtype    	CHAR    (  4 ) not null ,
  -- ҵ������
	txkind    	CHAR    (  5 )          ,
  -- ����
	cur_no    	CHAR    (  3 ) not null ,
  -- ���׽��
	tx_amt    	DECIMAL (17,2) not null ,
  -- ֧ȡ��ʽ
	drawtype  	CHAR    (  4 )          ,
  -- ����/֤������/֧������
	drawval   	CHAR    ( 20 )          ,
  -- �����ѱ�־
	tx_chrg_ind	CHAR    (  1 )          ,
  -- �����ѽ��
	chrg_amt  	DECIMAL (17,2)          ,
  -- ������ʶSR00-��SR01-��
	lw_ind    	CHAR    (  4 ) not null ,
  -- �����־
	cdflag    	CHAR    (  4 ) not null ,
  -- �������к�
	or_br_no  	CHAR    ( 14 ) not null ,
  -- �����������к�
	pay_qs_no 	CHAR    ( 14 ) not null ,
  -- �����˿�����
	pay_opn_br_no	CHAR    ( 14 )          ,
  -- �����˿���������
	pay_opn_br_nm	CHAR    ( 60 )          ,
  -- �������ʺ�
	pay_ac_no 	CHAR    ( 32 ) not null ,
  -- ����������
	pay_name  	CHAR    ( 60 ) not null ,
  -- �����˵�ַ
	pay_addr  	CHAR    ( 60 )          ,
  -- �������к�
	ac_br_no  	CHAR    ( 14 ) not null ,
  -- �����������к�
	cash_qs_no	CHAR    ( 14 ) not null ,
  -- �տ��˿�����
	cash_opn_br_no	CHAR    ( 14 )          ,
  -- �տ��˿���������
	cash_opn_br_nm	CHAR    ( 60 )          ,
  -- �տ����ʺ�
	cash_ac_no	CHAR    ( 32 ) not null ,
  -- �տ�������
	cash_name 	CHAR    ( 60 ) not null ,
  -- �տ��˵�ַ
	cash_addr 	CHAR    ( 60 )          ,
  -- ƾ֤����
	note_type 	CHAR    (  3 )          ,
  -- ƾ֤����
	note_no   	CHAR    ( 16 )          ,
  -- ����״̬
	lv_sts    	CHAR    (  3 ) not null ,
  -- ��ί������
	pack_date 	CHAR    (  10 )                ,
  -- �����
	packno    	INT                     ,
  -- һ������ʶ
	cnapsflag 	CHAR    (  1 )          ,
  -- ҵ��ܾ���
	rejectcode	CHAR    ( 35 )          ,
  -- ҵ��ܾ�ԭ��
	rejectreason	CHAR    (105 )          ,
  -- ��ִ����
	returndays	INT                     ,
  -- ��ִ����
	resp_date 	CHAR    (  10 )                ,
  -- ��ִ֧�����/��ִ�����
	respno    	INT                     ,
  -- ��ִ������
	rsp_pack_date	CHAR    (  10 )                ,
  -- ��ִ�����
	rsp_packno	INT                     ,
  -- ҵ���ִ״̬
	rcpstat   	CHAR    (  4 )          ,
  -- NPCת������
	trandate  	CHAR    (  10 )                ,
  -- �����ʺ�
	accno     	CHAR    ( 32 )          ,
  -- �����˻�����
	accnm     	CHAR    ( 60 )          ,
  -- ��������/ƽ̨����
	tx_date   	CHAR    (  10 )       not null ,
  -- ¼��ƽ̨��ˮ/��������ƽ̨��ˮ
	rcv_trace_no	INT                     ,
  -- ���ļ�������/¼��(��������)
	jz_date   	CHAR    (  10 )                ,
  -- ���ļ�����ˮ/¼��(��������)
	hostno    	INT                     ,
  -- ¼�����Ա��
	input_tel 	CHAR    ( 10 )          ,
  -- ¼���Ա��ˮ
	check_trace_no	INT                     ,
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
  -- �ֹ�����ƽ̨����
	enterptdate	CHAR    (  10 )                ,
  -- �ֹ�����ƽ̨��ˮ/������ʴ���ƽ̨��ˮ
	enterswno 	INT                     ,
  -- �ֹ����˺�������/������ʴ�������
	enterdate 	CHAR    (  10 )                ,
  -- �ֹ����˺�����ˮ/������ʴ�����ˮ
	entertrace	INT                     ,
  -- �ֹ����˹�Ա/������ʴ����Ա
	entertel  	CHAR    ( 10 )          ,
  -- �ֹ����˹�Ա��ˮ
	entertel_no	INT                     ,
  -- ���ʴ���ƽ̨����
	deal_date 	CHAR    (  10 )                ,
  -- ���ʴ���ƽ̨��ˮ
	deal_swno 	INT                     ,
  -- ���˴����������
	deal_mz_date	CHAR    (  10 )                ,
  -- ���ʴ��������ˮ
	deal_trace_no	INT                     ,
  -- ���ʴ������Ա��
	deal_tel  	CHAR    ( 10 )          ,
  -- ���ʴ����Ա��ˮ
	deal_tel_no	INT                     ,
  -- ���˴���ƽ̨��ˮ
	gzdeal_trace_no	INT                     ,
  -- ���˴���ƽ̨����
	gzdeal_trace_date	CHAR    (  10 )                ,
  -- ���˴��������ˮ
	gzdeal_host_no	INT                     ,
  -- ���˴����������
	gzdeal_host_date	CHAR    (  10 )                ,
  -- ���ж��ʼ��״̬
	checkflag 	CHAR    (  1 )          ,
  -- ���ڶ��ʼ��״̬
	hostflag  	CHAR    (  1 )          ,
  -- ��ӡ����
	prt_times 	INT            not null ,
  -- ���˲���Ա��
	check_tel 	CHAR    ( 10 )          ,
  -- ��Ȩ����Ա��
	auth_tel  	CHAR    ( 10 )          ,
  -- ���Ͳ���Ա��
	send_tel  	CHAR    ( 10 )          ,
  -- ��ִ����Ա��
	resp_tel  	CHAR    ( 10 )          ,
  -- �෽Э���
	protno    	CHAR    ( 60 )          ,
  -- �������ȼ�
	mesgpriority	CHAR    (  1 )          ,
  -- ҵ�����
	txno      	INT                     ,
  -- �ʻ�����
	acc_type  	CHAR    (  1 )          ,
  -- ����
	lv_brf    	CHAR    (270 )          ,
  -- ����2
	lv_brf2   	CHAR    (150 )          ,
  -- ��ע
	filler    	CHAR    (270 )          ,
  -- ��ע2
	filler2   	CHAR    ( 10 )          ,
  -- ���ʱ��
	chargeoff 	CHAR    ( 20 )          
) ;

CREATE UNIQUE INDEX lv_pkgreg_erdai_IDX ON lv_pkgreg_erdai ( or_br_no,wt_date,orderno,lw_ind );
