                
uniform vec2 TexSize;                
vec2 centre = vec2(960,540);
float radius = 400; 

vec4 effect (vec2 uv)                          
{                                    
	vec2 intXY = vec2(uv.x*TexSize.x, uv.y*TexSize.y);      
	vec2 delXY = centre - intXY;    
	float delL = length(delXY);       
	vec4 _finalColor = getFromColor(uv);                  
	if(delL > radius)        
		_finalColor.a = 0.1;                               
	else
		_finalColor.a = 1; 

	return  _finalColor;       
}                                      
