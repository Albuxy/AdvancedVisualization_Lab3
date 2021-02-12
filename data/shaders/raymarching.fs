//EStic a la uni i no puc tenir micro
//el ray casting sense jaggering hem funciona be, diria
varying vec3 v_position;
varying vec3 v_world_position;


uniform vec4 u_color;
uniform sampler3D u_texture;
uniform float u_time;
uniform vec3 u_camera_position;
uniform vec3 u_camera_position_local;

//uniforms Jittering
uniform sampler2D u_noiseTexture;
uniform bool u_jittering_bool;
uniform float u_stepsize;

//uniforms clipping
uniform bool u_clipping_bool;
uniform vec4 u_plane;
//uniforms TF
uniform bool u_TF_bool;
uniform float u_density_value_1;
uniform float u_density_value_2;

void main()
{ 
	//Number of steps
	int numSteps = 1000;

	//Noise of the texture
	float noise;
	noise = texture2D(u_noiseTexture,gl_FragCoord.xy*0.005).x;

	//Vector StepVector
	vec3 stepVector = normalize(v_position-u_camera_position_local)*u_stepsize;
	vec3 position=v_position;    

	//Condition if Jittering is active
	if (u_jittering_bool==true){
		position=v_position+noise*stepVector; //sumar el noise a la primera posició del raig en un principi nomes feia +noise
	}

	vec4 sampleColor;
	vec4 finalColor = vec4(0.0);
	float v;
	
	for(int i=0; i<numSteps; i++){
		if(u_clipping_bool==true){
			//define de plane (clipping)
			float plane = position.x*u_plane.x + position.y*u_plane.y + position.z*u_plane.z + u_plane.w;
			if (plane<=0){
				v= texture3D(u_texture,(position + 1) / 2).r;
			
			
				//Transfer function
				if(u_TF_bool==true){
					if(v<u_density_value_1){
						sampleColor=vec4(1,0,0,v);
					}
					else if(v<u_density_value_2){
						sampleColor= vec4(0,1,0,v);
					}
					else{
						sampleColor=vec4(1,1,1,v);
					}
				}
				else{
					sampleColor=vec4(v);
				}
			
				sampleColor.rgb=sampleColor.rgb*sampleColor.a;
				finalColor += length(stepVector) * (1.0 - finalColor.a) * sampleColor;
				position=position+stepVector;

				if(finalColor.a>=1|| position.x < -1 || position.x > 1 ||position.y < -1 || position.y > 1 ||position.z < -1 || position.z > 1){
					break;
				}
			}
		}
		else{
			v= texture3D(u_texture,(position + 1) / 2).r;
			
			
			//Transfer function
			if(u_TF_bool==true){
				if(v<u_density_value_1){
					sampleColor=vec4(1,0,0,v);
				}
				else if(v<u_density_value_2){
					sampleColor= vec4(0,1,0,v);
				}
				else{
					sampleColor=vec4(1,1,1,v);
				}
			}
			else{
				sampleColor=vec4(v);
			}
			
			sampleColor.rgb=sampleColor.rgb*sampleColor.a;
			finalColor += length(stepVector) * (1.0 - finalColor.a) * sampleColor;
			position=position+stepVector;

			if(finalColor.a>=1|| position.x < -1 || position.x > 1 ||position.y < -1 || position.y > 1 ||position.z < -1 || position.z > 1){
				break;
			}
		}
	}
		

	gl_FragColor = finalColor*9;
}
