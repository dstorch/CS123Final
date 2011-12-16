uniform float curTime;
uniform vec4 windOrigin;
uniform vec4 windDir;
uniform float windTime;
uniform float ambientWaveAmplitude;
uniform float perturbationWaveAmplitude;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;    
    
    vec4 vertex = gl_Vertex;
    
    if (vertex.y > 0.5) {
	
	// constant sway
	vec4 cDir = gl_ModelViewMatrix * vertex;
	float cDistance = length(cDir);
	cDir = normalize(cDir);
	float cPerturbation = ambientWaveAmplitude*sin(1.7*cDistance + 1.5*curTime);
	vertex.xz += cPerturbation * cDir.xz;
	
	// click wave
	vec4 dir = gl_ModelViewMatrix * vertex - windOrigin; 
	//vec4 temp = vec4((float)(vertex.x - 0.0), (float)(vertex.y - 0.0), (float)(vertex.z - 0.0), (float)0.0);
	//vec4 dir2 = gl_ModelViewMatrix * temp;

	float distance = length(dir);
	dir = normalize(dir);
	float dt = pow (max(0.0, dot(normalize(dir.xz), normalize(windDir.xz))), 3);

	float d = 10.0/distance;
	float timeAtt = pow((windTime / 100.0), 4);
	// 10/dist is good for amplitude alone
	
	float perturbation = perturbationWaveAmplitude*d*timeAtt*sin( distance*3/4 + curTime*1 )*dt;
	
	vertex.xz += perturbation * dir.xz;
    }
    
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
