//Ref: https://github.com/finallyjustice/sphfluid/tree/master/SPH_CPU_3D_v1/Shader
void main()
{
    const vec3 L = vec3(0.5, 0.5, 1.0);
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) discard;
    N.z = sqrt(1.0-mag);
    float diffuse = max(0.0, dot(L, N));
    gl_FragColor = gl_Color * diffuse;
}