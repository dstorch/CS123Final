uniform sampler2D grassTexture;

vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);

void main()
{
    vec4 sampleGrass = texture2D(grassTexture, gl_TexCoord[0].st);
    
    float blend = pow(gl_FragCoord.z, 200);
    
    vec4 color = (1.0 - blend) * sampleGrass + blend * fogColor;
    
    if (sampleGrass.r > 0.4)
    {
	color.a = 0.0;
    }
    
    gl_FragColor = color;
}     
