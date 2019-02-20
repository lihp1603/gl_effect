uniform float time;
uniform vec2 resolution;

vec4 color_;
vec4 col;

vec2 st;

void setColor(vec4 color)
{
  color_ = color;
}

vec4 circle(float x, float y, float radius)
{
  vec2 pos = vec2(x, y);
  float dist = distance(st, pos);//求两个点的距离
  float index = floor(dist);//向下取整
  dist = fract(time * 0.2 - dist * 3.0) + 0.4;//fract 取小数

  //type smoothstep(type edge0, type edge1, type x) 如果x<edge0,返回0，如果x>edge1,返回1，否则返回0-1.0之间的一个Hermite 算法插值
  dist = 1.0 - smoothstep(radius - 0.01, radius + 0.01, dist); //
 
  if(dist==1.0){
     vec4 col1 = vec4(vec3(0,0,0), 1);
    return col1;
  }
  return color_;

  //vec4 col = vec4(vec3(dist), 1)*color_;
  //return col;
}

vec4 effect (vec2 uv) {
  // If resolution.x > resolution.y, pos.x ~ (-1.xx, 1.xx), pos.y ~ (-1, 1)
  st = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);
  //setColor(vec4((st.x + 1.0)/2.0, (st.y + 1.0)/2.0, abs(sin(time)), 1.0));
  setColor(getFromColor(uv));
  //setColor(vec4(0,0,1,1));
  col = circle(0.0, 0.0, 0.5);
  return col;
  //return mix(getFromColor(uv),circle(0.0, 0.0, 0.5),0.1);
}