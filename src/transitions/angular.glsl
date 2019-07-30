// Author: Fernando Kuteken
// License: MIT

#define PI 3.141592653589

//起始偏移的旋转角度
uniform float startingAngle; // = 90;

vec4 transition (vec2 uv) {
  
  //将角度转换为弧度表示
  float offset = startingAngle * PI / 180.0;
  //将所有像素点的坐标移动(-0.5，-0.5)个位移，即将旋转中心从屏幕左上角变换为屏幕的中心点位置
  //
  float angle = atan(uv.y - 0.5, uv.x - 0.5) + offset;
  //float angle = atan(uv.y, uv.x) + offset;

  //这里进行归一化处理
  //angle + PI的处理是为了将旋转的起始位置进行调整
  float normalizedAngle = (angle + PI) / (2.0 * PI);
  //为了得到0-1的范围
  normalizedAngle = normalizedAngle - floor(normalizedAngle);

  //float step(float edge, float x)  : The step function returns 0.0 if x is smaller then edge and otherwise 1.0
  // edge<x: 0;   edge>=x:1;
  return mix(
    getFromColor(uv),
    getToColor(uv),
    step(normalizedAngle, progress)
    );
}
