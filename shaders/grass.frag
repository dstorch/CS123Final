uniform sampler2D grassTexture;
uniform vec3 eye;
varying vec4 vertex;

vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);

void main()
{
    vec4 sampleGrass = texture2D(grassTexture, gl_TexCoord[0].st);
    
    vertex = gl_ModelViewMatrix * vertex;
    
    vec2 dir = eye.xz - vertex.xz;
    float distance = length(dir);
    distance = distance / 130.0;
    
    float blend = min(1.0, max(0.0, distance));
    
    vec4 color = (1.0 - blend) * sampleGrass + blend * fogColor;
    
    if (sampleGrass.r > 0.4)
    {
	color.a = 0.0;
    }
    
    gl_FragColor = color;
}     
