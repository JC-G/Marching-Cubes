//from https://iquilezles.org/www/articles/ellipsoids/ellipsoids.htm

float ellipsoid_distance(vec3 inPos, vec4 location, vec4 radius) {
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return (k1-1.0) * min(min(radius.x,radius.y),radius.z);
}

vec3 ellipsoid_normal(vec3 inPos, vec4 location, vec4 radius) {
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return normalize(1.0/k1 * (inPos.xyz - location.xyz) / (radius.xyz * radius.xyz));
}

float cylinder_distance(vec3 inPos, vec4 a, vec4 b, float r) {
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

    float f  = cylinder_distance(inPos,a,b,r);
    float fx = cylinder_distance(dx,a,b,r);
    float fy = cylinder_distance(dy,a,b,r);
    float fz = cylinder_distance(dz,a,b,r);
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
    vec3 a = B - A, b = A - B * 2.0 + C, cc = a * 2.0, d = A - p;
    vec3 k = vec3(3.*dot(a,b),2.*dot(a,a)+dot(d,b),dot(d,a)) / dot(b,b);      
    vec3 t = clamp(solveCubic(k.x, k.y, k.z), 0.0, 1.0);
    vec3 pos = A + (cc + b*t.x)*t.x;
    float dis = length(pos - p);
    pos = A + (cc + b*t.y)*t.y;
    dis = min(dis, length(pos - p));
    pos = A + (cc + b*t.z)*t.z;
    dis = min(dis, length(pos - p));
    return dis;
}

float bezier_distance(vec3 inPos, vec4 A, vec4 B, vec4 C, float r) {
    return sdBezier(A.xyz,B.xyz,C.xyz,inPos) - r;
}

vec3 bezier_normal(vec3 inPos, vec4 A, vec4 B, vec4 C, float r) {
    //numerical normal of more complex distance function
    float eps = 0.001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f  = bezier_distance(inPos,A,B,C,r);
    float fx = bezier_distance(dx,A,B,C,r);
    float fy = bezier_distance(dy,A,B,C,r);
    float fz = bezier_distance(dz,A,B,C,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}

// Cubic bezier code adapted from https://www.shadertoy.com/view/flsGzH

const vec3 c = vec3(1.,0.,-1.);
const float pi = acos(-1.);

// Cubic bezier curve
vec3 B3(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3)
{
    float m1 = 1.-t;
    return m1*m1*m1*P0 + 3.*m1*t*(m1*P1+t*P2) + t*t*t*P3;
}

// Cubic bezier partial derivative with respect to t
vec3 B3Prime(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3)
{
    float m1 = 1.-t;
    return 3.*(m1*m1*(P1-P0)+2.*m1*t*(P2-P1)+t*t*(P3-P2));
}

// Cubic bezier second partial derivative with respect to t
vec3 B3Second(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3)
{
    float m1 = 1.-t;
    return 6.*m1*(P2-2.*P1+P0)+6.*t*(P3-2.*P2+P1);
}

// This is the "easy" representation of the quintic that has to be solved
// for the sdf.
float D3Prime(vec3 x, float  t, vec3 P0, vec3 P1, vec3 P2, vec3 P3)
{
    return dot(x-B3(t,P0,P1,P2,P3), B3Prime(t,P0,P1,P2,P3));
}

// Determine zeros of a*x^2+b*x+c
vec2 quadratic_zeros(float a, float b, float cc) {
    if(a == 0.) return -cc/b*c.xx;
    float d = b*b-4.*a*cc;
    if(d<0.) return vec2(1.e4);
    return (c.xz*sqrt(d)-b)/(2.*a);
}

// Determine zeros of a*x^3+b*x^2+c*x+d
vec3 cubic_zeros(float a, float b, float cc, float d) {
    if(a == 0.) return quadratic_zeros(b,cc,d).xyy;
    
    // Depress
    vec3 ai = vec3(b,cc,d)/a;
    
    //discriminant and helpers
    float tau = ai.x/3., 
        p = ai.y-tau*ai.x, 
        q = -tau*(tau*tau+p)+ai.z, 
        dis = q*q/4.+p*p*p/27.;
        
    //triple real root
    if(dis > 0.) {
        vec2 ki = -.5*q*c.xx+sqrt(dis)*c.xz, 
            ui = sign(ki)*pow(abs(ki), c.xx/3.);
        return vec3(ui.x+ui.y-tau);
    }
    
    //three distinct real roots
    float fac = sqrt(-4./3.*p), 
        arg = acos(-.5*q*sqrt(-27./p/p/p))/3.;
    return c.zxz*fac*cos(arg*c.xxx+c*pi/3.)-tau;
}

// Determine zeros of a*x^4+b*x^3+c*x^2+d*x+e
vec4 quartic_zeros(float a, float b, float cc, float d, float e) {
    if(a == 0.) return cubic_zeros(b, cc, d, e).xyzz;
    
    // Depress
    float _b = b/a,
        _c = cc/a,
        _d = d/a,
        _e = e/a;
        
    // Helpers
    float p = (8.*_c-3.*_b*_b)/8.,
        q = (_b*_b*_b-4.*_b*_c+8.*_d)/8.,
        r = (-3.*_b*_b*_b*_b+256.*_e-64.*_b*_d+16.*_b*_b*_c)/256.;
        
    // Determine available resolvent zeros
    vec3 res = cubic_zeros(8.,8.*p,2.*p*p-8.*r,-q*q);
    
    // Find nonzero resolvent zero
    float m = res.x;
    if(m == 0.) m = res.y;
    if(m == 0.) m = res.z;
    
    // Apply Ferrari method
    return vec4(
        quadratic_zeros(1.,sqrt(2.*m),p/2.+m-q/(2.*sqrt(2.*m))),
        quadratic_zeros(1.,-sqrt(2.*m),p/2.+m+q/(2.*sqrt(2.*m)))
    )-_b/4.;
}
//for returning the spline quintic zeros
struct vec5 {
    vec4 xyzw;
    float t;
};

vec5 solveSplineQuintic(in vec3 x, in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3) {
    //TODO - maybe obsolete this?
    // Use relative coordinates to eliminate all terms containing p0.
    x -= p0;
    p1 -= p0;
    p2 -= p0;
    p3 -= p0;
    p0 = c.yyy;
    
    // Use interval approximation to determine a numerical solution for the quintic.
    // TODO: find something better, I really would like an analytical approach
    float tmin = -0.5, tmax = 1.5, tnew, 
        dmin = D3Prime(x,tmin,p0,p1,p2,p3),
        dmax = D3Prime(x,tmax,p0,p1,p2,p3),
        dnew;
    
    for(int i=0; i<20; ++i)
    {
        tnew = mix(tmin, tmax, .5);
        dnew = D3Prime(x,tnew,p0,p1,p2,p3);
        
        if(dnew>0.)
        {
            tmin = tnew;
            dmin = dnew;
        }
        else 
        {
            tmax = tnew;
            dmax = dnew;
        }
    }
    
    // Determine coefficients of quintic equation.
    vec3 pa = p2-p1;
    float a5 = -dot(p3,p3)+3.*dot(pa,2.*p3-3.*pa),
        a4 = 5.*dot(p1-pa,p3)+15.*dot(pa,pa-p1),
        a3 = -6.*dot(p2,p2)+4.*dot(p1,9.*pa-p3),
        a2 = dot(p3-3.*pa,x)+9.*dot(p1,p1-pa),
        a1 = 2.*dot(pa-p1,x)-3.*dot(p1,p1),
        a0 = dot(p1,x);
    
    // Polynomial division of numerical solution.
    float _a = a5,
        _b = a4+_a*tmin,
        _c = a3+_b*tmin,
        _d = a2+_c*tmin,
        _e = a1+_d*tmin;
        
    vec4 t = clamp(quartic_zeros(_a,_b,_c,_d,_e),0.,1.);
    return vec5(t,tmin);

}

// minimum distance to a cubic spline with the following strategy:
float dcubic_spline(in vec3 x, in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3)
{
    vec5 result = solveSplineQuintic(x,p0,p1,p2,p3);
    float tmin = result.t;
    vec4 t=result.xyzw;

    tmin = clamp(tmin, 0.,1.);
    
    return min(
        length(x-B3(tmin,p0,p1,p2,p3)),
        min(
            min(
                length(x-B3(t.x,p0,p1,p2,p3)),
                length(x-B3(t.y,p0,p1,p2,p3))
            ),
            min(
                length(x-B3(t.z,p0,p1,p2,p3)),
                length(x-B3(t.w,p0,p1,p2,p3))
            )
        )
    );
}

vec3 d3cubic_spline(in vec3 x, in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3) {
    vec5 result = solveSplineQuintic(x,p0,p1,p2,p3);
    float tmin = result.t;
    vec4 t=result.xyzw;
    if (tmin > 1.0 || tmin < 0.0) {
        tmin = t.x;
    }

    float lMin = length(x-B3(tmin,p0,p1,p2,p3));
    vec3 vMin = x-B3(tmin,p0,p1,p2,p3);
    for (int i = 0; i < 4; i++) {
        if (length(x-B3(t[i],p0,p1,p2,p3)) < lMin) {
            vMin = x-B3(t[i],p0,p1,p2,p3);
            lMin = length(x-B3(t[i],p0,p1,p2,p3));
        }
    }
    return vMin;
}
float cubic_bezier_distance(vec3 inPos, vec4 A, vec4 B, vec4 C, vec4 D, float r) {
    return dcubic_spline(inPos, A.xyz,B.xyz,C.xyz,D.xyz)-r;
}

vec3 cubic_bezier_normal(vec3 inPos, vec4 A, vec4 B, vec4 C, vec4 D, float r) {
    float eps = 0.001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f  = cubic_bezier_distance(inPos,A,B,C,D,r);
    float fx = cubic_bezier_distance(dx,A,B,C,D,r);
    float fy = cubic_bezier_distance(dy,A,B,C,D,r);
    float fz = cubic_bezier_distance(dz,A,B,C,D,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}

vec3 capsuleDerivative(vec3 p, vec3 a, vec3 b, float r) {
    vec3 pa = p - a, ba = b - a;
    pa *= vec3(1.,5.,1.); ba *=vec3(1.,5.,1.);
    float h = clamp(dot(pa,ba)/dot(ba,ba),0.0, 1.0);
    return normalize((pa - ba*h) * vec3(1.,5.,1.));
}

//SDF of capsule between a and b, with radius r
float sdCapsule( vec3 p, vec3 a, vec3 b, float r ) {
    vec3 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
  return length( pa - ba*h ) - r;
}

//sdf of plane with normal n, passing through p0, offset in normal direction by h
float sdPlane(vec3 inPos, vec3 n, vec3 p0,float h) {
    return dot(n,inPos-p0)/length(n) - h;
}

//returns (distance, t value) for one line segment on the curve
vec2 road_segment(vec3 inPos, vec3 a, vec3 b, float r) {
    //normal in the plane containing line direction and up vector, perpendicular to direction
    vec3 planeDirection = b-a;
    vec3 planeNormal = vec3(
        -planeDirection.x*planeDirection.y,
        planeDirection.x * planeDirection.x + planeDirection.z * planeDirection.z, 
        -planeDirection.z*planeDirection.y
    );

    //proportion of radius the plane will be above the center - 0 for half-circle cross-section, 1 for circle cross-section
    float hProp = 0.2;

    //SDF value - intersect a capsule with a plane
    float capsuleDistance = sdCapsule(inPos,a,b,r);
    float planeDistance = sdPlane(inPos,planeNormal,a,hProp*r);
    float resDistance = max(capsuleDistance,planeDistance);

    //t value along the line a+t(b-a) such that p is closest
    float t = dot(inPos-a,b-a)/dot(b-a,b-a);
    return vec2(resDistance,t);

}

int roadResolution = 32;
float road_distance(vec3 inPos, vec4 A, vec4 B, vec4 C, vec4 D, float r) {
    //Approximation method
    vec2 dMin = vec2(1e4,0);
    int bestI = 0;
    for (int i = 0; i < roadResolution; i++) {

        float nt = i/float(roadResolution);
        float nt1 = (i+1.)/float(roadResolution);

        vec3 a = B3(nt ,A.xyz,B.xyz,C.xyz,D.xyz);
        vec3 b = B3(nt1,A.xyz,B.xyz,C.xyz,D.xyz);

        //bounding box with a little bit of wiggle room, so the very ends of the line are always generated properly
        vec4 bottom = min(a,b).xyzz - vec4(r * 1.1);
        vec4 top = max(a,b).xyzz + vec4(r * 1.1);

        if (inBox(bottom,top,inPos)) {
            vec2 segmentResult = road_segment(inPos,a,b,r);
            //store the closest distance and t value
            if (segmentResult.x < dMin.x) {
                dMin = segmentResult;
                bestI = i;
            }

        }
    }
    
    //not on one of the end caps - just return SDF
    if (dMin.y <=1. && dMin.y >= 0.) {
        return dMin.x;
    }
    //on an endcap, only return the part of the endcap that intersects with the next segment
    float nt, nt1;
    if (dMin.y > 1.) {
        //return intersection of endcap with next segment
        nt = (bestI+1.)/float(roadResolution);
        nt1 = (bestI+2.)/float(roadResolution);
    } else { //dMin.y < 0.
        // return intersection of endcap with previous segment
        nt = (bestI-1.)/float(roadResolution);
        nt1 = bestI/float(roadResolution);
    }
    //compute SDF of adjacent segment
    vec3 a = B3(nt ,A.xyz,B.xyz,C.xyz,D.xyz);
    vec3 b = B3(nt1,A.xyz,B.xyz,C.xyz,D.xyz);
    vec2 nextResult = road_segment(inPos,a,b,r);
    return max(nextResult.x,dMin.x);
}

vec3 road_normal(vec3 inPos, vec4 A, vec4 B, vec4 C, vec4 D, float r) {
    float eps = 0.001;
    vec3 dx = inPos + vec3(eps,0,0);
    vec3 dy = inPos + vec3(0,eps,0);
    vec3 dz = inPos + vec3(0,0,eps);

    float f  = road_distance(inPos,A,B,C,D,r);
    float fx = road_distance(dx,A,B,C,D,r);
    float fy = road_distance(dy,A,B,C,D,r);
    float fz = road_distance(dz,A,B,C,D,r);
    return normalize(vec3((fx-f)/eps, (fy-f)/eps, (fz-f)/eps));
}