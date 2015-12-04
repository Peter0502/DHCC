#define SUNYARD_H_ 172.16.100.157

#define ERR_CONNECT	9001	/*联机错误*/
#define ERR_SEND		9002	/*发送错误*/
#define ERR_RECV		9003	/*接收错误*/
#define ERR_CAL			9004	/*加押错误*/
#define ERR_CHECK		9005	/*核押错误*/
#define ERR_SIGN		9006	/*核对签名错误*/
#define ERR_SENDER		9007	/*发报行标识错误*/
#define ERR_RECEIVER	9008	/*收报行标识错误*/
#define ERR_EXCEPTION	9009	/*加密卡处理异常*/
#define ERR_DATA		9010	/*数据报文错误*/
#define ERR_TIMEOUT	9011		/*网络通信超时*/
#define ERR_PURVIEW	9012	/*发报行无编押权限*/

/* SetParam*/
int SetParam(char *My_IPAdress,int TimeOut);
int SetParamAll(char *My_IPAdress1,char *My_IPAdress2,int TimeOut);

/* CipherTransact */
int CipherTransact(int TransactType,char YwType,char* Date,char* SerialNo,char* Amount,char* SendNode,char* RecvNode,char* OtherInfo,char* CipherValue);

/*密押服务器的IP地址定义在 HUIPIAO_I 和 HUIPIAO_II 环境变量中*/
/*一个TCP常连接，自动向两台密押服务器发送交易*/
int DraftEncrypt(int TransactType,char YwType,char* Date,char* SerialNo,char* Amount,char* SendNode,char* RecvNode,char* OtherInfo,char* CipherValue);
