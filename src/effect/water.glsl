#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;

const float PI = 3.141592;

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
	vec2 a2 = vec2(sin(ang.y),cos(ang.y));
	vec2 a3 = vec2(sin(ang.z),cos(ang.z));
	mat3 m;
	m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}

float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
	return fract(sin(h)*43758.5453123);
}

float noise( in vec2 p ) {
	vec2 i = floor( p );
	vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
	return -1.0 + 2.0 * mix( 
		mix( hash( i + vec2(0.0,0.0) ), 
		     hash( i + vec2(1.0,0.0) ), u.x),
		mix( hash( i + vec2(0.0,1.0) ), 
		     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = max(e.y,0.0);
    return vec3(pow(1.0-e.y,5.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4);
}

// sea
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    //vec2 swv = abs(cos(uv));    
    //wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}


float map(vec3 p) {
	float freq = 0.16;
	float amp = 0.5;
	float choppy = 5.0;
	float stime = 2.0 + time * .6;
	
	float d, h = 0.0;    
	for(int i = 0; i < 8; i++) {        
		d = sea_octave((p.xz+stime)*freq,choppy);
		d += sea_octave((p.xz-stime * 1.3)*freq,choppy);
		d += sea_octave((p.xz+stime * 2.2)*freq,choppy);
		h += d * amp;        
		p.xz *= vec2(1.5,-1.71);
		freq *= 3.7;
		amp *= .15;
		choppy = mix(choppy,1.0,0.1);
	}
	return p.y - h;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
    vec3 n;
    n.y = map(p);    
    n.x = map(vec3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map(vec3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}


vec3 trace(vec3 ori, vec3 dir) {  
	float tm = 0.0;
	float tx = 1000.0;    
	float hx = map(ori + dir * tx);
	if(hx > 0.0) return vec3(tx);   
	float hm = map(ori + dir * tm);    
	float tmid = 0.0;
	vec3 p;
	for(int i = 0; i < 8; i++) {
		tmid = mix(tm,tx, hm/(hm-hx));                   
		p = ori + dir * tmid;                   
		float hmid = map(p);
			if(hmid < 0.0) {
			tx = tmid;
		    hx = hmid;
		} else {
		    tm = tmid;
		    hm = hmid;
		}
	}
    	return p;
}

const vec3 SEA_WATER_COLOR = vec3(0.9,0.8,0.6);
vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.55;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = vec3(0.1,0.19,0.22) + diffuse(n,l,80.0) * SEA_WATER_COLOR;
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 1.6);
    color += SEA_WATER_COLOR * (p.y - 0.5) * 0.18 * atten;
    
    return color;
}


// main
void main( void ) {
	//归一化处理，将xy的坐标变换为[0,1]范围
	vec2 uv = gl_FragCoord.xy / resolution.xy;
	//将[0,1]变换为[-1,1]
	uv = uv * 2.0 - 1.0;
	uv.x *= resolution.x / resolution.y;    
	
	// ray
	vec3 ori = vec3(0.0, 10.5, 1.0);
	vec3 dir = normalize(vec3(uv.xy,-2.0)); 
	//dir.z += length(uv) * 0.45;
	
	//vec3 ang = vec3(0.0, .4, .0);
	//dir = normalize(dir) * fromEuler(ang);
	
	// tracing
	vec3 p = trace(ori,dir);
	vec3 dist = p - ori;
	vec3 n = getNormal(p, dot(dist,dist) * (.2 / resolution.x));
	vec3 light = normalize(vec3(0.0,1.0,0.9)); 
	
	vec3 color = mix(
        	getSkyColor(dir),
        	getSeaColor(p,n,light,dir,dist),
    		pow(smoothstep(.0,-.05,dir.y),1.5));
	
	gl_FragColor = vec4(color, 1.0);
}