#version 330 core

out vec4 fragmentColor;

in vec2 UV;
in float vs_life;

uniform sampler2D texture0;

void main() {
    vec4 texColor = texture(texture0, UV);
    if (length(texColor.rgb) < 0.01) discard;
    fragmentColor = vec4(texColor.rgb * vs_life * vs_life,0.5f);
}
