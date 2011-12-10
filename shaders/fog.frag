uniform sampler2D scene;

void main()
{
    vec4 sampleScene = texture2D(scene, gl_TexCoord[0].st);
    
    gl_FragColor = sampleScene;
}     
