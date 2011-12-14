uniform float curTime;
uniform vec4 windOrigin;
uniform vec4 windDir;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;    
    
    vec4 vertex = gl_Vertex;
    
    if (vertex.y > 0.5) {
	
	// constant sway
	vec4 dir = gl_ModelViewMatrix * vertex - windOrigin; 
	//vec4 temp = vec4((float)(vertex.x - 0.0), (float)(vertex.y - 0.0), (float)(vertex.z - 0.0), (float)0.0);
	//vec4 dir2 = gl_ModelViewMatrix * temp;

	float distance = length(dir);
	dir = normalize(dir);
	float dt = pow ( max(0.0, dot(dir, windDir)), 3);

	float d = 10/distance;
	// 10/dist is good for amplitude alone
	
	float perturbation = 1.1*d*sin( distance*3/4 + curTime*1 )*dt;
	
	vertex.xz += perturbation * dir.xz;
    }
    
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}