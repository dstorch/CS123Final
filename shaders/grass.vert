uniform float curTime;
uniform vec4 windOrigin;
uniform vec4 windDir;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;    
    
    vec4 vertex = gl_Vertex;
    
    if (vertex.y > 0.5) {
	
	/*// constant sway
	vec4 dir = gl_ModelViewMatrix * vertex - windOrigin; 
	//vec4 temp = vec4((float)(vertex.x - 0.0), (float)(vertex.y - 0.0), (float)(vertex.z - 0.0), (float)0.0);
	//vec4 dir2 = gl_ModelViewMatrix * temp;

	float distance = length(dir);
	dir = normalize(dir);
	float dt = pow ( max(0.0, dot(dir, windDir)), 3);

	float d = 10/distance;
	// 10/dist is good for amplitude alone
	
	float perturbation = 1.5*d*sin( distance*3/4 + curTime*2 )*dt;
	
	vertex.xz += perturbation * dir.xz; */
	
	// FREEZE FRAME BEGIN // think about bounding perturb to >0!!??? i.e first half of sin curve?/???
	// constant sway
	vec4 dir = gl_ModelViewMatrix * vertex;
	vec4 dir2 = gl_ModelViewMatrix * (vertex - windOrigin);

	float distance = length(dir2);
	dir = normalize(dir2);

	float d = 10/distance;
	// 10/dist is good for amplitude alone
	
	float perturbation = 1.5*d*sin( distance*3/4 + curTime*2 );
	
	vertex.xz += perturbation * dir.xz; 
	//FREEZE FRAME END
	
	//vec4 canonicalPt = gl_ModelViewMatrix * vertex;
	//vec3 windCan = gl_ModelViewMatrix
	////float distance = length(dir); // distance from origin
	////dir = normalize(dir);
	//vec3 windToPoint = canonicalPt - windOrigin; 
	//float distanceFromWind = length(windToPoint);
	//vec3 perturbation = distanceFromWind*windToPoint + ; // how much wind bending to apply to a vertex
	
	////float perturbation = sin(4 * distance + curTime);
	
	////vertex.xyz += perturbation * dir.xyz;
    }
    
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}