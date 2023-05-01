#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // red
    vec3(1.0, 0.0, 0.0), // green
    vec3(1.0, 0.0, 0.0), // blue
    vec3(1.0, 0.0, 0.0), // red
    vec3(1.0, 0.0, 0.0), // blue
    vec3(1.0, 0.0, 0.0)  // yellow
);

vec2 positions[6] = vec2[](
    vec2(-0.5, -0.5), // bottom left
    vec2(0.5, -0.5),  // bottom right
    vec2(0.5, 0.5),   // top right
    vec2(-0.5, -0.5), // bottom left
    vec2(0.5, 0.5),   // top right
    vec2(-0.5, 0.5)   // top left
);

layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}