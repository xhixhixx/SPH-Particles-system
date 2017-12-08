//Ref: https://github.com/finallyjustice/sphfluid/tree/master/SPH_CPU_3D_v1/Shader
void main()
{
	vec3 E = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    float dist = length(E);
	gl_PointSize = 10.0/dist;
    
	gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
}