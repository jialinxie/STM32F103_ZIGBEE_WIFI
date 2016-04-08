#include "exfuns.h"
#include "fattester.h"	
 

///////////////////////////////公共文件区////////////////////////////////////////////
FATFS fs[2];  		//逻辑磁盘工作区.	 
FIL file;	  		//文件1
FIL ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 fatbuf[512];//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////


FIL *fc_src=&file;	//源文件
FIL *fc_dst=&ftemp;	//目标文件
u8 *fc_buf=fatbuf;	//数据缓存区  

//文件名及路径管理器
struct _m_pfnmg pfnmg=
{
	pfnmg_init,
	pfnmg_dir_opendir,
	pfnmg_get_pname,
	pfnmg_get_plfname,
	pfnmg_get_lfname,
	pfnmg_get_ifname,
	pfnmg_scan_folder,
	0,
	0,
	0,
	0,
	0,
	0,	
};

//初始化路径
//不能以'/'结尾!
//path:路径;
//返回值:0,OK;其他,失败.
u8 pfnmg_init(u8*path)
{
	u8 res;
	res=f_opendir(&dir,(const TCHAR*)path);
	if(res)return res;//此路径非法
	else
	{										  
		pfnmg.cftp=0XFF; 	//当前为路径
		pfnmg.foldernum=0; 	//总文件数归零
		pfnmg.dfnum=0;	   	//目标文件数归零
		pfnmg.fttblsize=0; 	//目标文件类型表清空
		f_strcpy(pfnmg.plfname,path);//路径复制给pname	
	}
	return 0;
}
//打开当前目录下的某个文件夹.
//dirname:文件夹名字
//返回值:0,成功;其他,失败;
u8 pfnmg_dir_opendir(u8*dirname)
{
	u8 res;
	u8 len;
	u8 deep;						   					    
	res=f_opendir(&dir,(const TCHAR*)dirname);//打开此文件夹
	if(res==0)//路径OK
	{
		len=f_strlen(dirname); 
		if(dirname[len-1]=='.'&&dirname[len-2]=='.')//回到上一层目录.
		{	    
			deep=f_getdirdeep(dirname);//得到目录深度
			if(deep)//还不是根目录
			{
				if(deep>1)deep--;						//得到上两层的目录
				f_getdirstr(dirname,dirname,deep-1); //得到上一层目录	
			}else dirname[len-2]=0;//添加结束符.	 
		}
		f_strcpy(pfnmg.plfname,dirname);//复制路径
 		pfnmg.cftp=0XFF; 	//当前为路径
		pfnmg.foldernum=0; 	//总文件数归零
		pfnmg.dfnum=0;	   	//目标文件数归零
		pfnmg_scan_folder();//扫描这个新文件夹.
	}
	return res;		
}

//得到当前路径名
u8* pfnmg_get_pname(void) 
{
	u8 len;
	if(pfnmg.cftp!=0XFF)//当前不完全是路径名
	{
		len=f_strlen(pfnmg.plfname);
		while(pfnmg.plfname[len]!='/')len--;
		pfnmg.plfname[len]='\0';//加结束符.
		pfnmg.cftp=0XFF;//当前是路径
	} 
	return pfnmg.plfname;
}   
//得到某个文件名的带路径后的名字
//返回值:路径文件名首地址.
u8* pfnmg_get_plfname(void)
{										  		 
 	return pfnmg.plfname;		    
}
//得到文件名
//返回值:文件名首地址.
u8* pfnmg_get_lfname(void)
{
	u8 len;
	if(pfnmg.cftp==0XFF)return 0;//当前完全是路径名
	else
	{
		len=f_strlen(pfnmg.plfname);
		while(pfnmg.plfname[len]!='/')len--; 
		return pfnmg.plfname+len+1;//返回文件名  
	} 							    
}   
//得到指定编号文件的文件名 编号:0~dfnum+foldernum-1
//index:文件编号(包含了文件夹)
//*ftpt:类型表.
//num:类型数
//返回值:该文件的文件名.
u8* pfnmg_get_ifname(u16 index)
{
	FILINFO finfo;
	u16 i=0;
	u8 t,ftype;
	u8 res;
	u8 tlfname[100];//100个字节用于存放长文件名
	u8 *plfname;

	plfname=pfnmg_get_pname();//得到当前路径名    
	res=f_opendir(&dir,(const TCHAR*)plfname);//打开当前目录
    finfo.lfsize=0;//不使用长文件名 
	if(res==FR_OK)
	{
		if(index<pfnmg.foldernum)//是文件夹区
		{
			for(i=0;i<index+1;)
			{	 
				if(i==index)//已经等于当前要找的文件了.使用长文件名.
				{
					finfo.lfname=(TCHAR*)tlfname;		//长文件名缓存
					finfo.lfsize=sizeof(tlfname);		//缓存大小
				}
				res=f_readdir(&dir,&finfo);	//读取一个文件的信息
		    	if(res!=FR_OK||finfo.fname[0]==0)break;//出错,或者已经读完了
				if(finfo.fattrib&AM_DIR)
				{
					if(finfo.fname[0]=='.'&&finfo.fname[1]=='\0')continue;//不计算"."文件夹
					i++;
				}										    
			}   
		}else //在有效文件区
		{
			index-=pfnmg.foldernum;//实际文件的偏移
			for(i=0;i<index+1;)
			{	 
				if(i==index)//已经等于当前要找的文件了.使用长文件名.
				{
					finfo.lfname=(TCHAR*)tlfname;		//长文件名缓存
					finfo.lfsize=sizeof(tlfname);		//缓存大小
				}
				res=f_readdir(&dir,&finfo);	//读取一个文件的信息
		    	if(res!=FR_OK||finfo.fname[0]==0)break;//出错,或者已经读完了
				ftype=f_typetell((u8*)finfo.fname);//得到文件类型
				for(t=0;t<pfnmg.fttblsize;t++)if(pfnmg.fttbl[t]==ftype)break;
				if(t!=pfnmg.fttblsize)i++;//找到一个符合条件的.
 				   									    
			}
		}
		if(res==FR_OK&&finfo.fname[0]!=0)//成功读取了.
		{				    
			f_stradd(pfnmg.plfname,"/");
			if(*finfo.lfname)f_stradd(pfnmg.plfname,(u8*)finfo.lfname);	//存在长文件名
			else f_stradd(pfnmg.plfname,(u8*)finfo.fname);			   	//拷贝短文件名
			if(finfo.fattrib&AM_DIR)pfnmg.cftp=T_FOLDER;		  		//是个文件夹.	 
			else pfnmg.cftp=f_typetell((u8*)finfo.fname);		  		//目标文件
		}  
	}
 	return pfnmg_get_lfname();//返回得到的文件名.	
}   
//扫描文件夹
//ftpt:文件类型数组
//num:文件类型数
//返回值:文件个数(包括文件夹)
u16 pfnmg_scan_folder(void)
{
	u16 fcnt;  
	u8 i;
	u8 *pname; 

	pname=pfnmg_get_pname();		//得到路径名
	fcnt=f_getfoldernum(pname);		//得到当前文件夹下,文件夹的个数 
	pfnmg.foldernum=fcnt;			//目录个数
	for(i=0;i<pfnmg.fttblsize;i++)	//得到目标文件的个数
	{
		fcnt+=f_getfilenum(pname,pfnmg.fttbl[i]); 
	} 
 	pfnmg.dfnum=fcnt-pfnmg.foldernum;//目标文件的个数
	return fcnt;	
}    		  

//复制字符串.
//将str2的内容,复制到str1里面.
//返回值:0,成功;其他失败.
void f_strcpy(u8*str1,u8*str2)
{				  
	while(*str2!='\0')
	{
		*str1=*str2;
		str1++;		 
		str2++;
	}
	*str1='\0';//末尾加入结束符.  
}	
//得到某个字符串的长度
//str:字符串;
//返回值:长度.
u8 f_strlen(u8*str)
{
	u8 strlen=0;
	while(*str!='\0')
	{
		strlen++;
		str++;
	}
	return strlen;
}					
//对比两个字符串,是否相等.
//相等,返回1;否则,返回0.
u8 f_strcmp(u8*str1,u8 *str2)
{
	while(1)
	{
		if(*str1!=*str2)return 0;//不相等
		if(*str1=='\0')break;//对比完成了.
		str1++;
		str2++;
	}
	return 1;//两个字符串相等
}
//将小写字母转为大写字母,如果是数字,则保持不变.
u8 f_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}
//将字符串str2添加到str1的后面.	 
u8 f_stradd(u8 *str1,u8 *str2) 
{
	while(*str1!='\0')str1++;
	while(*str2!='\0')
	{
		*str1=*str2;
		str1++;
		str2++;
	}
	*str1='\0';	//末尾添加结束符
	return 0;	//添加成功.
}
//查找某个目录下是否有某个文件
//返回值0,有.其他,没有.
u8 f_find(u8*fpath)
{
	u8 res;
	res=f_open(fc_src,(const TCHAR*)fpath,FA_READ);
	f_close(fc_src);
	return res;		    
}
//复制文件
//fsrc:源文件路径
//fdst:目标文件路径
//func:状态显示函数指针. u32(*prog_func)(u32,u32);,第一个参数为文件大小,第二个参数为当前进行的位置.
//返回值:0,OK,其他,失败.  
u8 f_copy(u8 *fsrc,u8* fdst,prog_funs prog_fun)  
{
	u8 res=0;
	u32 offset=0;
 	res=f_open(fc_src,(const TCHAR*)fsrc,FA_READ);//只读方式打开
	if(res==FR_OK)//成功打开了文件,才开始.
	{   
	 	res=f_open(fc_dst,(const TCHAR*)fdst,FA_WRITE|FA_CREATE_ALWAYS);//新建一个文件
 		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(fc_src,fc_buf,512,&br);	 
			if(res!=FR_OK)break;				//执行错误	  
	 		res=f_write(fc_dst,fc_buf,br,&bw);	//写入 
			if(res!=FR_OK)break;				//执行错误
			if(br!=bw){res=0xff;break;}			//写入错误
			offset+=br;	  
			prog_fun(fc_src->fsize,offset);		//进度显示
			if(br!=512)break;					//读完了.
			f_lseek(fc_src,offset);//偏移到下一次读的地址
		}  
		f_close(fc_dst);
	}
	f_close(fc_src);
	return res;
}
//得到目录的深度
//pathname:路径名
//返回值:0,第一级目录(根目录);1,第二级目录...254,第254级目录
u8 f_getdirdeep(u8*pathname)
{
	u8 dpf=0;
	while(1)
	{
		if(*pathname=='\0')break;//到末尾了
		if(*pathname=='/')dpf++; //找到一个区分符
		pathname++; 		
	}	
	return dpf;
}	   

//得到第几级的目录.
//dsrc:源字符串.(路径)
//ddst:第几级目录字符串(路径)
//deep:0,第一级目录;1,第二级目录...254,第254级目录
//返回值:与deep相等,则成功,0XFF,失败.
u8 f_getdirstr(u8* dsrc,u8 *ddst,u8 deep) 
{
	u8 dpf=0;
	while(1)
	{
		if(*dsrc=='\0')
		{
			if(dpf)dpf--;		//是找到了结束符,而非目录分隔标志.
			break;				//到末尾了	   
		}
		if(*dsrc=='/')			//找到一个区分符
		{
			if(dpf==deep)break;	//目录深度相等了,退出
			dpf++; 				//目录深度增加
		}
		*ddst=*dsrc;			//copy			   
		ddst++;
		dsrc++;
	}
	if(dpf!=deep)return 0XFF;
	else 
	{
		*ddst='\0';//加入结束符.
		return dpf;
	}
}	
		 
const u8 *FILE_TYPE_TAB[4][13]=
{
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},
{"LRC","TXT","C","H"},
{"BIN","FON","MP3"},
{"BMP","JPG","JPEG"},
};	    
//报告文件的类型
//fname:文件名
//返回值:文件类型编号.0XFF,表示无法识别.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 i=0,j;
	while(i<200)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==200)return 0XFF;//错误的字符串.
	fname--;		//最后一个字符的位置
	for(i=0;i<5;i++)//复制后缀名
	{
		tbuf[4-i]=*fname;
		fname--;
	}
	if(tbuf[0]=='.')//后缀名为4个字节
	{
		tbuf[0]=f_upper(tbuf[1]);
		tbuf[1]=f_upper(tbuf[2]);
		tbuf[2]=f_upper(tbuf[3]);
		tbuf[3]=f_upper(tbuf[4]);
		tbuf[4]='\0';//加入结束符
	}else if(tbuf[1]=='.')//3个字节
	{
		tbuf[0]=f_upper(tbuf[2]);
		tbuf[1]=f_upper(tbuf[3]);
		tbuf[2]=f_upper(tbuf[4]);  
		tbuf[3]='\0';//加入结束符
	}else if(tbuf[2]=='.')//2个字节
	{
		tbuf[0]=f_upper(tbuf[3]);
		tbuf[1]=f_upper(tbuf[4]);   
		tbuf[2]='\0';//加入结束符
	}else if(tbuf[3]=='.')//1个字节
	{
		tbuf[0]=f_upper(tbuf[4]);    
		tbuf[1]='\0';//加入结束符
	}else return 0XFF;//未找到后缀名.
	for(i=0;i<4;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TAB[i][j]==0)break;//此组已经没有可对比的成员了.
			if(f_strcmp((u8 *)FILE_TYPE_TAB[i][j],tbuf))//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到
		 			   
}
//得到指定目录下的指定类型文件的个数
u16 f_getfilenum(u8* path,u8 ftype)
{
	u8 res;
	u8 type;
	u16 fnum=0;
	FILINFO finfo;
	res=f_opendir(&dir,(const TCHAR*)path);//打开SD卡上的PICTURE文件夹.
    finfo.lfsize=0;//不使用长文件名 
	if(res==FR_OK)
	{
		while(1)
		{	 
			res=f_readdir(&dir,&finfo);	//读取一个文件的信息
	    	if(res!=FR_OK||finfo.fname[0]==0)break;//出错,或者已经读完了
			type=f_typetell((u8*)finfo.fname);//得到文件类型	 
			if(type==ftype)fnum++;//此类型文件个数增加1.
		}
	}
    return fnum;
}			
//得到指定目录下的文件夹的个数
//包含".."文件夹
u16 f_getfoldernum(u8* path)
{
	u8 res;	    
	u16 fnum=0;
	FILINFO finfo;
	res=f_opendir(&dir,(const TCHAR*)path);//打开SD卡上的PICTURE文件夹.
    finfo.lfsize=0;//不使用长文件名 
	if(res==FR_OK)
	{
		while(1)
		{

			res=f_readdir(&dir,&finfo);	//读取一个文件的信息
	    	if(res!=FR_OK||finfo.fname[0]==0)break;//出错,或者已经读完了
			if(finfo.fattrib&AM_DIR)
			{
				if(finfo.fname[0]=='.'&&finfo.fname[1]=='\0')continue;//不计算"."文件夹
				fnum++;
			}										    
		}
	}
    return fnum;
}									    








