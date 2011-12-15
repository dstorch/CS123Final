uniform sampler2D groundTexture;
uniform vec3 eye;
varying vec4 vertex;

vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);

void main()
{
    vec4 sampleGround = texture2D(groundTexture, gl_TexCoord[0].st);
    
    vec4 vertexWorld = gl_ModelViewMatrix * vertex;
    
    vec2 dir = eye.xz - vertexWorld.xz;
    float distance = length(dir);
    distance = distance / 130.0;
    
    float blend = min(1.0, max(0.0, distance));
    
    vec4 color = (1.0 - blend) * sampleGround + blend * fogColor;
    
    gl_FragColor = color;
}