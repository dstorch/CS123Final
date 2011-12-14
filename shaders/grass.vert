uniform float curTime;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;    
    
    vec4 vertex = gl_Vertex;
    
    if (vertex.y > 0.5) {
	
	// constant sway
	vec4 dir = gl_ModelViewMatrix * vertex;
	float distance = length(dir);
	dir = normalize(dir);
	float perturbation = sin(4 * distance + curTime);
	
	vertex.xz += perturbation * dir.xz;
    }
    
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}