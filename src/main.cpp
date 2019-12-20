//一个window下用opengl做渲染实现video transition 的demo
//author:lihaiping1603@aliyun.com
//date:2019/01/21

#include "contex.h"
#include "global.h"
#include <assert.h>
#include <iostream>
#include "render.h"
#include <string.h>
#include "transitionRender.h"
#include "imageHelper.h"

#define FRAME_TIME_BASE            1000000

//用于备份数据的
int32_t BackupFrame(MediaFrameInfo_S &srcFrame,MediaFrameInfo_S &bkFrame){
	if (srcFrame.pFrame!=NULL&&srcFrame.nFrameSize!=0
		&&srcFrame.nWidth!=0&&srcFrame.nHeight!=0)
	{
		//当数据大小不一致或者数据指针没有分配的时候才需要分配
		if (bkFrame.pFrame==NULL||bkFrame.nFrameSize!=srcFrame.nFrameSize)
		{
			if (bkFrame.pFrame)
			{
				delete bkFrame.pFrame;
				bkFrame.pFrame=NULL;
			}
			bkFrame.pFrame=new uint8_t[srcFrame.nFrameSize];
			assert(bkFrame.pFrame);
			bkFrame.nFrameSize=srcFrame.nFrameSize;
			memset(bkFrame.pFrame,0,bkFrame.nFrameSize);
		}
		memcpy(bkFrame.pFrame,srcFrame.pFrame,bkFrame.nFrameSize);
		bkFrame.nWidth=srcFrame.nWidth;
		bkFrame.nHeight=srcFrame.nHeight;
		bkFrame.ePixFmt=srcFrame.ePixFmt;
		bkFrame.fFps=srcFrame.fFps;
		bkFrame.lPts=srcFrame.lPts;
		bkFrame.dwFrameTime=srcFrame.dwFrameTime;
		bkFrame.eMediaType=srcFrame.eMediaType;
		bkFrame.eSampleFmt=srcFrame.eSampleFmt;
		bkFrame.lChanels=srcFrame.lChanels;
		bkFrame.nSampleRate=srcFrame.nSampleRate;
		return 0;
	}
	return -1;
}

int GetVideoFrameAndBackup(CContex* ctx,MediaFrameInfo_S &destFrame,MediaFrameInfo_S &bkFrame){
	int ret=-1;
	if (ctx==NULL)
	{
		return ret;
	}
	bool getVideoFrame=false;
	while (!getVideoFrame)
	{
		ret=ctx->GetFrame(&destFrame);
		if (ret<0)
		{
			std::cout<<"get frame failed"<<std::endl;
			return -1;
		}
		if (destFrame.eMediaType!=MEDIA_TYPE_VIDEO)
		{
			continue;
		}
		getVideoFrame=true;
		break;
	}
	return BackupFrame(destFrame,bkFrame);
}

//转场
int32_t TrainsitionDemo(const char* pEffectPath){
	//int64_t firstPts=-1;
	double firstPts=-1;
	//transition
	uint32_t transOffset=1;
	uint32_t transDuration=1;

	CContex* pMainDecCtx= new CContex();
	assert(pMainDecCtx);
	CContex* pSecDecCtx= new CContex();
	assert(pSecDecCtx);

	//const char* main_file="F:\\test_file\\fengjing\\03.mp4";
	const char* main_file="..\\material\\1.mp4";
	if (pMainDecCtx->OpenFile(main_file)<0)
	{
		std::cout<<"open main file failed:"<<main_file<<std::endl;
		return -1;
	}
	/*const char* second_file="F:\\test_file\\fengjing\\t4.mp4";*/
	const char* second_file="..\\material\\2.mp4";
	if (pSecDecCtx->OpenFile(second_file)<0)
	{
		std::cout<<"open second file failed:"<<second_file<<std::endl;
		return -1;
	}
	bool init=false;
	CRenderTransition* pRenderObj=new CRenderTransition();
	assert(pRenderObj);
	MediaFrameInfo_S mainFrame;
	MediaFrameInfo_S mainBkFrame;//备份一帧main的解码数据
	MediaFrameInfo_S secondFrame;
	MediaFrameInfo_S secondBkFrame;//用于备份一帧second的解码数据
	memset(&mainFrame,0,sizeof(MediaFrameInfo_S));
	memset(&mainBkFrame,0,sizeof(MediaFrameInfo_S));
	memset(&secondFrame,0,sizeof(MediaFrameInfo_S));
	memset(&secondBkFrame,0,sizeof(MediaFrameInfo_S));

	if (!init)
	{
		if (GetVideoFrameAndBackup(pMainDecCtx,mainFrame,mainBkFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
		if (GetVideoFrameAndBackup(pSecDecCtx,secondFrame,secondBkFrame)<0)
		{
			std::cout<<"second get frame failed"<<std::endl;
			return -1;
		}
		/*const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\transitions\\GlitchDisplace.glsl";*/
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\transitions\\circleopen.glsl";
		if (pRenderObj->SetupGL(mainBkFrame.nWidth/4,mainBkFrame.nHeight/4,mainBkFrame.nWidth,mainBkFrame.nHeight,pEffectPath)<0)
		{
			std::cout<<"setup GL failed"<<std::endl;
			return -1;
		}
		pRenderObj->CreateTransTexture(mainBkFrame.nWidth,mainBkFrame.nHeight);
		std::cout<<"fps:"<<mainBkFrame.fFps<<std::endl;
	}

	if (firstPts==-1)
	{
		//firstPts=mainBkFrame.lPts;
		firstPts=mainBkFrame.dwFrameTime;
	}

	int32_t second_ret=0;
	int32_t main_ret=0;
	while (second_ret==0)
	{
		//float fps_time=1.0/mainBkFrame.fFps;
		//const float ts = ((float)(mainBkFrame.lPts - firstPts) / FRAME_TIME_BASE) - transOffset;
		const float ts = (float)(mainBkFrame.dwFrameTime - firstPts) - transOffset;
		//progress的计算值:ts<0,progress=0;ts>1,progress=1;0<ts<1,progress=ts;
		const float progress = FFMAX(0.0f, FFMIN(1.0f, ts / transDuration));
		std::cout<<"main pts:"<<mainBkFrame.dwFrameTime<<"; firs pts:"<<firstPts<<"; ts:"<<ts<<"; progress:"<<progress<<std::endl;

		pRenderObj->Render(&mainBkFrame,&secondBkFrame,progress);
		Sleep(1);
		if (main_ret>=0)
		{
			main_ret=GetVideoFrameAndBackup(pMainDecCtx,mainFrame,mainBkFrame);
			/*main_ret=pMainDecCtx->GetFrame(&mainFrame);
			if (main_ret>=0)
			{
				BackupFrame(mainFrame,mainBkFrame);
			}*/
		}
		if (progress>=0)
		{
			second_ret = GetVideoFrameAndBackup(pSecDecCtx,secondFrame,secondBkFrame);
			/*second_ret=pSecDecCtx->GetFrame(&secondFrame);
			if (second_ret>=0)
			{
				BackupFrame(secondFrame,secondBkFrame);
			}*/
		}
	}
	return 0;
}


//单个effect
int32_t VideoEffectDemo(const char* pEffectPath){
	int64_t firstPts=-1;
	//transition
	uint32_t transOffset=1;
	uint32_t transDuration=1;
	CContex* pMainDecCtx= new CContex();
	assert(pMainDecCtx);

	//const char* main_file="F:\\test_file\\fengjing\\03.mp4";
	const char* main_file="..\\material\\1.mp4";
	if (pMainDecCtx->OpenFile(main_file)<0)
	{
		std::cout<<"open main file failed:"<<main_file<<std::endl;
		return -1;
	}
	bool init=false;
	CRender* pRenderObj=new CRender();
	assert(pRenderObj);
	MediaFrameInfo_S mainFrame;
	MediaFrameInfo_S mainBkFrame;//备份一帧main的解码数据
	memset(&mainFrame,0,sizeof(MediaFrameInfo_S));
	memset(&mainBkFrame,0,sizeof(MediaFrameInfo_S));
	float time=pRenderObj->GetTime();
	if (!init)
	{
		if (GetVideoFrameAndBackup(pMainDecCtx,mainFrame,mainBkFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\circle.glsl";
		if (pRenderObj->SetupGL(mainBkFrame.nWidth/4,mainBkFrame.nHeight/4,mainBkFrame.nWidth,mainBkFrame.nHeight,pEffectPath)<0)
		{
			std::cout<<"setup GL failed"<<std::endl;
			return -1;
		}
		//创建纹理对象
		pRenderObj->CreateTexture(mainBkFrame.nWidth,mainBkFrame.nHeight);

		std::cout<<"fps:"<<mainBkFrame.fFps<<std::endl;
		pRenderObj->GetShader()->setVec2("resolution",mainBkFrame.nWidth/4,mainBkFrame.nHeight/4);
	}

	if (firstPts==-1)
	{
		firstPts=mainBkFrame.lPts;
	}

	int32_t main_ret=0;
	while (main_ret>=0)
	{
		//float fps_time=1.0/mainBkFrame.fFps;
		const float ts = ((float)(mainBkFrame.lPts - firstPts) / FRAME_TIME_BASE) - transOffset;
		//progress的计算值:ts<0,progress=0;ts>1,progress=1;0<ts<1,progress=ts;
		const float progress = FFMAX(0.0f, FFMIN(1.0f, ts / transDuration));
		std::cout<<"main pts:"<<mainBkFrame.lPts<<"; firs pts:"<<firstPts<<"; ts:"<<ts<<"; progress:"<<progress<<std::endl;
		time=pRenderObj->GetTime();
		std::cout<<"time:"<<time<<std::endl;
		pRenderObj->GetShader()->setFloat("time",time);

		pRenderObj->Render(&mainBkFrame);
		Sleep(100);
		if (GetVideoFrameAndBackup(pMainDecCtx,mainFrame,mainBkFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
	}
	return 0;
}

//图片
int32_t ImageEffectDemo(const char* pEffectPath){
	int64_t firstPts=-1;

	//const char* main_file="F:\\test_file\\text_template\\213123213_00088\\1.png";
	const char* main_file="..\\material\\01.png";
	ImageHelper* pMainImage= new ImageHelper(main_file,PF_RGB32);
	assert(pMainImage);

	bool init=false;
	CRender* pRenderObj=new CRender();
	assert(pRenderObj);
	MediaFrameInfo_S mainFrame;
	memset(&mainFrame,0,sizeof(MediaFrameInfo_S));
	float time=pRenderObj->GetTime();
	if (!init)
	{
		if (pMainImage->GetFrame(&mainFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
		//const char* pEffectPath = NULL;
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\circle.glsl";
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\image_alpha.glsl";
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\brightness.glsl";
		//const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\split_screen.frag";

		if (pRenderObj->SetupGL(mainFrame.nWidth,mainFrame.nHeight,mainFrame.nWidth,mainFrame.nHeight,pEffectPath)<0)
		{
			std::cout<<"setup GL failed"<<std::endl;
			return -1;
		}
		//创建纹理对象
		pRenderObj->CreateTexture(mainFrame.nWidth,mainFrame.nHeight);

		pRenderObj->GetShader()->setVec2("resolution",mainFrame.nWidth,mainFrame.nHeight);
		//pRenderObj->GetShader()->setFloat("brightness",0);
	}
	Sleep(1000);
	int32_t main_ret=0;
	float brightness_factor=-1;
	while (main_ret>=0)
	{
		time=pRenderObj->GetTime();
		std::cout<<"time:"<<time<<std::endl;
		pRenderObj->GetShader()->setFloat("time",time);
		brightness_factor+=0.05;
		pRenderObj->GetShader()->setFloat("brightness",brightness_factor);

		pRenderObj->Render(&mainFrame);
		Sleep(500);
		main_ret=pMainImage->GetFrame(&mainFrame);
	}
	return 0;
}

//提取rgba中alpha通道上的颜色值，然后回去alpha包围的区域矩形
void GetAlphaRect(){
	const char* main_file="F:\\test_file\\text_template\\213123213_00088\\1.png";
	ImageHelper* pMainImage= new ImageHelper(main_file,PF_RGB32);
	assert(pMainImage);
	MediaFrameInfo_S mainFrame;
	memset(&mainFrame,0,sizeof(MediaFrameInfo_S));
	if (pMainImage->GetFrame(&mainFrame)<0)
	{
		std::cout<<"main get frame failed"<<std::endl;
		return;
	}
	for (int y = 0; y <= mainFrame.nHeight -1; y++) 
	{ 
		for (int x = 0; x <= mainFrame.nWidth -1; x++) 
		{ 
			//获取alpha的值
			uint8_t a=mainFrame.pFrame[y*mainFrame.nWidth*4+x+3];//
			if (a==0)
			{
				std::cout<<"x:"<<x<<";y:"<<y<<std::endl;
			}
		} 
	} 

}




void showUsge(){
	std::string strHelp("\
-t  video transition effect demo\n\
-e  image effect demo\n\
-v  video effect demo\n\
-xx fragment shader file\n\
\n");
	std::cout<<strHelp<<std::endl;
}

int parseOptions(const char *opt){
	int demoIndex = -1;
	if(strcmp(opt,"-t")==0){
		demoIndex = 0;
	}else if (strcmp(opt,"-e")==0){
		demoIndex = 1;
	}else if (strcmp(opt,"-v")==0){
		demoIndex = 2;
	}
	return demoIndex;
}


int main(int argc, char **argv){
	int demoIndex=-1;
	const char *opt=NULL;
	int optIndex=1;
	const char *fragPath=NULL;
	if(argc!=3){
		showUsge();
		return 0;
	}
	opt=argv[optIndex++];
	demoIndex = parseOptions(opt);
	opt = argv[optIndex++];
	fragPath= opt;

	switch(demoIndex){
	case 0:
		TrainsitionDemo(fragPath);
		break;
	case 1:
		ImageEffectDemo(fragPath);
		break;
	case 2:
		VideoEffectDemo(fragPath);
		break;
	default:
		showUsge();
	}
	
	//GetAlphaRect();
	system("pause");
	return 0;
}
