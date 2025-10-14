#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;

uniform sampler2D inTexture;

void main() {
  FragColor = texture(inTexture, TextureCoord);
}
