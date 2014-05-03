#version 130


uniform vec3 faceColor;
in vec3 vPosition;
uniform vec3 offset_Z;
uniform vec3 scale;
uniform mat4 Scale;
uniform mat4 view;
uniform mat4 proj;
out vec3 outColor;
 
 
 
 attribute vec2 texcoord;
 out vec2 f_texcoord;
in vec3 normalFaces; 
 
 in vec2 vertexUV;
in vec3 normal;
out vec2 UV;
vec4 AmbientProduct, DiffuseProduct, SpecularProduct; 
vec4 LightPosition;
float Shininess = 1000.0;
uniform vec3 angles;
out vec2 isPlaneFrag;
 
 
 
uniform vec2 isPlane;
 
void main() {
	isPlaneFrag = isPlane;
	
	if(isPlane == vec2(200,200)){
    vec3 Angle = radians( vec3(180.0f,0,0) );
	
     Angle = radians( vec3(angles.z,0,0) );
	 mat4 rot_z = mat4( 	
			       	   cos( Angle.x), -sin(Angle.x )   ,0.0 , 0.0
			    		, sin( Angle.x ),cos(Angle.x ) ,0.0   , 0.0
				     	 ,0.0,   0.0,                   1.0, 	0.0
				     	 ,0.0,   0.0,                   0.0, 	1.0 
				     	 );
	
    
    
    	
    		Shininess = 5;
    	//////////////////////////////////////////////////////////////////
    	
    	
    	mat4 scaleMatrix = mat4(  scale.x, 0.0,     0.0,     0.0,
			    			 0.0,     scale.y, 0.0,     0.0,
			            	 0.0,     0.0, 	   scale.z, 0.0,
				     	 	 0.0,     0.0, 	   0.0,     1.0 );
		
		
		mat4 transs = mat4(  1.0, 0.0, 0.0, offset_Z.x,
			    			 0.0, 1.0, 0.0, offset_Z.y,
			            	 0.0, 0.0, 1.0, offset_Z.z,
				     	 	 0.0, 0.0, 0.0, 1.0 );
		
		vec4 Ka = vec4(0.4 ,0.4, 0.4,1.0);	
		AmbientProduct = vec4(0.1, 0.1, 0.1, 1.0);
		DiffuseProduct = vec4(1.0, 0.8, 0.6, 1.0);
		SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0);
	
		mat4 ModelView = view  * transs * scaleMatrix;
		
	
		vec3 pos = (ModelView * vec4(vPosition,1.0)).xyz;
		// Compute viewer vector in eye coordinate 
		vec3 E = normalize( -pos );
	
	
	
		// Compute light vector in eye coordinate 
		LightPosition = vec4(-30.0,5.0,10.0,1.0);
		vec3 L; 
		if (LightPosition.w == 0.0) 
			 L = normalize( (view * LightPosition).xyz ); 
		else
			 L = normalize( (view * LightPosition).xyz - pos );
		// Compute halfway vector in eye coordinate 
		vec3 H = normalize( L + E ); 
		// halfway vector // Transform normal vector into eye coordinates 
		vec3 N = normalize( ModelView * vec4(normalFaces, 0.0) ).xyz;
	
	
	
	
		// Compute terms in the illumination equation 
		vec4 ambient = Ka * AmbientProduct ; 
		float Kd = max( dot(L, N), 0.0 ); 
		vec4 diffuse = Kd * DiffuseProduct; 
		float Ks = pow( max(dot(N, H), 0.0), Shininess ); 
		vec4 specular = Ks * SpecularProduct; 
		// discard the specular highlight if the light's behind the vertex 
		if ( dot(L, N) < 0.0 ) 
			specular = vec4(0.0, 0.0, 0.0, 1.0);
	
    	
    	
    	
    	
    	
    	/////////////////////////////////////////////////////
    
    
    
        gl_Position = proj * view*vec4(((vPosition.x*scale.x)+offset_Z.x),((vPosition.y*scale.y)+offset_Z.y),((vPosition.z*scale.z)+offset_Z.z),1.0);
        //outColor = vec3(faceColor.x,faceColor.y,faceColor.z);
		f_texcoord = texcoord;
	
		outColor = (ambient + diffuse + specular).xyz;
	
	
	}else if(isPlane == vec2(100,100)){
		
		
		vec3 Angle1 = radians( vec3(180.0f,0,0) );
	
     Angle1 = radians( vec3(angles.z,0,0) );
	 mat4 rot_z = mat4( 	
			       	  	cos( Angle1.x), 0.0, sin(Angle1.x ), 	0.0,
			    		 0.0,        1.0, 		0.0,	0.0,
				     	-sin( Angle1.x ), 0.0, cos(Angle1.x ),	0.0,
			             0.0,        0.0, 	    0.0, 	1.0 );
	
    
    
    
		
		
		mat4 scaleMatrix = mat4(  scale.x, 0.0,     0.0,     0.0,
			    			 0.0,     scale.z, 0.0,     0.0,
			            	 0.0,     0.0, 	   scale.y, 0.0,
				     	 	 0.0,     0.0, 	   0.0,     1.0 );
		
		
		mat4 transs = mat4(  1.0, 0.0, 0.0, offset_Z.x,
			    			 0.0, 1.0, 0.0, offset_Z.z,
			            	 0.0, 0.0, 1.0, offset_Z.y,
				     	 	 0.0, 0.0, 0.0, 1.0 );
		
		vec4 Ka = vec4(0.117647 ,0.117647, 0.117647,1.0);	
		AmbientProduct = vec4(0.4, 0.4, 0.4, 1.0);
		DiffuseProduct = vec4(1.0, 0.8, 0.6, 1.0);
		SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0);
	
		mat4 ModelView = view  * transs * scaleMatrix;
		
	
		vec3 pos = (ModelView * vec4(vPosition,1.0)).xyz;
		// Compute viewer vector in eye coordinate 
		vec3 E = normalize( -pos );
	
	
	
		// Compute light vector in eye coordinate 
		LightPosition = vec4(-30.0,5.0,10.0,1.0);
		vec3 L; 
		if (LightPosition.w == 0.0) 
			 L = normalize( (view * LightPosition).xyz ); 
		else
			 L = normalize( (view * LightPosition).xyz - pos );
		// Compute halfway vector in eye coordinate 
		vec3 H = normalize( L + E ); 
		// halfway vector // Transform normal vector into eye coordinates 
		vec3 N = normalize( ModelView * vec4(normal, 0.0) ).xyz;
	
	
	
	
		// Compute terms in the illumination equation 
		vec4 ambient = Ka * AmbientProduct ; 
		float Kd = max( dot(L, N), 0.0 ); 
		vec4 diffuse = Kd * DiffuseProduct; 
		float Ks = pow( max(dot(N, H), 0.0), Shininess ); 
		vec4 specular = Ks * SpecularProduct; 
		// discard the specular highlight if the light's behind the vertex 
		if ( dot(L, N) < 0.0 ) 
			specular = vec4(0.0, 0.0, 0.0, 1.0);
	
	
	vec3 Angle = radians( vec3(180.0f,0,0) );
	
	 mat4 rot_y = mat4( 	cos( Angle.x), 0.0, sin(Angle.x ), 	0.0,
			    		 0.0,        1.0, 		0.0,	0.0,
				     	-sin( Angle.x ), 0.0, cos(Angle.x ),	0.0,
			             0.0,        0.0, 	    0.0, 	1.0 );
	 Angle = radians( vec3(90.0f,0,0) );
	 mat4 rot_x = mat4( 	
			             1.0,  	0.0,   0.0,	    0.0,
				     	 0.0, 	   cos( Angle.x), -sin(Angle.x ),0.0
			    		,0.0, 	   sin( Angle.x ),cos(Angle.x ),0.0
				     	 ,0.0,   0.0,   0.0, 	1.0 
				     	 );
	
	
	mat4 transs1 = mat4(  1.0, 0.0, 0.0, 0,
			    			 0.0, 1.0, 0.0, 0.9,
			            	 0.0, 0.0, 1.0, 0,
				     	 	 0.0, 0.0, 0.0, 1.0 );
	
	
	
	mat4 scale_plane=mat4(0.5,0   ,0   ,0,
						  0  ,0.5 ,0   ,0,
						  0  ,0   ,0.5 ,0,
						  0  ,0   ,0   ,1);

		gl_Position = rot_z * vec4(vPosition,1.0);
    	

	//	gl_Position =  proj * view*vec4(((vPosition.x*scale.x)),((vPosition.y*scale.y)),((vPosition.z*scale.z)+offset_Z.z),1.0);	
		gl_Position =  proj * view *rot_y*rot_x*vec4(gl_Position.x/20 -offset_Z.x,(gl_Position.y/20 )+offset_Z.z,(gl_Position.z/20)+offset_Z.y+0.1,1.0);
		
		
		UV = vertexUV;
		outColor = (ambient + diffuse + specular).xyz;
		
		
		
		//gl_Position = proj * view*vec4(((vPosition.x*scale.x)+offset_Z.x),((vPosition.y*scale.y)+offset_Z.y),((vPosition.z*scale.z)+offset_Z.z),1.0);
        //outColor = vec4(faceColor.x,faceColor.y,faceColor.z,1.0);
	}
}