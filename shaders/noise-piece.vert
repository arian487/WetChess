uniform float time;
uniform float time2;
uniform int random_num;
uniform int noise;
uniform float transparency;
uniform float time_divider;
uniform float ripple_size;
uniform float ripple_speed;
uniform int should_occlude;
uniform int should_light;

uniform sampler2D colour_texture;
uniform sampler1D simplex_texture;
uniform sampler2D gradient_texture;

attribute float occlusion;

varying vec4 shadow_coordinates;
varying vec3 text_coord;
varying float albedo, specular;
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
	  //NOTE:  The texture lookup table was taken directly from Gustovssons paper
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
		//made it look good
	  return 40 * (n0 + n1 + n2 + n3 + n4);
}


void main( void )
{

	shadow_coordinates = gl_TextureMatrix[7] * gl_Vertex;
	
	

  vec3 pos_EC = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 normal_EC = normalize(gl_NormalMatrix * gl_Normal);
  vec3 light_EC = gl_LightSource[0].position.xyz - pos_EC;
  vec3 half_EC = normalize(normalize(light_EC) - normalize(pos_EC));
  
    light_EC = normalize(light_EC);
      text_coord = gl_Vertex.xyz;
    
    	float n = simplex_noise(vec4(0.5 * text_coord.xyz, 0.5 * (time/ripple_speed)));
    	n=n/ripple_size;
    	n=n*2;
    	n=n*(1-should_occlude);
//    	normal_EC.x += n;
//    	normal_EC.y += n;
//    	normalize(normal_EC);

  attenuation_factor = occlusion;

  albedo = max(dot(normal_EC, light_EC), 0.0);
  specular = pow(max(dot(normal_EC, half_EC),0.0),  gl_FrontMaterial.shininess);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[0] = gl_MultiTexCoord0;

  
	gl_Position.x += n;
	gl_Position.y + n;
	//adding noise to z screws everything up...
//	gl_Position.z += n*time/100;

}


