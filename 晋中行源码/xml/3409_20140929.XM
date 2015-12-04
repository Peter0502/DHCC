<?xml version="1.0" encoding="GBK" ?>
<TRANSACTION RIGHTS="11110000" TITLE="3409 久悬处理"
  xmlns="http://www.otd.org/MENU" xmlns:W3CNS="http://www.w3c.org">
<COMMSEND>"{
	commsend_001();
	$FD21=#CURCD;
	$FD25=#NAME;
	$FD26='';
	$FD31=#ACTNO;
	$FD33=#IN_ACTNO; //内部账户
	$FD34=#ACTSEQ;
	$FD39=#TXAMT9;
	$FD67=#TXTP;
	$FD68='2';
	$FD81=#CONT;
	$FD100=getitems('#RAVBAL');
	//$FD101=getitems('#ACTNO#ACTSEQ#RAVBAL');
	}"
</COMMSEND>
<COMMRECV>"{
	commrecv_001();
}"
</COMMRECV>

<VARIABLE>
  <ITEM LINE="3"  COL="3" TYPE="%-60s" FUNCTIONS=
   "T(3409                            久悬处理)"/>
  <ITEM LINE="4"  COL="3"  TYPE="%-74s" FUNCTIONS=
   "T(─────────────────────────────────────)"/>

  <ITEM NAME="#TXNO" TYPE="%-4s" FUNCTIONS="T($TXNO=2409)"/>
  <ITEM NAME="#MSGTYPE" TYPE="%-5s" FUNCTIONS="T($MSGTYPE=03001)"/>
  <ITEM NAME="#ACT_LX_CK" TYPE="%-5s" FUNCTIONS=""/>
  
  <ITEM LINE="5"   COL="09" TYPE="%-50s" FUNCTIONS="T(注意:本交易不处理在旧系统中已经成为久悬户的户)"/>
  <ITEM LINE="7"   COL="09" TYPE="%-12s" FUNCTIONS="T(处理方式:!)"/>
  <ITEM LINE="11"  COL="09" TYPE="%-12s" FUNCTIONS="T(账/卡 号:)"/>
  <ITEM LINE="11"  COL="45" TYPE="%-12s" FUNCTIONS="T(账户序号:)"/>
  <ITEM LINE="12"  COL="09" TYPE="%-12s" FUNCTIONS="T(户    名:)"/>
  <ITEM LINE="13"  COL="09" TYPE="%-12s" FUNCTIONS="T(币    种:)"/>
  <ITEM LINE="14"  COL="09" TYPE="%-12s" FUNCTIONS="T(余    额:)"/>
  <ITEM LINE="16"  COL="09" TYPE="%-12s" FUNCTIONS="T(摘    要:)"/>
  
 
  <ITEM NAME="#TXTP" INPUT="SELECT" LINE="7"  COL="19" TYPE="%-1s" FUNCTIONS="T(1)">
  <SELECT>
	 <OPTION VALUE="1" TITLE="久悬户登记"/>
	 <OPTION VALUE="2" TITLE="久悬户取回"/>
  </SELECT>
  </ITEM>
  <ITEM NAME="#STXTP"  TYPE="%-4s"   FUNCTIONS="j(#TXTP=1,T{登记})j(#TXTP=2,T{取回})">
  <ONLOSTFOCUS>
	"{  
	   if (#TXTP=='2'){
	    	show(#IN_NAME,false);
				show(#IN_ACTNO,false);
				enable(#IN_NAME,false);
				enable(#IN_ACTNO,false);
	}
	}"
	</ONLOSTFOCUS>
	</ITEM>	

  <ITEM NAME="#ACTNO" 	INPUT="TEXT" 	LINE="11"  COL="19" MESSAGE="请输入新帐号" TYPE="%-19s" FUNCTIONS="T()V(NB)">
  <ONLOSTFOCUS>
	"{  dim @zt1 as string;
	    @zt1=substr(#ACTNO,0,1);
	   if (@zt1=='1'){
	    showmsg('此帐户为储蓄户');
	    return(false);	
	}
	}"
	</ONLOSTFOCUS>
	</ITEM>
  <ITEM NAME="#ACTSEQ" 	INPUT="TEXT" 	LINE="11"  COL="55	" TYPE="%4d" 	FUNCTIONS="T()V(NB)"/>

  <ITEM NAME="#CON2"    TYPE="%-1s" >
  <ONLOSTFOCUS>
   "{
		dim @amt as string;
		commsend_001();
		$FD16='9702';
		$FD31=#ACTNO;
		$FD34=#ACTSEQ;
		callserver();

		#NAME=$FD25;
		showmsg(#NAME);
		#CURCD=$FD21;
		if($FD12=='0000'){
			#RAVBAL=substr($FD100,0,16);
			@amt=substr($FD100,0,14)+'.'+substr($FD100,14,2);
			//#AVBAL=comma(@amt,21);
			//为转帐TRANSINFO.IMP准备交易金额
			#TXAMT=val(@amt);
			$FD41='0000000000000000';
		}else{
		
			#RAVBAL=substr($FD41,0,16);
			@amt=substr($FD41,0,14)+'.'+substr($FD41,14,2);
			//为转帐TRANSINFO.IMP准备交易金额
			#TXAMT=val(@amt);
		}
    #AVBAL=comma(itoa(#TXAMT,'%17.2f'),21);
		show(#NAME,true);
		show(#CURCD,true);
		enable(#CURCD,false);
		show(#AVBAL,true);
   }"
  </ONLOSTFOCUS>
  </ITEM>
	<ITEM NAME="#IN_NAME" LINE="18"  COL="09" TYPE="%-14s" FUNCTIONS="T(转入内部户:)"/>
	<ITEM NAME="#IN_ACTNO" 	INPUT="TEXT" 	LINE="18"  COL="19" TYPE="%-19s" FUNCTIONS="V(NB)">
	  <ONLOSTFOCUS>
	"{  dim @zt1 as string;
			$FD33=#IN_ACTNO;
	    @zt1=substr(#IN_ACTNO,0,1);
	   if (@zt1=='1'){
	    showmsg('此帐户为储蓄户');
	    return(false);	
	}
	}"
	</ONLOSTFOCUS>
	</ITEM>

  <ITEM NAME="#NAME" 	LINE="12"  COL="19" TYPE="%-50s" FUNCTIONS=""/>
  <ITEM NAME="#CURCD" 	INPUT="SELECT" 	LINE="13"  COL="19" TYPE="%-2s"  FUNCTIONS="T(01)" ENABLE="FALSE">
  <SELECT>
	 <OPTION VALUE="01" TITLE="人民币  "/>
  </SELECT>
  </ITEM>

  <ITEM NAME="#RAVBAL" 	 TYPE="%-16s"   FUNCTIONS=""/>
  <ITEM NAME="#TXAMT"    TYPE="%17.2f"  FUNCTIONS=""/><!---余额--->
  <ITEM NAME="#AVBAL" 	 INPUT="LABEL"  LINE="14"  COL="19" TYPE="%-21s" />

  <ITEM NAME="#CONT" 	 INPUT="TEXT" 	LINE="16"  COL="19" TYPE="%-50s" FUNCTIONS=""/>
  
  <ITEM NAME="#PASSWD" 	TYPE="%-6s" 	FUNCTIONS="T(      )"/>
  <ITEM NAME="#TXTIME" 	TYPE="%-6s" 	FUNCTIONS="T(      )"/>
  <ITEM NAME="#CHECKNO" TYPE="%-8s" 	FUNCTIONS="T(        )"/>
  <ITEM NAME="#WSSRNO"  TYPE="%-6s" 	FUNCTIONS="T(      )"/>
  <ITEM NAME="#TXAMT9"  TYPE="%-16s" 	FUNCTIONS="T(                )"/>
  <ITEM NAME="#ACTNO2"  TYPE="%-19s" 	FUNCTIONS="T(               )"/>
  <ITEM LINE="21" COL="59"  TYPE="%-6s" FUNCTIONS="T(确  定)" INPUT="SUBMIT"/>

  </VARIABLE>
<REQUEST>
</REQUEST>
<RESPONSE>
	<PACKAGE DEVICE="SCREEN" DESC="久悬户处理成功">
	NOTHING
	</PACKAGE>

	<PRINT>"{
		$KINDNO='00';
		if(#TXTP=='1')
		
		{
		print( 8, 25,'久悬户登记成功 ！');
	  }
	  else
	  {
	  print( 8, 25,'久悬户取回成功 ！');
	  }
	}"
	</PRINT>
</RESPONSE>
<RESPONSE>

<RESPONSE>
	<PACKAGE DEVICE="PRINTER" DESC="请打印业务专用凭证" LINESPACE="24">
	NOTHING
	</PACKAGE>
	<PRINT>
	"{
		dim @txtime as string;

		@txtime = substr($SYSTIME,0,2)+':'+substr($SYSTIME,2,2)+':'+substr($SYSTIME,4,2);
		$KINDNO = '00';
		
		print( 5,20,'机构名称:[30]',$BRNAME);
		print( 5,2,'代码:3409');
		print( 5,13,'交易:久悬处理交易');
		if(#TXTP == '1') //登记
		{
			print( 7,30,'久悬户处理([4   ])',#STXTP);
			print( 8,20,'帐    号:[19                 ]     帐户序号:[4]',#ACTNO,#ACTSEQ);
			print( 9,20,'币    种:人民币                  户    名:[50]',#NAME);
			print(10,20,'摘    要:[20                  ]    交易金额:[21]',#CONT,#AVBAL);
			
		}
		else //取回
		{	
				print( 7,30,'久悬户处理([4   ])',#STXTP);
				print( 8,20,'帐    号:[19                 ]     帐户序号:[4]',#ACTNO,#ACTSEQ);
				print( 9,20,'币    种:人民币                  户    名:[50]',#NAME);
				print(10,20,'摘    要:[20                  ]    交易金额:[21]',#CONT,#AVBAL);
				print(11,20,'对方帐号:[20                  ]    对方户名:[50]',#LDFNO,#101CNAME);
		}
		print_sq();
		print(24,20,'备注:');
		print(24,27,'交易日期:[8] [8]',$HDATE,@txtime);
		print(24,5,'柜员:[4]',$FD7);
		print(24,5,'流水号:[6]',$FD4);
	}"
	</PRINT>
</RESPONSE>
<PACKAGE DEVICE="PRINTER" LINESPACE="24" DESC="打印业务专用凭证">
   NOTHING
 </PACKAGE>
 <PRINT>
  "{
 $KINDNO='00';
 dim @txtime  as string;
@txtime=substr($SYSTIME,0,2)+':'+substr($SYSTIME,2,2)+':'+substr($SYSTIME,4,2);
	print( 5,20,'机构名称:[30]',$BRNAME);
	print( 5,2,'代码:3409');
	print( 5,13,'交易:久悬处理交易');
	print( 7,30,'久悬户处理([4   ])',#STXTP);
	print( 9,20,'账号：[20]',#ACTNO);
	print( 10,20,'户名：[50]',#NAME);
	print( 11,20,'久悬账号：[20]',#IN_ACTNO);
	print( 12,20,'金额：[10]',#AVBAL)
	printacdtl_1();
	print_sq();
	print(24,20,'备注:');
	print(24,27,'交易日期:[8] [8]',$HDATE,@txtime);
	print(24,5,'柜员:[4]',$FD7);
	print(24,5,'流水号:[6]',$FD4);
	}"
	</PRINT>
</RESPONSE>
</TRANSACTION>

