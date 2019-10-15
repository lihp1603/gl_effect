                
uniform vec2 TexSize;                
vec2 mosaicSize = vec2(8,8);    

vec4 effect (vec2 uv)                          
{                                    
	vec2 intXY = vec2(uv.x*TexSize.x, uv.y*TexSize.y);     
	vec2 XYMosaic = vec2(floor(intXY.x/mosaicSize.x)*mosaicSize.x,floor(intXY.y/mosaicSize.y)*mosaicSize.y) + 0.5*mosaicSize;  
	vec2 delXY = XYMosaic - intXY;    
	float delL = length(delXY);       
	vec2 UVMosaic = vec2(XYMosaic.x/TexSize.x,XYMosaic.y/TexSize.y);  
	vec4 _finalColor;                 
	if(delL< 0.5*mosaicSize.x)        
		_finalColor = getFromColor(UVMosaic);   
	else                              
		_finalColor = getFromColor(uv);      
	return  _finalColor;       
}                                      
