uniform vec2 TexSize;

vec4 effect (vec2 uv)                
{                           
   vec2 texCoord =uv;  
	vec2 upLeftUV = vec2(texCoord.x-1.0/TexSize.x,texCoord.y-1.0/TexSize.y);           
	vec4 srcColor = getFromColor(uv);                           
	vec4 upLeftColor = getFromColor(upLeftUV);                  
	vec4 delColor = srcColor - upLeftColor;                           
	float h = 0.3*delColor.x + 0.59*delColor.y + 0.11*delColor.z;                  
   	vec4 bkColor = vec4(0.5, 0.5, 0.5, 1.0);                  
	return vec4(h,h,h,0.0) +bkColor;                             
}                           