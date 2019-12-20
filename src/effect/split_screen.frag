//实现一个分屏的滤镜效果
uniform float horizontal=2;  // (1) 横屏分屏数量
uniform float vertical=3; //竖屏切分成的分屏数量

vec4 effect (vec2 uv)                          
{                    
	float horizontalCount = max(horizontal,1.0);
	float verticalCount = max(vertical,1.0);
	float ratio= verticalCount / horizontalCount;
	vec2 originSize = vec2(1.0,1.0);//原图像的大小尺寸，这里指的纹理大小
	vec2 newSize = originSize;   //用于存储分屏之后，我们需要展示的图像纹理大小
	if(ratio>1.0){//说明竖屏展示的数量比横屏的要多，所以选择让横屏完整展示，将原图像的竖向进行裁剪
		newSize.y = 1.0/ratio;
	}else{//说明横屏展示的数量较多
		newSize.x = ratio;
	}       
	//需要新展示的图像大小和原图像大小都知道了，我们就可以计算我们需要裁剪的图像大小尺寸
	vec2 cropSize = originSize - newSize;
	//因为需要图像居中裁剪，所以我们需要裁剪的偏移大小需要/2
	vec2 offset = cropSize/2;
    //这里我们计算纹理采样的纹理坐标
	//我们将原始的纹理坐标，乘上 horizontalCount 和 verticalCount 的较小者，然后对新的尺寸进行求模运算。
	//这样，当原始纹理坐标在 0 ~ 1 的范围内增长时，可以让新的纹理坐标在 newSize 的范围内循环多次。
	//另外，计算的结果加上 offset，可以让新的纹理坐标偏移到居中的位置
	vec2 position = offset + mod(uv*min(horizontalCount,verticalCount),newSize);
	//采样
	vec4 _finalColor = getFromColor(position);                  
	 
	return  _finalColor;       
}                                      
