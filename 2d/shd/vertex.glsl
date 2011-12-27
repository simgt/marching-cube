uniform mat4 p_matrix;
attribute vec4 position;

void main() {
   gl_Position = p_matrix * position;
}