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
		return 0;
	}
	return -1;
}

//转场
int32_t TrainsitionDemo(){
	int64_t firstPts=-1;
	//transition
	uint32_t transOffset=1;
	uint32_t transDuration=1;

	CContex* pMainDecCtx= new CContex();
	assert(pMainDecCtx);
	CContex* pSecDecCtx= new CContex();
	assert(pSecDecCtx);

	const char* main_file="F:\\test_file\\fengjing\\03.mp4";
	if (pMainDecCtx->OpenFile(main_file)<0)
	{
		std::cout<<"open main file failed:"<<main_file<<std::endl;
		return -1;
	}
	const char* second_file="F:\\test_file\\fengjing\\t4.mp4";
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
		if (pMainDecCtx->GetFrame(&mainFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
		if (pSecDecCtx->GetFrame(&secondFrame)<0)
		{
			std::cout<<"second get frame failed"<<std::endl;
			return -1;
		}
		BackupFrame(mainFrame,mainBkFrame);
		BackupFrame(secondFrame,secondBkFrame);
		const char* pTransitionPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\transitions\\GlitchDisplace.glsl";
		if (pRenderObj->SetupGL(mainBkFrame.nWidth/4,mainBkFrame.nHeight/4,mainBkFrame.nWidth,mainBkFrame.nHeight,pTransitionPath)<0)
		{
			std::cout<<"setup GL failed"<<std::endl;
			return -1;
		}
		std::cout<<"fps:"<<mainBkFrame.fFps<<std::endl;
	}

	if (firstPts==-1)
	{
		firstPts=mainBkFrame.lPts;
	}

	int32_t second_ret=0;
	int32_t main_ret=0;
	while (second_ret==0)
	{
		//float fps_time=1.0/mainBkFrame.fFps;
		const float ts = ((float)(mainBkFrame.lPts - firstPts) / FRAME_TIME_BASE) - transOffset;
		//progress的计算值:ts<0,progress=0;ts>1,progress=1;0<ts<1,progress=ts;
		const float progress = FFMAX(0.0f, FFMIN(1.0f, ts / transDuration));
		std::cout<<"main pts:"<<mainBkFrame.lPts<<"; firs pts:"<<firstPts<<"; ts:"<<ts<<"; progress:"<<progress<<std::endl;

		pRenderObj->Render(&mainBkFrame,&secondBkFrame,progress);
		Sleep(1);
		if (main_ret>=0)
		{
			main_ret=pMainDecCtx->GetFrame(&mainFrame);
			if (main_ret>=0)
			{
				BackupFrame(mainFrame,mainBkFrame);
			}
		}
		if (progress>=0)
		{
			second_ret=pSecDecCtx->GetFrame(&secondFrame);
			if (second_ret>=0)
			{
				BackupFrame(secondFrame,secondBkFrame);
			}
		}
	}
	return 0;
}


//单个effect
int32_t EffectDemo(){
	int64_t firstPts=-1;
	//transition
	uint32_t transOffset=1;
	uint32_t transDuration=1;
	CContex* pMainDecCtx= new CContex();
	assert(pMainDecCtx);

	const char* main_file="F:\\test_file\\fengjing\\03.mp4";
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
		if (pMainDecCtx->GetFrame(&mainFrame)<0)
		{
			std::cout<<"main get frame failed"<<std::endl;
			return -1;
		}
		BackupFrame(mainFrame,mainBkFrame);
		const char* pEffectPath = "F:\\Media\\OpenGL\\dev\\gl_effect\\src\\effect\\circle.glsl";
		if (pRenderObj->SetupGL(mainBkFrame.nWidth/4,mainBkFrame.nHeight/4,mainBkFrame.nWidth,mainBkFrame.nHeight,pEffectPath)<0)
		{
			std::cout<<"setup GL failed"<<std::endl;
			return -1;
		}
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
		main_ret=pMainDecCtx->GetFrame(&mainFrame);
		if (main_ret>=0)
		{
			BackupFrame(mainFrame,mainBkFrame);
		}
	}
	return 0;
}

int main(){
	
	//TrainsitionDemo();
	EffectDemo();
	system("pause");
	return 0;
}
