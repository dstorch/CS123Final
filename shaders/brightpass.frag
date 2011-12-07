uniform sampler2D tex;
const vec3 avgVector = vec3(0.299, 0.587, 0.114);
void main(void) {
    vec4 sample = texture2D(tex, gl_TexCoord[0].st);
    float luminance = max(0.0, dot(avgVector, sample.rgb));
    
    vec4 outcolor;
    if (luminance >= 1.0) outcolor = sample;
    else outcolor = vec4(0.0, 0.0, 0.0, 0.0);
    
    gl_FragColor = outcolor;
}
