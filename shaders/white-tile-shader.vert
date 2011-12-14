uniform float time;

attribute float occlusion;

varying float albedo, specular;
varying vec4 shadow_coordinates;

void main( void )
{

  shadow_coordinates = gl_TextureMatrix[7] * gl_Vertex;

  vec3 pos_EC = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 normal_EC = normalize(gl_NormalMatrix * gl_Normal);
  vec3 light_EC = gl_LightSource[0].position.xyz - pos_EC;
  vec3 half_EC = normalize(normalize(light_EC) - normalize(pos_EC));
  
    light_EC = normalize(light_EC);

  albedo = max(dot(normal_EC, light_EC), 0.0);
  specular = pow(max(dot(normal_EC, half_EC),0.0),  gl_FrontMaterial.shininess);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[0] = gl_MultiTexCoord0;

}
