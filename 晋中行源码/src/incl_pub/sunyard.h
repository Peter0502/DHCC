#define SUNYARD_H_ 172.16.100.157

#define ERR_CONNECT	9001	/*��������*/
#define ERR_SEND		9002	/*���ʹ���*/
#define ERR_RECV		9003	/*���մ���*/
#define ERR_CAL			9004	/*��Ѻ����*/
#define ERR_CHECK		9005	/*��Ѻ����*/
#define ERR_SIGN		9006	/*�˶�ǩ������*/
#define ERR_SENDER		9007	/*�����б�ʶ����*/
#define ERR_RECEIVER	9008	/*�ձ��б�ʶ����*/
#define ERR_EXCEPTION	9009	/*���ܿ������쳣*/
#define ERR_DATA		9010	/*���ݱ��Ĵ���*/
#define ERR_TIMEOUT	9011		/*����ͨ�ų�ʱ*/
#define ERR_PURVIEW	9012	/*�������ޱ�ѺȨ��*/

/* SetParam*/
int SetParam(char *My_IPAdress,int TimeOut);
int SetParamAll(char *My_IPAdress1,char *My_IPAdress2,int TimeOut);

/* CipherTransact */
int CipherTransact(int TransactType,char YwType,char* Date,char* SerialNo,char* Amount,char* SendNode,char* RecvNode,char* OtherInfo,char* CipherValue);

/*��Ѻ��������IP��ַ������ HUIPIAO_I �� HUIPIAO_II ����������*/
/*һ��TCP�����ӣ��Զ�����̨��Ѻ���������ͽ���*/
int DraftEncrypt(int TransactType,char YwType,char* Date,char* SerialNo,char* Amount,char* SendNode,char* RecvNode,char* OtherInfo,char* CipherValue);
