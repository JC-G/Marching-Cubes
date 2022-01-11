float ellipsoid_density(vec3 inPos, vec4 location, vec4 radius) {
    //from https://iquilezles.org/www/articles/ellipsoids/ellipsoids.htm
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return (k1-1.0) * min(min(radius.x,radius.y),radius.z);
}

vec3 ellipsoid_normal(vec3 inPos, vec4 location, vec4 radius) {
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return normalize(1.0/k1 * (inPos.xyz - location.xyz) / (radius.xyz * radius.xyz));
}

float cylinder_density(vec3 inPos, vec4 a, vec4 b, float r) {
    //https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
    vec3  ba = b.xyz - a.xyz;
    vec3  pa = inPos - a.xyz;
    float baba = dot(ba,ba);
    float paba = dot(pa,ba);
    float x = length(pa*baba-ba*paba) - r*baba;
    float y = abs(paba-baba*0.5)-baba*0.5;
    float x2 = x*x;
    float y2 = y*y*baba;
    float d = (max(x,y)<0.0)?-min(x2,y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
    return sign(d)*sqrt(abs(d))/baba;
}

vec3 cylinder_normal(vec3 inPos, vec4 a, vec4 b, float r) {
    //numerical normal - TODO get analytical normal 
    float eps = 0.001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f  = cylinder_density(inPos,a,b,r);
    float fx = cylinder_density(dx,a,b,r);
    float fy = cylinder_density(dy,a,b,r);
    float fz = cylinder_density(dz,a,b,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}

// Bezier code adapted from https://www.shadertoy.com/view/ltXSDB
// Solve cubic equation for roots
vec3 solveCubic(float a, float b, float c)
{
    float p = b - a*a / 3.0, p3 = p*p*p;
    float q = a * (2.0*a*a - 9.0*b) / 27.0 + c;
    float d = q*q + 4.0*p3 / 27.0;
    float offset = -a / 3.0;
    if(d >= 0.0) { 
        float z = sqrt(d);
        vec2 x = (vec2(z, -z) - q) / 2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        return vec3(offset + uv.x + uv.y);
    }
    float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
    float m = cos(v), n = sin(v)*1.732050808;
    return vec3(m + m, -n - m, n - m) * sqrt(-p / 3.0) + offset;
}

// Find the signed distance from a point to a bezier curve
float sdBezier(vec3 A, vec3 B, vec3 C, vec3 p)
{    
    B = mix(B + vec3(1e-4), B, abs(sign(B * 2.0 - A - C)));
    vec3 a = B - A, b = A - B * 2.0 + C, c = a * 2.0, d = A - p;
    vec3 k = vec3(3.*dot(a,b),2.*dot(a,a)+dot(d,b),dot(d,a)) / dot(b,b);      
    vec3 t = clamp(solveCubic(k.x, k.y, k.z), 0.0, 1.0);
    vec3 pos = A + (c + b*t.x)*t.x;
    float dis = length(pos - p);
    pos = A + (c + b*t.y)*t.y;
    dis = min(dis, length(pos - p));
    pos = A + (c + b*t.z)*t.z;
    dis = min(dis, length(pos - p));
    return dis;
}

float bezier_density(vec3 inPos, vec4 A, vec4 B, vec4 C, float r) {
    return sdBezier(A.xyz,B.xyz,C.xyz,inPos) - r;
}

vec3 bezier_normal(vec3 inPos, vec4 A, vec4 B, vec4 C, float r) {
    float eps = 0.001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f  = bezier_density(inPos,A,B,C,r);
    float fx = bezier_density(dx,A,B,C,r);
    float fy = bezier_density(dy,A,B,C,r);
    float fz = bezier_density(dz,A,B,C,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}