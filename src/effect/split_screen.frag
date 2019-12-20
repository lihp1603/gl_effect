//ʵ��һ���������˾�Ч��
uniform float horizontal=2;  // (1) ������������
uniform float vertical=3; //�����зֳɵķ�������

vec4 effect (vec2 uv)                          
{                    
	float horizontalCount = max(horizontal,1.0);
	float verticalCount = max(vertical,1.0);
	float ratio= verticalCount / horizontalCount;
	vec2 originSize = vec2(1.0,1.0);//ԭͼ��Ĵ�С�ߴ磬����ָ�������С
	vec2 newSize = originSize;   //���ڴ洢����֮��������Ҫչʾ��ͼ�������С
	if(ratio>1.0){//˵������չʾ�������Ⱥ�����Ҫ�࣬����ѡ���ú�������չʾ����ԭͼ���������вü�
		newSize.y = 1.0/ratio;
	}else{//˵������չʾ�������϶�
		newSize.x = ratio;
	}       
	//��Ҫ��չʾ��ͼ���С��ԭͼ���С��֪���ˣ����ǾͿ��Լ���������Ҫ�ü���ͼ���С�ߴ�
	vec2 cropSize = originSize - newSize;
	//��Ϊ��Ҫͼ����вü�������������Ҫ�ü���ƫ�ƴ�С��Ҫ/2
	vec2 offset = cropSize/2;
    //�������Ǽ��������������������
	//���ǽ�ԭʼ���������꣬���� horizontalCount �� verticalCount �Ľ�С�ߣ�Ȼ����µĳߴ������ģ���㡣
	//��������ԭʼ���������� 0 ~ 1 �ķ�Χ������ʱ���������µ����������� newSize �ķ�Χ��ѭ����Ρ�
	//���⣬����Ľ������ offset���������µ���������ƫ�Ƶ����е�λ��
	vec2 position = offset + mod(uv*min(horizontalCount,verticalCount),newSize);
	//����
	vec4 _finalColor = getFromColor(position);                  
	 
	return  _finalColor;       
}                                      
