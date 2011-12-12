uniform sampler2D grassTexture;

vec4 textureBackground = vec4(0.1294, 0.3059, 0.0667, 0.0);

void main()
{
    vec4 sampleGrass = texture2D(grassTexture, gl_TexCoord[0].st);
    
    if( sampleGrass.a < .5)
	discard;
    if (sampleGrass.r > 0.3 || sampleGrass.g < .2)
    {
	gl_FragColor = textureBackground;
    }
    else
    {
	gl_FragColor = sampleGrass;
    }
}     
