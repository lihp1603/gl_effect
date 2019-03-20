
vec4 effect (vec2 uv) {
  //获取原图的rgba值
  vec4 tex_rgba=getFromColor(uv);
  if(tex_rgba.a<=0.1){
     tex_rgba=vec4(1.0f,0.0f,0.0f,1.0f);
  }
  return tex_rgba;
  }
 