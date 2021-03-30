#version 430
uniform sampler2D phTex;
in vec2 texCoord;
out vec4 color;
void main() {
    float c = texture(phTex, texCoord).x;
    color = vec4(c, 1.0, 1.0, 1.0);
    //color = texture(phTex, texCoord);
}