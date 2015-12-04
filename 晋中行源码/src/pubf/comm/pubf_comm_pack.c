#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_pack.c
* 功能描述：   
*              pub_comm_crea_pack     生成通讯包
*              pub_comm_chan_pack     将通讯包内容拆入数据字典缓冲区
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/

#include <stdio.h>
#include <time.h>
#define EXTERN
#include "public.h"
#include "svrpub_c.h"
#include "tx_log_c.h"
#include "data_dic_c.h"
#include "tx_def_c.h"
#include "auth_chk_def_c.h"

/**********************************************************************
 * 函 数 名：  pub_comm_crea_pack
 * 函数功能：  生成通讯包
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：btmap:      比特图
 *
 *     输  出：pack:       通讯包
 *             pleng:      通讯包长度
 *
 *     返回值： 0 成功
 *            非0 失败
 *
 * 修改历史：
 *                   
********************************************************************/
long get_tlsrno();
long set_zd_long( char *sjbm, long longdata );

int pub_comm_crea_pack( char btmap[129], unsigned char *pack, int *pleng )
{
    register i;
    unsigned char pack_h[45];
    unsigned char *p,*q,tt;
    int j,l,len,bitnum,hnum=0;
    char formstr[10],tcd[4],pstr[7],asmeb[34],as1[11];
    char bit[129];
    int plen=0;
char xxx[101];
/* 20120419 by Wang Yongwei 判断是否存在相同的保存文件 */
    int    ret=0;
    char   Tx_code[5];
    char   file_name[101];
    FILE   *fp1=0;
    FILE *fp=NULL;
    long   Tx_date=0;
    long   Tx_trace_no=0;
    char  packm[6700];
    char  sLeng[11];
    char  sDc_tel[7];
    long  iLeng=0; 
    char filename[101];
    char filename_cp[101];
    char ttyno[11];
    char file_flag[3];
    char tmp_str[101];
    memset(Tx_code,0x00,sizeof(Tx_code));
    memset(sDc_tel,0x00,sizeof(sDc_tel));
    memset(ttyno,0x00,sizeof(ttyno));
    get_zd_data(DC_TX_CODE,Tx_code);
    get_zd_data(DC_TEL,sDc_tel);
    get_zd_data(DC_TX_BR_NO,g_pub_tx.tx_br_no);
    get_zd_long(DC_DATE,&g_pub_tx.tx_date); /* 终端号 */
    get_zd_long(DC_TRACE_NO,&g_pub_tx.trace_no); /* 终端号 */
    if(!memcmp(Tx_code,"9191",4)&&!strcmp(g_pub_tx.reply,"0000"))
    {
	memset(packm,0x00,sizeof(packm));
	memset(file_name,0x00,sizeof(file_name));
    	get_zd_long("0450",&Tx_date);
    	get_zd_long("0460",&Tx_trace_no);
	sprintf(file_name,"%s/txt/%ld-%ld-%s.txt",getenv("HOME"),Tx_date,Tx_trace_no,sDc_tel);
	/* 若是补打文件则读取文件组成报文 */
	fp1=fopen(file_name,"r+");		
	if(fp1 == NULL)
	{
		vtcp_log("%s,%d,打开文件失败!",__FILE__,__LINE__);
		return(-1);
	}
	memset(sLeng,0x00,sizeof(sLeng));
	memset(file_flag,0x00,sizeof(file_flag));
	memset(tmp_str,0x00,sizeof(tmp_str));
	fgets(sLeng,10,fp1);
	iLeng=atol(sLeng);
	fgets(tmp_str,101,fp1);
	fread(packm,iLeng,1,fp1);
	vtcp_log("%s,%d,打印组包内容[%s]",__FILE__,__LINE__,packm);
	fclose(fp1);
	memcpy(pack,packm,iLeng);
	pub_base_cut_str(tmp_str,file_flag,".",1);
	if(file_flag[0]=='1')
	{
		pub_base_cut_str(tmp_str,ttyno,".",2);
		pub_base_cut_str(tmp_str,Tx_code,".",3);
	vtcp_log("%s,%d,三个要素[%s][%s]",__FILE__,__LINE__,ttyno,Tx_code);
		memset(filename,0x00,sizeof(filename));
		sprintf(filename,"%s/txt/%s%s",getenv("HOME"),g_pub_tx.tx_br_no,ttyno);
		sprintf(filename_cp,"%s/txt/%ld-%ld-%s.snd",getenv("HOME"),Tx_date,Tx_trace_no,sDc_tel);
		memset(tmp_str,0x00,sizeof(tmp_str));
		sprintf(tmp_str,"cp %s %s",filename_cp,filename);
		vtcp_log("%s,%d,复制文件失败cp %s %s",__FILE__,__LINE__,filename_cp,filename);
		ret=system(tmp_str);
		if(ret)
		{
			vtcp_log("%s,%d,复制文件失败cp %s %s",__FILE__,__LINE__,filename_cp,filename);
			return(-1);
		}
		set_zd_data(DC_FILE_SND,file_flag);
		set_zd_data(DC_TX_CODE,Tx_code);
	}
	*pleng=iLeng;
	return(0);
    } 

    MEMSET_DEBUG(bit,'0',129);
    bit[128]=0;
    strcpy(bit,btmap);
    bit[DCBT_TX_CODE-1]='1';
    bit[DCBT_TRACE_NO-1]='1';
    bit[DCBT_REPLY-1]='1';
    bit[DCBT_AUTH_CODE-1]='1';
    bit[DCBT_GET_MSG-1]='1';
    bit[DCBT_FILE_SND-1]='1';
    bit[104-1]='1'; /*用来传打印标示码 */
    bit[3-1]='1';/*新平台传文件*/
    bit[10-1]='1';
    MEMSET_DEBUG(pack_h,'\0',45);
    pack_h[0]='0';
    hnum=1;
    MEMSET_DEBUG(pack,'\0',hnum);
    MEMCPY_DEBUG(pack,pack_h,hnum);

    q=pack+4+hnum;
    MEMSET_DEBUG(pack+hnum,'\0',20);
    MEMCPY_DEBUG(pack+hnum,"1001",4);

    bitnum= bit[0]=='1'? 128:64;
    p=q+bitnum/8;
    for (i=0,len=0; i<bitnum; i++)
    {
        l=i%8;
        if (!l) {tt=128;j=i/8; } else tt=tt>>1;
        if (bit[i]=='1') q[j]^=tt;
        else continue;
        /*if (i==0 || i==64) continue;   */ 
        if (i==0 ) continue;    
        if (!Bit_Arr[i].len) continue;

        if (Bit_Arr[i].lx!='0')
        {
            sprintf(formstr,"%%0%cd",Bit_Arr[i].lx);
            sprintf(tcd,formstr,Bit_Arr[i].len);
            MEMCPY_DEBUG(p,tcd,Bit_Arr[i].lx-'0');
            p+=Bit_Arr[i].lx-'0';
        }
        l=Bit_Arr[i].val;
        MEMCPY_DEBUG(p,Pu_Val[l].val,Bit_Arr[i].len);

/**
MEMCPY_DEBUG(xxx,Pu_Val[l].val,Bit_Arr[i].len);
xxx[Bit_Arr[i].len]=0;
vtcp_log("[%d]  [%s] [%d]",i+1,xxx,l);
**/
        p+=Bit_Arr[i].len;
    }

    *pleng=p-pack;
    plen=p-pack;
    plen=plen-20;
	/****TODO:Add by SSH,2006.8.23,调试,应答原始数据*****/
	vtcp_log("%s,%d,应答原始报文:",__FILE__,__LINE__);
	vtcp_log("MSGTYPE:[%.5s]\n",pack);
	vtcp_log("BITMAP1:[%.64s]\n",bit);
	vtcp_log("BITMAP2:[%.64s]\n",bit+64);
	vtcp_log("DATA:[%s]\n",pack+21);
/* 为补打时查询原有报文做准备 20120418 by Wang Yongwei, 将pack的内容保存到一个文件内*/
     {
	memset(file_name,0x00,sizeof(file_name));
	memset(file_flag,0x00,sizeof(file_flag));
        get_zd_data(DC_FILE_SND,file_flag);
	if(g_pub_tx.trace_no>0)
	{
		if(file_flag[0]>'0')
		{
			file_flag[0]='1';
		}else{
			file_flag[0]='0';
		}
		sprintf(file_name,"%s/txt/%ld-%ld-%s.txt",getenv("HOME"),g_pub_tx.tx_date,g_pub_tx.trace_no,sDc_tel);
		/* 得到文件名 */
		fp=fopen(file_name,"w+");	
		if(fp==NULL)
		{
			vtcp_log("%s,%d,打开文件失败!",__FILE__,__LINE__);
			return(-1);
		}
    		get_zd_data(DC_TTY,ttyno); /* 终端号 */
		/* 写入报文长度 */
		fprintf(fp,"%d\n",*pleng);
		fprintf(fp,"%s.%s.%s\n",file_flag,ttyno,Tx_code);
		fwrite(pack,*pleng,1,fp);
		fclose(fp);
		/* 判断是否存在文件若存在保存文件为别名 */
		if(file_flag[0]>'0') /* 表示有文件存在,将文件复制一个别名 */
		{
			memset(filename,0x00,sizeof(filename));
			sprintf(filename,"%s/txt/%s%s",getenv("HOME"),g_pub_tx.tx_br_no,ttyno);
			sprintf(filename_cp,"%s/txt/%ld-%ld-%s.snd",getenv("HOME"),g_pub_tx.tx_date,g_pub_tx.trace_no,sDc_tel);
			memset(tmp_str,0x00,sizeof(tmp_str));
			sprintf(tmp_str,"cp %s %s",filename,filename_cp);
			ret=system(tmp_str);
			if(ret)
			{
				vtcp_log("%s,%d,复制文件失败cp %s %s",__FILE__,__LINE__,filename,filename_cp);
				return(-1);
			}
		}
	}
     }
     /*  END BY WangYongwei 20120418 */
    return 0;
}
/**********************************************************************
 * 函 数 名：  pub_comm_chan_pack
 * 函数功能：  将通讯包内容拆入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：pack:       通讯包
 *     输  出：数据字典缓冲区
 *     返回值：0 成功      非0 失败
 * 修改历史：
 *                   
********************************************************************/

int pub_comm_chan_pack( unsigned char *pack )
{
    register i;
    unsigned char tt,*p,*q;
    char tcd[14];
    char XXXXXX[1200];
    int j,l,len,bitnum;
    char line[100],bnum[7];
    struct tx_log_c tx_log;
    struct tx_def_c tx_def;
	int tmplen;
	long auth_no;
	char auth[5];
	int tpl;

char xxxx[100];
    MEMSET_DEBUG(&tx_log,0x00,sizeof(tx_log));

/*JYW*/
vtcp_log("======pack========");
vtcp_log("[%21.21s]", pack);
vtcp_log("[%s]", pack+21);
vtcp_log("jyw ---- look ---- 2");
/**/
    q=pack+5;
    if (q[0]&128) bitnum=128; else bitnum=64;
    vtcp_log("[%s][%d]q[0]=[%c],bitnum=[%d]", __FILE__,__LINE__,q[0],bitnum);
    p=pack+5+bitnum/8;    
    for (i=0; i<bitnum; i++)
    {
	memset(XXXXXX,0x00,sizeof(XXXXXX));
        l=i%8;
        if (!l) { tt=128; j=i/8; } else tt=tt>>1;
        if ((q[j]&tt)!=0) Rec_Bitp[i]='1';
        else { Rec_Bitp[i]='0'; continue; }
        /* if (i==0 || i==64) continue; */
        if (i==0 ) continue;
        if (!Bit_Arr[i].len) continue;
        if (Bit_Arr[i].lx!='0')
        {
            l=Bit_Arr[i].lx-'0';
            MEMCPY_DEBUG(tcd,p,l);
            tcd[l]='\0';
            len=atoi(tcd);
            p+=l;
        }
        else
        {
            len = Bit_Arr[i].len;
        }
        l=Bit_Arr[i].val;
        MEMCPY_DEBUG(Pu_Val[l].val,p,len ); 
        MEMCPY_DEBUG(XXXXXX,Pu_Val[l].val,len ); 
	if(getenv("BBDEBUG")!=NULL)
              vtcp_log("[%d]  [%s] [%d]",i+1,XXXXXX,l);
		p+=len;
    }
    Rec_Bitp[bitnum]='\0';
    get_zd_data(DC_TX_BR_NO,tx_log.tx_br_no);
    get_zd_data(DC_TX_CODE,tx_log.tx_code);

	RespCode=Tx_def_Sel(RtCode,&tx_def,"tx_code='%s'",tx_log.tx_code);
	if ( RespCode )
	{
        strcpy(RtCode,"S007");
        sprintf(acErrMsg,"交易定义表中无此交易定义[%s]",tx_log.tx_code);
		WRITEMSG
        return 1;
	}

	get_zd_long( DC_AUTH_CODE,&auth_no );
	/*
	get_zd_data( DC_AUTH,auth );
	pub_base_strpack(auth);
	*/

	if ( tx_def.log_ind[0]=='1' )
	{
        tx_log.trace_no=get_tlsrno ();
		if ( tx_log.trace_no < 0 )
		{
            strcpy(RtCode,"S023");
            sprintf(acErrMsg,"生成流水号错误");
			WRITEMSG
            return 1;
		}
		set_zd_long(DC_TRACE_NO,tx_log.trace_no);

        MEMSET_DEBUG(tx_log.tx_pack,0x00,sizeof(tx_log.tx_pack));
        strncpy(tx_log.tx_pack,pack,5);
        tx_log.tx_pack[5]=0x00;
        strcpy(tx_log.bit_map,Rec_Bitp);
        if(Rec_Bitp[0]=='1')
        {
            strcat(tx_log.tx_pack,"^^^^^^^^^^^^^^^^");
			tpl=strlen(pack+21);
			if( tpl<1000 )
            	strcat(tx_log.tx_pack,pack+21);
			else
            	strncat(tx_log.tx_pack,pack+21,1000);
            tx_log.tx_pack[strlen(tx_log.tx_pack)]=0x00;
        }
        else
        {
            strcat(tx_log.tx_pack,"^^^^^^^^");
            tpl=strlen(pack+13);
			if( tpl<1010 )
            	strcat(tx_log.tx_pack,pack+13);
			else
            	strncat(tx_log.tx_pack,pack+13,1010);
            tx_log.tx_pack[strlen(tx_log.tx_pack)]=0x00;
        }
        RespCode = Tx_log_Ins( tx_log, RtCode );
        if( RespCode )
        {
            vtcp_log("--tx_log INSERT err:[%d]--", RespCode );
            strcpy(RtCode,"S024");
            sprintf(acErrMsg,"登记通讯日志错");
            return( RespCode );
        }
	}
	else
	{
        MEMSET_DEBUG(g_rcvpack,0x00,sizeof(g_rcvpack));
        strncpy(g_rcvpack,pack,5);
        g_rcvpack[5]=0x00;
        if(Rec_Bitp[0]=='1')
        {
            strcat(g_rcvpack,"^^^^^^^^^^^^^^^^");
            strcat(g_rcvpack,pack+21);
            g_rcvpack[strlen(g_rcvpack)]=0x00;
        }
        else
        {
            strcat(g_rcvpack,"^^^^^^^^");
            strcat(g_rcvpack,pack+13);
            g_rcvpack[strlen(g_rcvpack)]=0x00;
        }
	}

    return 0;
}
/*********************************************************************
 * 函 数 名：  pub_comm_chan_oldpack
 * 函数功能：  将通讯包内容拆入数据字典缓冲区2
********************************************************************/
int pub_comm_chan_oldpack( char *Bitp,unsigned char *pack )
{
    register i;
    unsigned char tt,*p,*q;
    char tcd[14];
    int j,l,len,bitnum;
    char line[100],bnum[7];
    struct tx_log_c tx_log;
    struct tx_def_c tx_def;
	int tmplen;
	long auth_no;

	bitnum=128;
    p=pack+5+16;
    for (i=1; i<bitnum; i++)
    {
		if( Bitp[i]=='0' ) continue;

        if (!Bit_Arr[i].len) continue;
        if (Bit_Arr[i].lx!='0')
        {
            l=Bit_Arr[i].lx-'0';
            MEMCPY_DEBUG(tcd,p,l);
            tcd[l]='\0';
            len=atoi(tcd);
            p+=l;
        }
        else
        {
            len = Bit_Arr[i].len;
        }
        l=Bit_Arr[i].val;
        MEMCPY_DEBUG(Old_Pu_Val[l].val,p,len ); 
		p+=len;
    }

    return 0;
}
/*********************************************************************
 * 函 数 名：  pub_comm_
 * 函数功能：  将通讯包内容拆入临时结构回传
********************************************************************/
int pub_comm_show_pack( char *Bitp )
{
    int i;
    unsigned char tt,*p,*q;
    char tcd[14];
    int j,l,len,bitnum;
    char line[100],bnum[7];
    struct tx_log_c tx_log;
    struct tx_def_c tx_def;
	int tmplen;
	long auth_no;
	struct data_dic_c vdata_dic;
	char vdatacode[5];
	int ret=0;
	char tmpstr[1024];
	double tmpdb;
	int k=0;

	MEMSET_DEBUG( Bitp,'0',20 ); /*字典头20个公用*/
	Bitp[DCBT_TEL-1]='1';

	ret=Data_dic_Dec_Sel(g_pub_tx.reply," 1=1 order by data_code ");
	if( ret ) goto ErrExit;
	while(1)
	{
		ret=Data_dic_Fet_Sel( &vdata_dic,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( vdata_dic.data_leng==0 ) continue;

		strcpy( vdatacode,vdata_dic.data_code );
		vdatacode[3]='\0';
		i=atol(vdatacode)-1;

		if( i<=0 ) continue;
		if( i>=128 ) break;
		if( Bitp[i]=='0' ) continue;
        if (!Bit_Arr[i].len) continue;

        l=Bit_Arr[i].val;
		while( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm)>0 )
		{
			l++;
		}
		if( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm) ) 
			continue;
        MEMCPY_DEBUG(tmpstr,Old_Pu_Val[l].val,vdata_dic.data_leng ); 
		tmpstr[vdata_dic.data_leng]='\0';
		if( vdata_dic.data_type[0]=='4' )
		{
			tmpdb=atof(tmpstr);
			for( k=0;k<vdata_dic.data_dec;k++ )
				tmpdb=tmpdb/10;
			set_zd_double(vdata_dic.data_code,tmpdb);
		}
		else
			set_zd_data(vdata_dic.data_code,tmpstr);
	}
	Data_dic_Clo_Sel();

    return 0;
ErrExit:
    return -1;
}
/*比较输入与记录,复核不是比较，将原纪录整个写入到包里*/
int pub_comm_cmpchk_pack_data( char *Bitp )
{
    register i;
    unsigned char tt,*p,*q;
    char tcd[14];
    int j,l,len,bitnum;
    char line[100],bnum[7];
    struct tx_log_c tx_log;
    struct tx_def_c tx_def;
	int tmplen;
	long auth_no;
	double tmpdb1,tmpdb;
	struct data_dic_c vdata_dic;
	char vdatacode[5];
	int ret=0;
	char tmpstr[1024];
	char tmpstr1[1024];

	MEMSET_DEBUG( Bitp,'0',20 ); /*字典头20个公用*/

	ret=Data_dic_Dec_Sel(g_pub_tx.reply," 1=1 order by data_code ");
	if( ret ) goto ErrExit;
	while(1)
	{
		ret=Data_dic_Fet_Sel( &vdata_dic,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( vdata_dic.data_leng==0 ) continue;

		strcpy( vdatacode,vdata_dic.data_code );
		vdatacode[3]='\0';
		i=atol(vdatacode)-1;

		if( i<=0 ) continue;
		if( i>=128 ) break;
		if( Bitp[i]=='0' ) continue;
        if (!Bit_Arr[i].len) continue;

        l=Bit_Arr[i].val;
		while( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm)>0 )
		{
			l++;
		}
		if( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm) ) 
			continue;
        MEMCPY_DEBUG(tmpstr,Old_Pu_Val[l].val,vdata_dic.data_leng ); 
		tmpstr[vdata_dic.data_leng]='\0';

		if( vdata_dic.data_type[0]=='0' )
		{
			set_zd_data(vdata_dic.data_code,tmpstr);
		}
		else if( vdata_dic.data_type[0]=='4' )
		{
			tmpdb=atof(tmpstr);
			for( i=0;i<vdata_dic.data_dec;i++ )
			{
				tmpdb=tmpdb/10;
			}
			set_zd_double(vdata_dic.data_code,tmpdb);
		}
		else
		{
			set_zd_data(vdata_dic.data_code,tmpstr);
		}

	}
	Data_dic_Clo_Sel();

    return 0;
ErrExit:
    return -1;
}
/*比较输入与记录*/
int pub_comm_cmp_pack_data( char *Bitp )
{
    register i;
    unsigned char tt,*p,*q;
    char tcd[14];
    int j,l,len,bitnum;
    char line[100],bnum[7];
    struct tx_log_c tx_log;
    struct tx_def_c tx_def;
	int tmplen;
	long auth_no;
	double tmpdb1,tmpdb;
	struct data_dic_c vdata_dic;
	char vdatacode[5];
	int ret=0;
	char tmpstr[1024];
	char tmpstr1[1024];

	MEMSET_DEBUG( Bitp,'0',20 ); /*字典头20个公用*/

	ret=Data_dic_Dec_Sel(g_pub_tx.reply," 1=1 order by data_code ");
	if( ret ) goto ErrExit;
	while(1)
	{
		ret=Data_dic_Fet_Sel( &vdata_dic,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( vdata_dic.data_leng==0 ) continue;

		strcpy( vdatacode,vdata_dic.data_code );
		vdatacode[3]='\0';
		i=atol(vdatacode)-1;

		if( i<=0 ) continue;
		if( i>=128 ) break;
		if( Bitp[i]=='0' ) continue;
        if (!Bit_Arr[i].len) continue;

        l=Bit_Arr[i].val;
		while( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm)>0 )
		{
			l++;
		}
		if( strcmp(vdata_dic.data_code,Old_Pu_Val[l].sjbm) ) 
			continue;
        MEMCPY_DEBUG(tmpstr,Old_Pu_Val[l].val,vdata_dic.data_leng ); 
		tmpstr[vdata_dic.data_leng]='\0';

		get_zd_data(vdata_dic.data_code,tmpstr1);
		pub_base_strpack(tmpstr);
		pub_base_strpack(tmpstr1);

		if( vdata_dic.data_type[0]=='0' && strcmp(tmpstr,tmpstr1) )
		{
			sprintf(acErrMsg,"%s [%s]",vdata_dic.data_code,tmpstr );
			set_zd_data(DC_GET_MSG,acErrMsg);
			strcat(acErrMsg,"[");
			strcat(acErrMsg,tmpstr1);
			strcat(acErrMsg,"]");
			WRITEMSG
			goto ErrExit;
		}
		if( vdata_dic.data_type[0]=='4' )
		{
			tmpdb=atof(tmpstr);
			tmpdb1=atof(tmpstr1);
			for( i=0;i<vdata_dic.data_dec;i++ )
			{
				tmpdb=tmpdb/10;
			}
			if( tmpdb-tmpdb1>0.0005 || tmpdb-tmpdb1<-0.0005 )
			{
				sprintf(acErrMsg,"%s [%lf]",vdata_dic.data_code,tmpdb );
				set_zd_data(DC_GET_MSG,acErrMsg);
				strcat(acErrMsg,"[");
				strcat(acErrMsg,tmpstr1);
				strcat(acErrMsg,"]");
				WRITEMSG
				goto ErrExit;
			}
		}
		if( vdata_dic.data_type[0]=='1' 
				&& ( atol(tmpstr)-atol(tmpstr1)<=-0.00005 
					|| atol(tmpstr)-atol(tmpstr1)>=0.00005 ) ) 
		{
			sprintf(acErrMsg,"%s [%s]",vdata_dic.data_code,tmpstr );
			set_zd_data(DC_GET_MSG,acErrMsg);
			strcat(acErrMsg,"[");
			strcat(acErrMsg,tmpstr1);
			strcat(acErrMsg,"]");
			WRITEMSG
			goto ErrExit;
		}
	}
	Data_dic_Clo_Sel();

    return 0;
ErrExit:
    return -1;
}
int set_chk_fied( char *tx_code )
{
	int ret=0;
	char reply[5];
	struct auth_chk_def_c auth_chk_def_c;
	char rtstr[101];
	int i=0;

	ret = Auth_chk_def_Dec_Sel( reply , "tx_code='%s'" , tx_code );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}

	i=0;
	MEMSET_DEBUG( rtstr,0,sizeof(rtstr) );
	while( 1 )
	{
		ret = Auth_chk_def_Fet_Sel( &auth_chk_def_c , reply );
		if( ret==100 ) break;
		if (ret)
		{
			WRITEMSG
			goto ErrExit;
		}
		i++;
		if( i>25 ) break;
		strcat(rtstr,auth_chk_def_c.data_code);
		strcat(rtstr,",");
	}

	Auth_chk_def_Clo_Sel();
vtcp_log(" CHECK 0760 [%s]",rtstr );
	set_zd_data("0760",rtstr);
	pub_set_trad_bit_one(76-1);
	pub_set_trad_bit_one(DCBT_CHK-1);
	pub_set_trad_bit_one(DCBT_AUTH_CODE-1);

	return 0;
ErrExit:
	set_zd_data("0760",rtstr);
	return 1;
}

vprintfBit( unsigned char *pBit)
{
   int  i=0,j=0;
   int tt=0;
   int tmp;
   unsigned char cTmp ;
   cTmp = pBit[i];
   vtcp_log("pBit===[");
   for(i=0; i<16; i++)
   {
 for (j=0; j<8; j++)
 {
    cTmp = pBit[i];
 if(j == 0) { tt=128;}   /* j+1 = 在第几个位图字节 */
 else  { tt = tt>>1;}
   cTmp = cTmp & tt;
            if(cTmp == 0)       vtcp_log("0");
            else                vtcp_log("1");
        }
      }
      vtcp_log("]\n");
}
