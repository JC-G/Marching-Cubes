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
    float eps = 0.00001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f = cylinder_density(inPos,a,b,r);
    float fx = cylinder_density(dx,a,b,r);
    float fy = cylinder_density(dy,a,b,r);
    float fz = cylinder_density(dz,a,b,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}