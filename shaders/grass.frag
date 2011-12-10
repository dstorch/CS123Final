uniform sampler2D grassTexture;

void main()
{
    vec4 sampleGrass = texture2D(grassTexture, gl_TexCoord[0].st);
    
    gl_FragColor = sampleGrass;
    gl_FragColor = sampleGrass;
}     
