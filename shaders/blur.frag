const int MAX_KERNEL_SIZE = 128;
uniform sampler2D tex;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE]; 
uniform float kernel[MAX_KERNEL_SIZE];
void main(void) { 
    // Fill this in!

	vec4 outcolor = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < MAX_KERNEL_SIZE; i++)
	{
		vec4 sample = texture2D(tex,  gl_TexCoord[0].st + offsets[i]);
		outcolor += kernel[i] * sample;
	}

	gl_FragColor = outcolor;
}
