#version 330 core
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;
uniform float time;

mat4 S(float s){
    return mat4(mat3(s));
}

mat4 R(float degrees){
    mat3 R = mat3(1);
    float alpha = radians(degrees);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);
    return mat4(R);
}

mat4 T(float rate){
    mat4 T = mat4(1);
    T[0][3] =  rate;
	T[1][3] =  rate;
	T[2][3] =  rate;
    return T;
}

void main() {
    // gl_Position = S(.5) * R(10) * vec4(vpoint, 1.0);

	float move = 8*(sin(time*20));

	//gl_Position = vec4(vpoint, 2.0);// * R(move);

	gl_Position = S(abs(.2 * sin(time * 4.0f))+0.3) * vec4(vpoint, 2.0) * R(move) * T(0.6) * R(-2.5*10*time) * T(-0.6);
	
	//gl_Position = S(abs(.2 * sin(time * 4.0f))+0.3) * vec4(vpoint, 2.0) * T(0.6) * R(-2.5*10*time) * T(-0.6);
	    
    uv = vtexcoord;
}
