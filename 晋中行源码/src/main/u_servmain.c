/******************************************************************************
 *    ����  : u_servmain
 *    ����  ��make -f  mk.tux
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include "apl_incl.h"
#include "public.h"
#include "svrpub_c.h"
#include "tx_def_c.h"
#include "tx_flow_def_c.h"
#include "tx_sub_def_c.h"
#include "tx_sub_rel_c.h"
#include "tx_dc_rel_c.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"
#include "com_def_chrg_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"
#include "com_chrg_hst_c.h"
#include "prdt_ac_id_c.h"
#include "find_debug.h"
char  tp_name[]      =    "u_servmain";
char  tp_backcallname[]="";

extern struct  tx_def_c g_tx_def_c;

short special_server = 0;   /* 0 : ServerName = RegionId + TpName (normal) */
                            /* 1 : ServerName = TpName (special server)    */
short service_type   = 0;   /* 0 : Normal Server */
                            /* 1 : Common Server */

void  u_servmain();
void  cm_table_update_ph2();
void  (*func)() = u_servmain;
void  (*db_service_init)() = NULL;          /* initialize DBSR working area */
void  (*user_poll_call)()  = NULL;          /* userpollcall entry for AP    */
void  (*user_call_back)()  = NULL;          /* nowait request call back     */
void  (*Pre_Server_Call)() = NULL;          /* pre server call entry        */
int  (*ZE_MACTA_TO_TOTA)() = NULL;
int  (*MACTA_Check_Process)() = NULL;
int  (*MACTA_Update_Process)() = NULL;
void  (*common_table_update)() = NULL;
int tx_flow_chk(char *sub_tx_rslt,char *rtcode,long *flow_code);

/******************************************************************************/
void u_servmain()
{
    void * handle;
    void (* myFun)();
    char tx_code[5];
    char libso[50];
	char tmprsp[5];
    struct tx_flow_def_c tx_flow;
    struct tx_sub_def_c tx_sub;
	int vfirst=0;
	char pcCzTlrno[5];
	
	MEMSET_DEBUG(pcCzTlrno,'\0',sizeof(pcCzTlrno));
	/* begin add by LiuHuafeng 20070118 */
	MEMSET_DEBUG(tx_code, 0 , sizeof(tx_code));
	MEMSET_DEBUG(libso, 0 , sizeof(libso));
	MEMSET_DEBUG(tmprsp, 0 , sizeof(tmprsp));
	/* end by LiuHuafeng 20070118 */
	Gl_file_first=1;
    vtcp_log("%s,%d,���״��룺txno[%d]",__FILE__,__LINE__,it_txcom.txno);TRACE
    sprintf(tx_code,"%04d",it_txcom.txno);
	vtcp_log("%s,%d log_ind=[%c],tx_type=[%c]",__FILE__,__LINE__,g_tx_def_c.log_ind[0],g_tx_def_c.tx_type[0]);
	if( g_tx_def_c.log_ind[0]=='1'
		&& g_tx_def_c.tx_type[0]!='4' && g_tx_def_c.tx_type[0]!='5' )
	{
		vtcp_log("%s,%d log_ind=[%c],tx_type=[%c]",__FILE__,__LINE__,g_tx_def_c.log_ind[0],g_tx_def_c.tx_type[0]);
		RespCode = pub_comm_chrg_chk();
		if( RespCode )
		{
			vtcp_log("--need chrg:[%d]--", RespCode );
			goto Exit;
		}
	}
	g_acc_amt[0]=g_acc_amt[1]=0.00;
	/***Add by SSH,2006.11.1,����98��Ա***/
	get_zd_data ( DC_TEL , pcCzTlrno );
	if( memcmp(pcCzTlrno+2,"98",2)==0 && it_txcom.txno<8000){
		if(it_txcom.txno!=5103 &&
			it_txcom.txno!=5104 &&	
			it_txcom.txno!=5106 &&	
			it_txcom.txno!=5107 &&	
			it_txcom.txno!=5108 &&	
			it_txcom.txno!=5109 &&	
			it_txcom.txno!=5110 &&	
			it_txcom.txno!=5111 &&	
			it_txcom.txno!=5112 &&	
			it_txcom.txno!=5201 &&	
			it_txcom.txno!=5202 &&	
			it_txcom.txno!=5203 &&	
			it_txcom.txno!=5204 &&
			it_txcom.txno!=5502 &&
			it_txcom.txno!=5501 &&
			it_txcom.txno!=5505 &&
			it_txcom.txno!=5603 &&
			it_txcom.txno!=5605 &&
			it_txcom.txno!=5608 &&
			it_txcom.txno!=5610 &&
			it_txcom.txno!=5611 &&
			it_txcom.txno!=5101 &&
			it_txcom.txno!=5114 &&
			it_txcom.txno!=7774 &&
			it_txcom.txno!=7704 &&
			it_txcom.txno!=5701 &&
			it_txcom.txno!=5102
			){	
				strcpy ( RtCode , "Z10B" );
				set_zd_data ( DC_REPLY , RtCode );
				vtcp_log("%s,%d,���״��룺98��Ա[%s][%d]Ȩ�޲���",__FILE__,__LINE__,g_pub_tx.tel,it_txcom.txno);
				goto Exit;
		}
	}

	/***���ȵ�����Ȩ����ӽ���***/
	if( g_tx_def_c.log_ind[0]=='1' && 
		g_tx_def_c.tx_type[0]!='4' && strcmp(g_tx_def_c.tx_code,"9800") )
	{
		strcpy( tx_flow.tx_code,tx_code );
		tx_flow.flow_code=-1;
		strcpy( tx_flow.sub_tx_code,"SQ01" );
		strcpy( tx_flow.sub_tx_rslt,"#=0000@0@#$" );
		vfirst=0;
	}
	else
	{
       	RespCode=Tx_flow_def_Sel(RtCode,&tx_flow,
            "tx_code='%s' AND flow_code=0 ",tx_code);
		if ( RespCode )
		{
			strcpy ( RtCode , "S025" );
			set_zd_data ( DC_REPLY , RtCode );
			vtcp_log("%s,%d,���״��룺txno[%d]δ���壡",__FILE__,__LINE__,it_txcom.txno);
			goto Exit;
		}
	}

vtcp_log("[%s][%d] set time",__FILE__,__LINE__);
pub_base_wrt_log_bg(); /****add zgf 20131021 ?**/
    while(1)
    {
        vtcp_log("u_servmain's is code[%s]",tx_flow.sub_tx_code);TRACE
	

        RespCode=Tx_sub_def_Sel(RtCode,&tx_sub,
            "sub_tx_code='%s'",tx_flow.sub_tx_code);
        if ( RespCode )
        {
            strcpy ( RtCode , "S026" );
            set_zd_data ( DC_REPLY , RtCode );
            vtcp_log("�ӽ��״��룺txno[%s]δ���壡",tx_flow.sub_tx_code);
            return ;
        }

		pub_base_strpack( tx_sub.tx_func );
vtcp_log("run sub_tx[%s] [%s] [%s]",tx_sub.sub_tx_code,tx_sub.sub_tx_name,tx_sub.tx_func );

			if ( tx_sub_init(tx_code,tx_flow.sub_tx_code) )
			{
				goto Exit;
			}

		set_zd_data(DC_SUB_TX_CODE,tx_flow.sub_tx_code);
		if( strcmp(tx_flow.sub_tx_code,"NULL") )
		{
        	sprintf( libso,"%s/lib/",getenv("HOME") );
        	ldchar( tx_sub.tx_func,strlen(tx_sub.tx_func),tx_sub.tx_func );
        	pub_base_strpack ( libso );
        	strcat ( libso , tx_sub.tx_func);
        	libso[strlen(libso)]=0;
        	ldchar(libso,strlen(libso),libso);
        	strcat(libso,".so");
            vtcp_log("libso-name1:[%s] [%s]",libso,tx_sub.tx_func);

        	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
        	if(handle == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log(">>----------�򿪴���");
            	goto Exit ;
        	}
		
            	vtcp_log("%s,%d,Funname=[%s]",__FILE__,__LINE__,tx_sub.tx_func);
        	myFun = (void(*)(int))dlsym(handle,tx_sub.tx_func);
        	if (myFun == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("���ô���");
            	dlerror();
            	goto Exit ;
        	}
        	else (*myFun)();
            	vtcp_log("���ú���������[%s][%d]",__FILE__,__LINE__);
		

        	if(dlclose(handle))
        	{
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("�رմ���");
            	goto Exit ;
        	}
		}
        get_zd_data(DC_REPLY,RtCode);

       vtcp_log("[%s][%d]����[%s]out[%s]",__FILE__,__LINE__,tx_sub.sub_tx_code,RtCode);

		if( !strncmp(RtCode,"000",3) ) /**��Ӧ��000X��ͬ�ɹ�***/
		{
			strcpy( tmprsp,RtCode );
			if ( tx_sub.acct_ind[0]=='1' )
			{
				if ( pubf_acct_proc(tx_sub.sub_tx_code) )
				{
					vtcp_log("���û�ƽӿڳ������");
            		set_zd_data ( DC_REPLY , RtCode );
					goto Exit;
				}
			}
			strcpy( RtCode,tmprsp );
		}

       vtcp_log("[%s][%d]����[%s]out[%s]",__FILE__,__LINE__,tx_sub.sub_tx_code,RtCode);

		if( !vfirst )
		{
        	if ( tx_flow_chk ( tx_flow.sub_tx_rslt,RtCode,&tx_flow.flow_code ) )
        	{
            	set_zd_data(DC_REPLY,"S028");
            	vtcp_log("�������̶������");
            	goto Exit;
        	}
		}

		if ( tx_flow.flow_code!=9999 )
		{
        	RespCode=Tx_flow_def_Sel(RtCode,&tx_flow,
            	"tx_code='%s' AND flow_code=%d",tx_code,tx_flow.flow_code);
        	if ( RespCode )
        	{
            	strcpy ( RtCode , "S025" );
            	set_zd_data ( DC_REPLY , RtCode );
            	vtcp_log("�������̴���[%d][%d]δ����",
                	it_txcom.txno,tx_flow.flow_code);
            	goto Exit;
        	}
			if ( tx_sub_init(tx_code,tx_flow.sub_tx_code) )
			{
				goto Exit;
			}
		}
        else
        {
            get_zd_data ( DC_REPLY , RtCode );
vtcp_log("DC_REPLY[%s][%d]",RtCode,vfirst);
			if( g_tx_def_c.log_ind[0]=='1' && 
				g_tx_def_c.tx_type[0]!='4' && strcmp(g_tx_def_c.tx_code,"9800") &&
				!strcmp(RtCode,"0000") && vfirst==0 )
			{
vtcp_log("dc_REPLY[%s]",RtCode);
				/***���ȵ�����Ȩ����ӽ���***/
				strcpy( tx_flow.tx_code,tx_code );
				tx_flow.flow_code=9999;
				strcpy( tx_flow.sub_tx_code,"SQ99" );
				strcpy( tx_flow.sub_tx_rslt,"#=0000@0@#$" );
				vfirst=1;
				continue;
			}
			else if( !strcmp(RtCode,"0000") )
			{
vtcp_log("Dc_REPLY[%s]",RtCode);
                vtcp_log("%s,%d�շ� ����ʼ",__FILE__,__LINE__ );
				RespCode = pub_comm_chrg_proc();
				if( RespCode )
				{
					vtcp_log("%s,%d �շѴ���"__FILE__,__LINE__);
					set_zd_data ( DC_REPLY , RtCode );
				}
			}
            vtcp_log("[%s][%d]����ִ�н�����",__FILE__,__LINE__);
			set_zd_data("0093","0");
            goto Exit;
        }
    }
Exit:
	sprintf(acErrMsg,"[%s][%d]D[%.2lf]C[%.2lf]==E%.2lfE",tx_code,g_pub_tx.trace_no,g_acc_amt[0],g_acc_amt[1],g_acc_amt[0]-g_acc_amt[1]);
	WRITEMSG
	pub_reg_rec_file(acErrMsg);
    /*if( strncmp(tx_code,"9988",4) ) pub_base_wrt_log(); */
    /*if( strncmp(tx_code,"9987",4) && strncmp(tx_code,"9986",4) ) */
		pub_base_wrt_log();
    return ;
}
static tx_flow_chk(char *sub_tx_rslt,char *rtcode,long *flow_code)
{
    char *p,*q;
    char tmpcode[5];
    char vrtcode[5];
    char tmp_syn[2];
 /*   char *val; */
	char val[201];
    *flow_code=0;
    p=sub_tx_rslt;
			strcpy( vrtcode,"0000" );
			if( strlen(rtcode) ) 
				strncpy( vrtcode+4-strlen(rtcode),rtcode,strlen(rtcode) );
			else
				strcpy( vrtcode,rtcode );
				
    while(1)
    {
        q=strchr(p,'#');
        if ( q==NULL )
        {
            vtcp_log("�������̽��ʹ���1");
            return -1;
        }
        *q=0;
        *q++;
        tmp_syn[0]=q[0];
        tmp_syn[1]='\0';
        if ( tmp_syn[0]=='$' )
        {
            *flow_code=9999;
            break;
        }
        *q++;
        strncpy ( tmpcode , q , 4 );
		tmpcode[4]=0;
        p=q;
        q=strchr(p,'@');
        if ( q==NULL )
        {
            vtcp_log("�������̽��ʹ���2");
            return -1;
        }
        *q=0;
        *q++;
        sscanf( q, "%s", val );
        q=strchr(val,'@');
        if ( q==NULL )
        {
            vtcp_log("�������̽��ʹ���2");
            return -1;
        }
        *q=0;
        *q++;
        p=q;
        if ( tmp_syn[0]=='=' )
        {
vtcp_log("######>[%s] ?= [%s]",tmpcode ,vrtcode );
            if ( !strncmp ( tmpcode , vrtcode , 4 ) )
            {
                *flow_code=atoi(val);
                break;
            }
        }
        else
        {
            if ( strncmp ( tmpcode , vrtcode , 4 ) )
            {
                *flow_code=atoi(val);
                break;
            }
        }
    }

    return 0;
}
static tx_sub_init(char *tx_code,char *in_sub_tx)
{
	struct tx_sub_rel_c sub_rel;
	char *data;
	int i;

	set_zd_data("0191","0"); /**initxrb **/

	RespCode=Tx_sub_rel_Dec_Sel(RtCode,
		"tx_code='%s' and in_sub_tx='%s'",tx_code,in_sub_tx);
	if ( RespCode )
	{
		vtcp_log("ȡ�ӽ��׹�ϵ����[%d]",RespCode);
		return 1;
	}
	while(1)
	{
		MEMSET_DEBUG(&sub_rel,'\0',sizeof(sub_rel));
		RespCode=Tx_sub_rel_Fet_Sel(&sub_rel,RtCode);
		if ( RespCode )
		{
			strcpy ( RtCode , "0000" );
			break;
		}
		
		if( sub_rel.out_ind[0]=='9' ) /**������**/
			continue;

		pub_base_strpack( sub_rel.in_data );
		pub_base_strpack( sub_rel.out_data );
vtcp_log("[%s]--------[%s]",sub_rel.in_data,sub_rel.out_data );

		if( tx_sub_out_data(sub_rel.out_data,sub_rel.out_ind,
			sub_rel.in_data ) )
		{
			vtcp_log("1�ӽ��׹�ϵ���ݶ������[%s]",sub_rel.out_data );
			return 1;
		}
	}
	Tx_sub_rel_Clo_Sel();
	return 0;
}
int tx_sub_out_data( char * out_data,char * out_ind,char * in_data )
{
	int i,j,len;
	char data[2048];
	double vdbl;
	char vcdbl[30];

	MEMSET_DEBUG( data,0,sizeof(data));

		j = pub_base_GetDataCode( in_data );
		if ( j<0 )
		{
			vtcp_log("2�ӽ��׹�ϵ���ݶ������[%s]",in_data );
			goto ErrExit;
		}
	MEMSET_DEBUG( data,' ',Pu_Val[j].length);

	len=strlen( out_data );
	if( out_ind[0]=='1' ) /**����**/
	{
		MEMCPY_DEBUG( data,out_data,len );
	}
	else /**0���ݱ���**/
	{
		if( len>4 ) /**��ʽ�����Խ������Ч**/
		{
			if( pub_acct_dc_get_amt( out_data,&vdbl ) )
			{
				vtcp_log("[%s]�ӽ��׹�ϵ���ݶ������[%s]",__FILE__,out_data );
				goto ErrExit;
			}
			else
			{
				sprintf( vcdbl,"%.2lf",vdbl );
				len=strlen(vcdbl);
				MEMCPY_DEBUG( data,vcdbl,len );
			}
		}
		else if( len==4 )/**��Ϊ�ǵ�һ���ݱ���**/
		{
			i = pub_base_GetDataCode( out_data );
			if ( i<0 )
			{
				vtcp_log("[%s]�ӽ��׹�ϵ���ݶ������[%s]",__FILE__,out_data );
				goto ErrExit;
			}
			else
			{
				pub_base_Nctoc((char*)Pu_Val[i].val,data,
					Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
			}
		}
		else
		{
			vtcp_log("[%s]�ӽ��׹�ϵ���ݶ������[%s]",__FILE__,out_data );
			goto ErrExit;
		}
	}

		pub_base_Ctonc(data,(char *)Pu_Val[j].val,
			Pu_Val[j].lx,Pu_Val[j].length,Pu_Val[j].prec);
vtcp_log("==[%s]==>[%s]++++[%s]",out_data,in_data,data);

	return 0;
ErrExit:
	vtcp_log("error send");
	return 1;
}
pub_comm_chrg_procD()
{
    void * handle;
	char ind[2];
	int i=0;
	double tmpdbl;
	char tmpstr[100];
    void (* myFun)();
    char tx_code[5];
    char libso[50];
	char txsub[21];
	char dc_code[10];

	get_zd_data( "0093",ind );
vtcp_log(" GET 0093 [%s]",ind );

	if( ind[0]=='0' ) 
		return 0;

	for( i=0;i<3;i++ )
	{
		if( i==0 )
		{
			get_zd_data("1228",dc_code);
			get_zd_double("1229",&tmpdbl);
		}
		else if( i==1 )
		{
			get_zd_data("122A",dc_code);
			get_zd_double("122B",&tmpdbl);
			set_zd_data("1228",dc_code);
			set_zd_double("1229",tmpdbl);
		}
		else if( i==2 )
		{
			get_zd_data("122C",dc_code);
			get_zd_double("122D",&tmpdbl);
			set_zd_data("1228",dc_code);
			set_zd_double("1229",tmpdbl);
		}
		
		if( tmpdbl<=0.005 ) continue;

		vtcp_log("�շ��ӽ���[%s][%d]",dc_code,tmpdbl);

		/**�շ��ӽ���**/
		set_zd_data(DC_SUB_TX_CODE,"D108");
        	sprintf( libso,"%s/lib/spD108.so",getenv("HOME") );
			strcpy( txsub,"spD108" );
        	vtcp_log("libso-name chrg:[%s] [%s]",libso);

        	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
        	if(handle == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("�򿪴���");
            	goto Exit ;
        	}

        	myFun = (void(*)(int))dlsym(handle,txsub);
        	if (myFun == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("���ô���");
            	dlerror();
            	goto Exit ;
        	}
        	else (*myFun)();

        	if(dlclose(handle))
        	{
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("�رմ���");
            	goto Exit ;
        	}
		/**ȡ���ӽ���**/
		if( ind[0]=='2' ) 
		{
			set_zd_data(DC_SUB_TX_CODE,"D003");
			get_zd_data("1221",tmpstr); /*�˺�*/
			set_zd_data("1021",tmpstr);
			set_zd_double("102F",tmpdbl);
			set_zd_data("0191","1");
			set_zd_data("0192","0000000");
			set_zd_data("0193","1001");
        	sprintf( libso,"%s/lib/spD003.so",getenv("HOME") );
			strcpy( txsub,"spD003" );
        	vtcp_log("libso-name chrg:[%s] [%s]",libso);
		}
		else
		{
			set_zd_data("1181","01"); /*����*/
			set_zd_double("1184",tmpdbl);
			set_zd_data(DC_SUB_TX_CODE,"M001");
        	sprintf( libso,"%s/lib/spM001.so",getenv("HOME") );
			strcpy( txsub,"spM001" );
        	vtcp_log("libso-name chrg:[%s] [%s]",libso);
		}

        	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
        	if(handle == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("�򿪴���");
            	goto Exit ;
        	}

        	myFun = (void(*)(int))dlsym(handle,txsub);
        	if (myFun == NULL)
        	{
            	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("���ô���");
            	dlerror();
            	goto Exit ;
        	}
        	else (*myFun)();

        	if(dlclose(handle))
        	{
            	set_zd_data(DC_REPLY,"S027");
            	vtcp_log("�رմ���");
            	goto Exit ;
        	}
	}
    return 0;
Exit:
	return 1;
}
/**������ȡ����**/
int pub_comm_chrg_proc()
{
	int ret=0;
	double	amt,chrg_amt;
	double	xj_amt,zz_amt,kx_amt;
	char vtx_code[5];
	struct prdt_ac_id_c prdt_ac_id;
	struct	com_def_chrg_c	def_chrg;
	struct	mo_def_rate_c	mo_def_rate;
	struct	com_chrg_rate_c	com_chrg_rate;
	struct	dc_acc_rel_c	dc_acc_rel;
	struct	com_chrg_hst_c	com_chrg_hst;
	struct	mdm_ac_rel_c	vmdm_ac_rel;
	char filename[100];
	char tmpstr[100];
	char ac_name[61];
	int need=0;
	char ind[2];
	int i=0;
	FILE *fp;
	int fs=0;
	char mode[11];
	long cif_no;
	char name[51];
	int iii=0;
	char tmpacno[32];
	char ac_no[20];
	char acc_no[10];
	int rrtt=0;
	/* begin add by Liuhuafeng 20080111 */
	char cTmp_ac_no[20];
	memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
	memset(ac_no, 0 , sizeof(ac_no));
	memset(ac_name, 0 , sizeof(ac_name));
	/* end  by Liuhuafeng 20080111 */

	fs=0;
	/* begin add by LiuHuafeng 20070208 */
	memset(tmpstr, 0 , sizeof(tmpstr));
	/* end by LiuHuafeng 20070208 */

	get_zd_data( "0093",ind );
	vtcp_log("%s,%d ind=[%s]",__FILE__,__LINE__,ind);
	set_zd_data( "1225",ind );
vtcp_log("%s,%d GET 0093 [%s]",__FILE__,__LINE__,ind );

	if( ind[0]=='1' ) 
		set_zd_data("1241","�ֽ�");
	else if( ind[0]=='2' ) 
		set_zd_data("1241","ת��");
	else 
		return 0;

	get_zd_data( DC_TX_CODE,vtx_code );

	xj_amt=zz_amt=kx_amt=0.00;

	vtcp_log( "TJ:tx_code='%s' order by seqn",vtx_code );

	ret=Com_def_chrg_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by seqn",vtx_code );
	if( ret )
	{
		goto ErrExit;
	}

	while( 1 )
	{
		ret=Com_def_chrg_Fet_Sel( &def_chrg,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack( def_chrg.amt_no );
		pub_base_strpack( def_chrg.mo );

		ret=pubf_def_chrg_proc( def_chrg.chrg_rate_code,
			mo_def_rate.chrg_rate_no,def_chrg.amt_no,&amt );
		vtcp_log("%s,%d out from pubf_def_chrg_proc ret=[%d]",__FILE__,__LINE__,ret);
		if( ret ) 
		{
			vtcp_log("%s,%d ����pubf_def_chrg_proc ����",__FILE__,__LINE__);
			goto ErrExit;
		}

		ret=pubf_acct_chrg_proc( mo_def_rate.chrg_rate_no,
			com_chrg_rate.dc_code,&chrg_amt,amt );
		if( ret ) 
		{
			vtcp_log("%s,%d ����pubf_def_chrg_proc ����",__FILE__,__LINE__);
			goto ErrExit;
		}
vtcp_log("%s,%d CHRG_AMT[%.2lf][%.2lf]",__FILE__,__LINE__,chrg_amt,amt);
vtcp_log("%s,%d NOTE txcode=[%d],tmpstr=[%s]",__FILE__,__LINE__,it_txcom.txno,tmpstr);
		if( chrg_amt>0.005 ) 
		{
			vtcp_log("%s,%d NOTE txcode=[%d],tmpstr=[%s]",__FILE__,__LINE__,it_txcom.txno,tmpstr);
			if( i==0 )
			{
				get_zd_data(def_chrg.chrg_ac_no,tmpstr);

				/***�˺�***/
			}
			vtcp_log("%s,%d NOTE txcode=[%d],tmpstr=[%s]",__FILE__,__LINE__,it_txcom.txno,tmpstr);
			/* begin add by LiuHuafeng 20070206 */
			{
				get_zd_data(def_chrg.chrg_ac_no,tmpstr);
				vtcp_log("%s,%d txcode=[%d],tmpstr=[%s]",__FILE__,__LINE__,it_txcom.txno,tmpstr);
				/**����������**/
				if( pub_comm_auth_condchk(def_chrg.cond) )
				{
					continue;
				}
			}
			/* end by LiuHuafeng 20070206 */

vtcp_log("%s,%d charge to do[%s] [%.2lf]",__FILE__,__LINE__,tmpstr,chrg_amt);
			need++;

			i++;
			
			/*****������*******/
			ret=Dc_acc_rel_Sel(RtCode,&dc_acc_rel,
				"dc_code='%s' and data_code='0152' ",com_chrg_rate.dc_code);
			if( ret ) 
			{
				sprintf(acErrMsg,"ȡ���ÿ�Ŀdc_code'%s'",com_chrg_rate.dc_code);
				WRITEMSG
				goto ErrExit;
			}
TRACE
			MEMSET_DEBUG( &g_pub_tx,0,sizeof(g_pub_tx) );
			set_zd_data(DC_SUB_TX_CODE,"SF00");
			pub_base_sysinit();
			{/****TODO****/
				struct com_item_c v_com_item;
				MEMSET_DEBUG(&v_com_item,'\0',sizeof(v_com_item));
				if(Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_hrt='%s'",dc_acc_rel.acc_hrt)<0){
					sprintf(acErrMsg,"ȡ���ÿ�Ŀ'%s'����",dc_acc_rel.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				strcpy(g_pub_tx.ac_no,v_com_item.acc_no);
			}
			/**
			strcpy( g_pub_tx.ac_no,dc_acc_rel.acc_hrt );
			**/
			/**** ƾ֤ע���˷� �Ǹ��� wudawei 20140806
			g_pub_tx.tx_amt1=chrg_amt;
			set_zd_double("1226",chrg_amt);
			**** wudawei end 20140806   */
			/*wudawei 20140806 begin */
			if(strcmp(g_pub_tx.tx_code,"5111")==0)
			{
				char no_type[4];
				
				get_zd_data( "0890", no_type );
				get_zd_data( "0250", ac_name );				
				get_zd_data( "0310", ac_no );
				vtcp_log("%s,%d no_type=[%s],ac_name=[%s],ac_no=[%s]",__FILE__,__LINE__,no_type,ac_name,ac_no);
																																																					/* ����֧���������˻���
																																																					1�����������ҽ�Ʊ��չ���������ģ��˺ţ�5009835900016��5009835900024��5009835900032��5009835900040��5009835900057��
																																																					2�������в����֣��˺ţ�5009851600011��5009851600029��
																																																					3���������Ͷ�����ᱣ�Ͼ֣��˺ţ�5013971600018��
																																																					4������������ҽԺ���˺ţ�5009917500031��
																																																					5�������л�ƺ������ģ��˺ţ�5015653800014��
																																																					Ϊ�г������Ĺ���ƾ֤ʱ������õĿͻ�,��ƾ֤���˷ѡ�
																																																					  */
				if((strcmp(no_type,"001")==0 ||  strcmp(no_type,"002")==0) && strncmp(ac_name,"��������",8)!=0 && strncmp(ac_name,"��������ҵ����",14)!=0 && strncmp(ac_no,"5009835900016",13)!=0 && strncmp(ac_no,"5009835900024",13)!=0 && strncmp(ac_no,"5009835900032",13)!=0 && strncmp(ac_no,"5009835900040",13)!=0 && strncmp(ac_no,"5009835900057",13)!=0 && strncmp(ac_no,"5009851600011",13)!=0&& strncmp(ac_no,"5009851600029",13)!=0&& strncmp(ac_no,"5013971600018",13)!=0&& strncmp(ac_no,"5009917500031",13)!=0 && strncmp(ac_no,"5013917900019",13)!=0  && strncmp(ac_no,"5015653800014",13)!=0)
				{
					vtcp_log("%s,%d 1.�ߵ�if�� ��Ҫ�˷�",__FILE__,__LINE__);
					long lCount=0;
					get_zd_long( "0520", &lCount );				/* ƾ֤���� */
			    g_pub_tx.tx_amt1=-chrg_amt*lCount;
			    set_zd_double("1226",g_pub_tx.tx_amt1);
			  }else
			  {
					vtcp_log("%s,%d 1.�ߵ�else�� ����Ҫ�˷�",__FILE__,__LINE__);
			  	g_pub_tx.tx_amt1 = 0;
			    set_zd_double("1226",g_pub_tx.tx_amt1);
			  }
			}
			else
			{
				g_pub_tx.tx_amt1=chrg_amt;
				set_zd_double("1226",chrg_amt);
			}
			/* wudawei 20140806 end  */
			strcpy( g_pub_tx.brf,def_chrg.mo );
			strcpy( g_pub_tx.ct_ind,ind );
			if( def_chrg.chrg_mode[0]=='A' )
				strcpy( g_pub_tx.ct_ind,"1" );
			else if( def_chrg.chrg_mode[0]=='B' )
				strcpy( g_pub_tx.ct_ind,"2" );
			strcpy( g_pub_tx.cur_no,"01" );
			strcpy( g_pub_tx.ac_id_type,"9" ); 		/*�˻�����Ϊ�ڲ�*/
			strcpy( g_pub_tx.ac_wrk_ind,"000" );  	/*���˶�ƾ֤�ţ��������ˮ����ϸ*/
			strcpy(g_pub_tx.add_ind,"1");/*������־*/
			g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
			strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
			vtcp_log("%s,%d ������������ into pub_acct_trance",__FILE__,__LINE__);
			ret=pub_acct_trance();
			if( ret ) 
			{
				vtcp_log("%s,%d ������������ʧ��",__FILE__,__LINE__);
				goto ErrExit;
			}
			vtcp_log("%s,%d out from function pub_acct_trance",__FILE__,__LINE__);
			ret=pubf_acct_proc("SF00");
			if( ret ) 
			{
				vtcp_log("%s,%d function pub_acct_proc error ",__FILE__,__LINE__);
				goto ErrExit;
			}

			MEMSET_DEBUG( &com_chrg_hst,0,sizeof(com_chrg_hst) );
			com_chrg_hst.tx_date =g_pub_tx.tx_date;
			get_zd_long( DC_TRACE_NO,&com_chrg_hst.trace_no );
			strcpy( com_chrg_hst.ac_no,tmpstr );
			vtcp_log("%s,%d ind=[%s]",__FILE__,__LINE__,ind);

		/**/
				pub_base_strpack(com_chrg_hst.ac_no);
vtcp_log("acno[%s]",com_chrg_hst.ac_no);
			strcpy( name,"" );
		/* begin add by LiuHuafeng 20080111 */
		memcpy(cTmp_ac_no,com_chrg_hst.ac_no,sizeof(cTmp_ac_no)-1);
		pub_base_strpack(cTmp_ac_no);
		/* end by LiuHuafeng 20080111 */
		/* modify by LiuHuafeng 20080111 */
		if(memcmp(com_chrg_hst.ac_no,"10101",5) || (!memcmp(com_chrg_hst.ac_no, "10101", 5) && strlen(cTmp_ac_no)>7 ))
		/* end by LiuHuafeng 20080111 */
		{
			/* begin add by LiuHuafeng 20070206 */
			if(strlen(com_chrg_hst.ac_no)==5 || strlen(com_chrg_hst.ac_no)==7)
			{
			}
			else
			/* end by LiuHuafeng 20070206 */
			{
					rrtt=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_no='%s'",com_chrg_hst.ac_no);
					if( rrtt ) {
						vtcp_log("%s,%d Mdm_ac_rel_Sel error ",__FILE__,__LINE__);
						vtcp_log("%s,%d ac_no=[%s] ",__FILE__,__LINE__,com_chrg_hst.ac_no);
						goto ErrExit;
					}

					rrtt=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id,"ac_id=%d",vmdm_ac_rel.ac_id);
					if( rrtt ) 
					{
						vtcp_log("%s,%d Prdt_ac_id_Sel error ",__FILE__,__LINE__);
						vtcp_log("%s,%d ac_id=[%d] ",__FILE__,__LINE__,vmdm_ac_rel.ac_id);
						goto ErrExit;
					}
					{
						if( pub_base_acgetname_mst( prdt_ac_id.ac_id,prdt_ac_id.ac_seqn
							,name ) )
								strcpy( name,"" );

						if(  pub_base_prdt_accno( prdt_ac_id.prdt_no,acc_no ) )
								strcpy( acc_no,"" );
					}
			}
			acc_no[5]='\0';

				MEMSET_DEBUG( tmpacno,0,sizeof(tmpacno) );
				MEMSET_DEBUG( tmpacno,' ',24 );
				strncpy( tmpacno,com_chrg_hst.ac_no,strlen(com_chrg_hst.ac_no));
				strncpy( tmpacno+19,acc_no,5 );
vtcp_log("tmpacno[%s][%d]",tmpacno,strlen(tmpacno));
				set_zd_data( "1242",tmpacno );
		/***/
				/*set_zd_data( "1242",com_chrg_hst.ac_no );*/
				/**
				if( pub_base_acgetname(com_chrg_hst.ac_no,&cif_no,name) )
					strcpy( name,"" );
				***/
				set_zd_data( "1249",name );
			get_zd_data( DC_TX_CODE,com_chrg_hst.tx_code );
			strcpy( com_chrg_hst.chrg_rate_code,mo_def_rate.chrg_rate_no );
			strcpy( com_chrg_hst.name,def_chrg.mo );
			com_chrg_hst.chrg_amt=chrg_amt;
			com_chrg_hst.tx_amt=amt;
			strcpy( com_chrg_hst.sts,"Y" );
			vtcp_log("JYW[%f][%f]",com_chrg_hst.chrg_amt,com_chrg_hst.tx_amt);				

			if( Com_chrg_hst_Ins( com_chrg_hst,g_pub_tx.reply ) )
			{
				vtcp_log("%s,%d function Com_chrg_hst_Ins error",__FILE__,__LINE__);
				goto ErrExit;
			}
		}
		else {
			/** begin add by LiuHuafeng 20061105 */
			if(ind[0]=='2'){
				/* ת�ʷ�ʽ�ʹ��� */
				strncpy( g_pub_tx.reply,"P087" ,4);
				set_zd_data("0130","�ֽ�����ѡ���ֽ�ʽ");
				vtcp_log("%s,%d �ֽ�����ѡ���ֽ�ʽ",__FILE__,__LINE__);
				goto ErrExit;
			}
			/* end by LiuHuafeng 20061105 */
		}

TRACE
			vtcp_log("%s,%d ind=[%s]",__FILE__,__LINE__,ind);
			/*****֧ȡ�ͻ���*****/
			if( ind[0]=='2' )
			{
				MEMSET_DEBUG( &g_pub_tx,0,sizeof(g_pub_tx) );
				set_zd_data(DC_SUB_TX_CODE,"SF02");
				pub_base_sysinit();
				strcpy( g_pub_tx.ac_no,tmpstr );
				/*** ƾ֤ע���˷�  �Ǹ���  wudawei  20140806 begin
				g_pub_tx.tx_amt1=chrg_amt;
				wudawei  20140806 end****/
				if(strcmp(g_pub_tx.tx_code,"5111")==0)
				{
						char no_type[4];
						get_zd_data( "0890", no_type );
						vtcp_log("%s,%d no_type=[%s]",__FILE__,__LINE__,no_type);
						if((strcmp(no_type,"001")==0 ||  strcmp(no_type,"002")==0) && strncmp(ac_name,"��������",8)!=0 && strncmp(ac_name,"��������ҵ����",14)!=0 && strncmp(ac_no,"5009835900016",13)!=0 && strncmp(ac_no,"5009835900024",13)!=0 && strncmp(ac_no,"5009835900032",13)!=0 && strncmp(ac_no,"5009835900040",13)!=0 && strncmp(ac_no,"5009835900057",13)!=0 && strncmp(ac_no,"5009851600011",13)!=0&& strncmp(ac_no,"5009851600029",13)!=0&& strncmp(ac_no,"5013971600018",13)!=0&& strncmp(ac_no,"5009917500031",13)!=0 && strncmp(ac_no,"5013917900019",13)!=0  && strncmp(ac_no,"5015653800014",13)!=0)
						{
							    vtcp_log("%s,%d 2.�ߵ�if�� ��Ҫ�˷�",__FILE__,__LINE__);
								long lCount1=0;
								get_zd_long( "0520", &lCount1 );				/* ƾ֤���� */
						        g_pub_tx.tx_amt1=-chrg_amt*lCount1;
				 		}else
				 		{
								vtcp_log("%s,%d 2.�ߵ�else�� ����Ҫ�˷�",__FILE__,__LINE__);
				 				g_pub_tx.tx_amt1 = 0;
				 		}
				}
				else
				{
						g_pub_tx.tx_amt1=chrg_amt;
				}
				strcpy( g_pub_tx.brf,def_chrg.mo );
				strcpy( g_pub_tx.ct_ind,ind );
				if( def_chrg.chrg_mode[0]=='A' )
					strcpy( g_pub_tx.ct_ind,"1" );
				else if( def_chrg.chrg_mode[0]=='B' )
					strcpy( g_pub_tx.ct_ind,"2" );
				strcpy( g_pub_tx.cur_no,"01" );
				strcpy( g_pub_tx.note_type,"150" );
				strcpy(g_pub_tx.add_ind,"0"); /**����**/
					strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
					strcpy(g_pub_tx.ac_wrk_ind,"000000");
					g_pub_tx.svc_ind=1001;
				ret=pub_acct_trance();
				if( ret )
				{
					vtcp_log("%s,%d pub_acct_trance err",__FILE__,__LINE__);
					 goto ErrExit;

				}
				ret=pubf_acct_proc("SF02");
				if( ret ) {
					vtcp_log("%s,%d pub_acct_proc err",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
			/*****��ȡ�ֽ�*****/
			else if( ind[0]=='1' )
			{
				MEMSET_DEBUG( &g_pub_tx,0,sizeof(g_pub_tx) );
				set_zd_data(DC_SUB_TX_CODE,"SF01");
				pub_base_sysinit();
				g_pub_tx.tx_amt1=chrg_amt;
				strcpy( g_pub_tx.brf,def_chrg.mo );
				strcpy( g_pub_tx.cur_no,"01" );
				strcpy( g_pub_tx.add_ind, "1" );    /**�ֽ�����**/
				strcpy( g_pub_tx.ct_ind, "1" );     /**��ת��־**/
				strcpy(g_pub_tx.hst_ind,"1");
					g_pub_tx.svc_ind=0;
				ret=pub_acct_cash();
				if( ret )
				{
					vtcp_log("%s,%d pub_acct_cash err",__FILE__,__LINE__);
					 goto ErrExit;
				}

				ret=pubf_acct_proc("SF01");
				if( ret ) {
					vtcp_log("%s,%d pub_acct_proc err",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
TRACE
			fs++;
			iii++;
			if( iii==1 )
			{
				set_zd_data("1243",def_chrg.mo);
				set_zd_double("1244",chrg_amt);
			}
			if( iii==2 )
			{
				set_zd_data("1245",def_chrg.mo);
				set_zd_double("1246",chrg_amt);
			}
			if( iii==3 )
			{
				set_zd_data("1247",def_chrg.mo);
				set_zd_double("1248",chrg_amt);
			}
			kx_amt+=chrg_amt;
		}
TRACE
	}
	Com_def_chrg_Clo_Sel();

	if( iii ) 
		pub_set_trad_bit_one(124-1);

TRACE

OkExit:
	sprintf(acErrMsg,"�շѼ���ӽ��׳ɹ�!");
	WRITEMSG
	/****Modified by SSH,2006.8.22,���û��****/
	/****
	get_zd_data( "0093","0" );
	****/
	strcpy( RtCode,"0000" );
	set_zd_data(DC_REPLY,RtCode);
	return 0;
ErrExit:
	sprintf( acErrMsg,"�շ�ʧ��[%s]",RtCode);
	/****Modified by SSH,2006.8.22,���û��****/
	/****
	get_zd_data( "0093","0" );
	****/
	strcpy( RtCode,g_pub_tx.reply );
	set_zd_data(DC_REPLY,RtCode);
    return 1;
}

