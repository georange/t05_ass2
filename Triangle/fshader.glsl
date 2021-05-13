#version 330 core
in vec3 fcolor; ///< passed by vshader
out vec4 color; ///< output color

void main() {
    color = vec4(fcolor,1.0); /// fcolor is interpolated!
}
