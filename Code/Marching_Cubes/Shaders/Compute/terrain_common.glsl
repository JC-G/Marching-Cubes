const float HASHSCALE1 = .1031;
float hash12(vec2 p)
{
    vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 noised2(vec2 x)
{
    vec2 f = fract(x);
    vec2 u = f*f*(3.0-2.0*f);

    vec2 p = floor(x);
    float a = hash12(p+vec2(0,0));
    float b = hash12(p+vec2(1,0));
    float c = hash12(p+vec2(0,1));
    float d = hash12(p+vec2(1,1));
    return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,
            6.0*f*(1.0-f)*(vec2(b-a,c-a)+vec2(a-b-c+d)*u.yx));
}
//returns noise with a height of 1, over a grid of 1
float terrain(vec2 p) { //in xz, out y
    float total = 0.0;
    float max = 0.0;
    float heightScale = 0.5;
    int octaves = 7;
    for (int i = 0; i < octaves; i++) {
        float n = noised2(p).x;
        total += n * heightScale;
        max += heightScale;
        heightScale *= 0.5;
        p = 2.0 * p;
    }
    return total/max;
}

vec2 terrainDerivative(vec2 p) {
    float max = 0.0;
    float heightScale = 1.0;
    int octaves = 7;
    vec2 terrDer = vec2(0);
    for (int i = 0; i < octaves; i++) {
        max += heightScale;
        terrDer += noised2(p).yz;
        p = 2.0 * p;
        heightScale *= 0.5;
    }
    return terrDer;
}