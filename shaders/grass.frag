uniform sampler2D grassTexture;
uniform vec3 eye;
varying vec4 vertex;

vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);
vec4 sunColor = vec4(0.7, 0.7, 0.1, 1.0);

vec3 sun = vec3(95.0, 50.0, 95.0);

void main()
{
    vec4 sampleGrass = texture2D(grassTexture, gl_TexCoord[0].st);
    
    vec4 vertexWorld = gl_ModelViewMatrix * vertex;
    
    vec2 dir = eye.xz - vertexWorld.xz;
    float distance = length(dir);
    distance = distance / 130.0;
    
    vec3 sunDir = normalize(sun - vertexWorld.xyz);
    vec3 view = normalize(eye.xyz - vertexWorld.xyz);
    float specular = min(1.0, max(0.0, dot(view, sunDir)));
    specular = 0.3 * pow(specular, 4);
    specular = min(1.0, max(0.0, specular)); 
    
    float blend = min(1.0, max(0.0, distance));
    
    vec4 color = (1.0 - blend) * sampleGrass + blend * fogColor;
    color +=  specular * sunColor;
    
    if (sampleGrass.r > 0.4)
    {
	color.a = 0.0;
    }
    
    gl_FragColor = color;
}     
