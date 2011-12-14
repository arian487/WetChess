uniform sampler2D white_texture;
uniform sampler2D shadow_map;

attribute float occlusion;

varying float albedo, specular;
varying vec4 shadow_coordinates;

void main( void )
{
   	vec4 normalized_shadows = shadow_coordinates / shadow_coordinates.w ;
	//this epsilon addition along with disabling front face culling will help reduce self shadowing effects
	normalized_shadows.z += 0.0005;
	
	float distance_light = texture2D(shadow_map,normalized_shadows.st).z;
	float shadow_val = 1;
	
	//basically, if we should shadow, we decide to darken the colour or not
	if (shadow_coordinates.w > 0)
		shadow_val = distance_light < normalized_shadows.z ? 0.2 : 1;

	vec4 colour = vec4(1,0,0,0);
	vec4 texture_colour = texture2D(white_texture,vec2(gl_TexCoord[0].s,gl_TexCoord[0].t));
    vec4 ambientp = 0.5 * texture_colour;
    vec4 diffusep = gl_LightSource[0].diffuse * texture_colour;
    vec4 specularp = gl_LightSource[0].specular * texture_colour;
    

  gl_FragColor = (ambientp + albedo * diffusep + specularp*specular) * shadow_val;

}