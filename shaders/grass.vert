uniform vec4 windCenter;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;    
    
    vec4 vertex = gl_Vertex;
    
    /*if (vertex.y > 0.5) {
	vec4 dir = normalize(gl_ModelViewMatrix * vertex - windCenter);
	vertex.xz += 5 * dir.xz;
    }*/
    
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}