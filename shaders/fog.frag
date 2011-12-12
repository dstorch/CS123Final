uniform sampler2D scene;
uniform sampler2D depth;

void main()
{
    vec4 sampleScene = texture2D(scene, gl_TexCoord[0].st);
    vec4 sampleDepth = texture2D(depth, gl_TexCoord[0].st);
    
    gl_FragColor = sampleDepth;
}     
