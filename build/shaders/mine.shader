[[FX]]

context AMBIENT {
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_AMBIENT;
}

context LIGHTING
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_LIGHTING;
}

[[VS_GENERAL]]

uniform mat4 viewProjMat;
uniform mat4 worldMat;

attribute vec4 vertPos;
attribute vec3 normal;

varying vec3 fgPosition;
varying vec3 fgNormal;

void main () {
    gl_Position = viewProjMat * worldMat * vertPos;

    fgPosition = vec3(worldMat * vertPos);
	fgNormal = normalize(normal);
}


[[FS_AMBIENT]]

varying vec3 fgPosition;
varying vec3 fgNormal;

void main () {
	vec3 albedo = vec3(1, 1, 1); // TODO: temp

	gl_FragColor.rgb = albedo * 0;
}

[[FS_LIGHTING]]

uniform vec4 specParams;

uniform vec3 viewerPos;
uniform vec4 lightPos;
uniform vec4 lightDir;
uniform vec3 lightColor;

varying vec3 fgPosition;
varying vec3 fgNormal;

void main () {
	vec3 albedo = vec3(1, 1, 1); // TODO: temp

	float specMask = specParams.x;
	float specExp = specParams.y;

    vec3 light = lightPos.xyz - fgPosition;
	float lightLen = length(light);
	light /= lightLen; // normalize
	
	// Distance attenuation
	float lightDepth = lightLen / lightPos.w;
	float atten = max(1.0 - lightDepth * lightDepth, 0.0 );
	
	// Spotlight falloff
	float angle = dot(lightDir.xyz, -light );
	atten *= clamp((angle - lightDir.w) / 0.2, 0.0, 1.0 );
		
	// Lambert diffuse
	float NdotL = max(dot(fgNormal, light), 0.0);
	atten *= NdotL;
		
	// Blinn-Phong specular with energy conservation
	vec3 view = normalize(viewerPos - fgPosition);
	vec3 halfVec = normalize(light + view);
	float spec = pow( max(dot(halfVec, fgNormal ), 0.0 ), specExp);
	spec *= (specExp * 0.04 + 0.32) * specMask;  // Normalization factor (n+8)/8pi
	
	// Final color
	gl_FragColor.rgb = albedo * lightColor * atten * (1.0 + spec);
}
