uniform vec2 resolution;
uniform float time;
uniform  float brightness;

float Constrain(float color_value)
{
	// Constrain new color from 0 to 1
	if (color_value < 0.0)
		color_value = 0.0;
	else if (color_value > 1.0)
		color_value = 1.0;

	return color_value;
}

vec4 effect (vec2 uv) {
	vec4 srcColor=getFromColor(uv);
	//调节rgb的值，来调节亮度，值变小，亮度变暗，值变大，亮度变大
	float sR=Constrain(srcColor.r+1*brightness);
	float sG=Constrain(srcColor.g+1*brightness);
	float sB=Constrain(srcColor.b+1*brightness);

	return vec4(sR,sG,sB,srcColor.a);
}

