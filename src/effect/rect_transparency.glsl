                
uniform vec2 TexSize;                
uniform vec2 rect_start ;
uniform vec2 rect_wh; 

//写一个判断这个点是否在矩形框内部的判断函数
bool rectangle_inside(vec2 start,vec2 wh,vec2 point){
	vec2 end= start+wh;
	if(point.x >= start.x && point.y >= start.y && point.x <= end.x && point.y <= end.y){
		return true;
	}
	return false;
}

vec4 effect (vec2 uv)                          
{                                           
	vec4 _finalColor = getFromColor(uv);   
	vec2 intXY = vec2(uv.x*TexSize.x, uv.y*TexSize.y);
	//判断这个像数点位置  
	if(!rectangle_inside(rect_start,rect_wh,intXY))        
		_finalColor.a = 0.2;//不透明                    
	//else 
		//_finalColor.a = 1; 
		
		

	return  _finalColor;       
}                                      
