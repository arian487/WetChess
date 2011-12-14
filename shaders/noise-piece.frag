uniform sampler2D colour_texture;
uniform sampler1D simplex_texture;
uniform sampler2D gradient_texture;
uniform sampler2D water;
uniform float time;
uniform int noise;
uniform float transparency;
uniform float time_divider;
uniform int time2;
uniform sampler2DShadow shadow_map;
uniform int should_occlude;
uniform int should_light;

varying vec3 text_coord;
varying float albedo, specular;
varying vec4 shadow_coordinates;
varying float attenuation_factor;

//using 4d simplex noise...because why not?
float simplex_noise(vec4 coord) {

	// These are skewing and unskewing factors...to be honest I have no idea how
	// Perlin devised these numbers...defining them in the preprocessor
	//so we don't recalculate once per vertex
	// (sqrt(5.0)-1.0)/4.0
	#define skew 0.3090169943
	// (5.0-sqrt(5.0))/20.0
	#define unskew 0.1381966011
	
	  
	  //here I am finding out which simplex I'm in (24 possible ones in the 4D case...4!).  
	  float skewer = (coord.x + coord.y + coord.z + coord.w) * skew;
	  vec4 coordi = floor(coord + skewer);
	  float unskewer = (coordi.x + coordi.y + coordi.z + coordi.w) * unskew;
	  vec4 coord0 = coordi - unskewer; 
	  //now I offset by the one texels worth
	  coordi = coordi * 0.0078125 + 0.00390625;
	  vec4 coordf0 = coord - coord0;  //This gives me the distances from the cells origin
	
		//need at least one contribution...so I offset by one texels worth.
		//NOTE a texel is 1/128 since my texture is 128x128
		//compare each possible pair of the 4 coordinates to arrive at a texture look up
		//this tels us which simplice we're in
		
		//are we in upper or lower half?
	  float contributionxy = (coordf0.x > coordf0.y) ? 0.5078125 : 0.0078125; 
	  //now we check which half of the xz plane we're in
	  float contributionxz = (coordf0.x > coordf0.z) ? 0.25 : 0;
	  //now check which half of yz
	  float contributionyz = (coordf0.y > coordf0.z) ? 0.125 : 0;
	  //now check which half of xw
	  float contributionxw = (coordf0.x > coordf0.w) ? 0.0625 : 0;
	  //now check which half of yw
	  float contributionyw = (coordf0.y > coordf0.w) ? 0.03125 : 0;
	  //now check which half of zw
	  float contributionzw = (coordf0.z > coordf0.w) ? 0.015625 : 0;
	  float simplex_index = contributionxy + contributionxz + contributionyz + contributionxw + contributionyw + contributionzw;
	  //Now that we have all of our contributions, we know where in the simplex texture to look in
	  //to get our offsets
	  //NOTE:  The texture was taken directly from Gustovssons paper
	  vec4 val = texture1D(simplex_texture, simplex_index).rgba;
	  vec4 o1 = step(0.625, val);
	  vec4 o2 = step(0.375, val);
	  vec4 o3 = step(0.125, val);
	
	  // Phew now we can calculate the noise contributions from the corners
	  float colourxy = texture2D(colour_texture, coordi.xy).a;
	  float colourzw = texture2D(colour_texture, coordi.zw).a;
	  vec4  gradient = texture2D(gradient_texture, vec2(colourxy, colourzw)).rgba;
	  float t = 0.6 - dot(coordf0, coordf0);
	  float n0;
	  //don't want negative contributions
	  if (t < 0.0) {
	  	n0 = 0.0;
	  }
	  else {
	    n0 = t * t * t * t * dot(gradient, coordf0);
	  }
	
	  // Noise contribution from second corner
	  vec4 coordf1 = -o1 + unskew + coordf0;
	  o1 = o1 * 0.0078125;
	  colourxy = texture2D(colour_texture, coordi.xy + o1.xy).a;
	  colourzw = texture2D(colour_texture, coordi.zw + o1.zw).a;
	  gradient = texture2D(gradient_texture, vec2(colourxy, colourzw)).rgba;
	  t = 0.6 - dot(coordf1, coordf1);
	  float n1;
	  if (t < 0.0) {
	  	n1 = 0.0;
	  }
	  else {
	    n1 = t * t * t * t * dot(gradient, coordf1);
	  }
	  
	  float new_unskew = unskew*2;
	  coordf1 =  -o2 + new_unskew + coordf0;
	  o2 = o2 * 0.0078125;
	  colourxy = texture2D(colour_texture, coordi.xy + o2.xy).a;
	  colourzw = texture2D(colour_texture, coordi.zw + o2.zw).a;
	  gradient = texture2D(gradient_texture, vec2(colourxy, colourzw)).rgba;
	  t = 0.6 - dot(coordf1, coordf1);
	  float n2;
	  if (t < 0.0) {
	  	n2 = 0.0;
	  }
	  else {
	    n2 = t * t * t * t * dot(gradient, coordf1);
	  }
	  
	  new_unskew = unskew * 3;
	  coordf1 = -o3 + new_unskew + coordf0;
	  o3 = o3 * 0.0078125;
	  colourxy = texture2D(colour_texture, coordi.xy + o3.xy).a;
	  colourzw = texture2D(colour_texture, coordi.zw + o3.zw).a;
	  gradient = texture2D(gradient_texture, vec2(colourxy, colourzw)).rgba;
	  t = 0.6 - dot(coordf1, coordf1);
	  float n3;
	  if (t < 0.0) {
	  	n3 = 0.0;
	  }
	  else {
	    n3 = t * t * t * t * dot(gradient, coordf1);
	  }
	  
	  coordf1 = coordf0 - vec4(1.0-4.0*unskew);
	  colourxy = texture2D(colour_texture, coordi.xy + vec2(0.0078125, 0.0078125)).a;
	  colourzw = texture2D(colour_texture, coordi.zw + vec2(0.0078125, 0.0078125)).a;
	  gradient = texture2D(gradient_texture, vec2(colourxy, colourzw)).rgba;
	  t = 0.6 - dot(coordf1, coordf1);
	  float n4;
  	  if (t < 0.0) {
  	  	n4 = 0.0;
      }
  	  else {
    	n4 = t * t * t * t * dot(gradient, coordf1);
      }
	
		//original contributions too small, so i multiply by some arbitrary number that 
		//made it look good...dont ask me why 40 works :P
	  return 40 * (n0 + n1 + n2 + n3 + n4);
}


void main( void )
{

	vec4 normalized_shadows = shadow_coordinates / shadow_coordinates.w ;
	//this epsilon addition along with disabling front face culling will help reduce self shadowing effects
	normalized_shadows.z += 0.0005;
	
	float distance_light = shadow2DProj(shadow_map,shadow_coordinates).r;
	float shadow_val = 1;
	
	//basically, if we should shadow, we decide to darken the colour or not
	if (shadow_coordinates.w > 0)
		shadow_val = distance_light < normalized_shadows.z ? 0.2 : 1;
	
  float n = simplex_noise(vec4(text_coord.xyz, (time/300)));
  n=n/15;

	vec4 colour = vec4(1,0,0,0);
	vec4 texture_colour = texture2D(water,vec2(gl_TexCoord[0].s-time/time_divider-(n*noise),gl_TexCoord[0].t-(n*noise)));
    vec4 final_colour = (texture_colour);// + (perlin_colour * 0.3);
    vec4 ambientp1 =  ((1-should_light) * 0.9 + 0.1) * ((noise)*attenuation_factor + (1-noise)*(1-attenuation_factor)) * vec4(1,1,1,1);
    vec4 ambientp2 = 0.2 * final_colour;
    vec4 ambientp = should_occlude*ambientp1 + (1-should_occlude)*ambientp2;
    
    vec4 diffusep1 = gl_LightSource[0].diffuse * vec4(1,1,1,1);
    vec4 diffusep2 = gl_LightSource[0].diffuse * final_colour;
    vec4 diffusep = should_occlude*diffusep1 + (1-should_occlude)*diffusep2;
    
    vec4 specularp1 = gl_LightSource[0].specular * vec4(1,1,1,1);
    vec4 specularp2 = gl_LightSource[0].specular * final_colour;
    vec4 specularp = should_occlude*specularp1 + (1-should_occlude)*specularp2;
    

 	vec4 colour1 = vec4(vec3(ambientp + albedo*diffusep*should_light), 1);
 	vec4 colour2 = vec4(vec3(ambientp + albedo * diffusep + specular*specularp), transparency);
 	gl_FragColor = colour1 * should_occlude + (1-should_occlude)*colour2;
 //gl_FragColor = vec4(vec3(ambientp + albedo*diffusep + specular*specularp), 1);
 // gl_FragColor = vec4(vec3(ambientp + albedo * diffusep + specular*specularp), transparency);
//  gl_FragColor = texture2D(water,vec2(gl_TexCoord[0].s-time/256,gl_TexCoord[0].t)) * vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);

}

