#version 130
in vec3 outColor;


in vec2 UV;
uniform sampler2D myTextureSampler;
uniform vec2 isPlane;
in vec2 isPlaneFrag;
out vec3 color;

in vec2 f_texcoord;
uniform sampler2D mytexture;

void
main()
{

    if(isPlane.x == 200.0f){
		//color = outColor;
		 vec2 flipped_texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
  		 color = (texture2D(mytexture, flipped_texcoord)).xyz + outColor;
	}else if (isPlane == vec2(100,100)){
		color = texture2D( myTextureSampler, UV ).rgb + outColor;
	}
}
