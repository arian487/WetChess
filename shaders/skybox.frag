uniform sampler2D skybox_texture;

void main(void) 
{

	gl_FragColor = texture2D(skybox_texture,vec2(gl_TexCoord[0].s,gl_TexCoord[0].t));

}