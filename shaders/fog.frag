uniform sampler2D sceneTex;
uniform sampler2D depthTex;
    
vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);

float fogBlendFactor(vec4 depth)
{
    float z = depth.x;
    return pow(z, 150);
}

void main()
{
    vec4 sampleScene = texture2D(sceneTex, gl_TexCoord[0].st);
    vec4 sampleDepth = texture2D(depthTex, gl_TexCoord[0].st);
    
    float blend = 0.3 * fogBlendFactor(sampleDepth);
    
    vec4 color = (1.0 - blend) * sampleScene + blend * fogColor;
    
    gl_FragColor = sampleScene;
}     
