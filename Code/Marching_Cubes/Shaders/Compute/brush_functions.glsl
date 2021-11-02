float ellipsoid_density(vec3 inPos, vec4 location, vec4 radius) {
    //from https://iquilezles.org/www/articles/ellipsoids/ellipsoids.htm
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return (k1-1.0) * min(min(radius.x,radius.y),radius.z);
}

vec3 ellipsoid_normal(vec3 inPos, vec4 location, vec4 radius) {
    float k1 = length((inPos-location.xyz)/radius.xyz);
    return normalize(1.0/k1 * (inPos.xyz - location.xyz) / (radius.xyz * radius.xyz));
}