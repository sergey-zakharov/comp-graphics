#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform bool isWireframe;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec4 color;

out vec4 interpColor;

void main()
{
	if( isWireframe ) {
		interpColor = vec4(.5, .5, 0.5, 1.);
	} 
	//else{
	//	interpColor = vec4(1., 0.5, 0., 1.);
	//}
	else{
		interpColor = color; //vec4(1., 1., 1., 1.);
	}
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.);
}
