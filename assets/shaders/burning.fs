#version 330

in vec2 fragTexCoord;
uniform sampler2D texture0;   // The blade texture
uniform float scale;          // Blade's scale, e.g. 0.0..1.0
uniform vec3 palette;         // The blade's average color, normalized to [0..1]

out vec4 finalColor;

void main() {
    // Sample the source color from the texture
    vec4 srcColor = texture(texture0, fragTexCoord);

    if (srcColor.a < 0.05) {
        discard;
    }
    
    float safeScale = max(scale, 0.001);  // Avoid division by zero

    float r = clamp(palette.r * 1.8 * (6.0 / safeScale), 0.0, 1.0);
    float g = clamp(palette.g * (0.8 / safeScale),      0.0, 1.0);
    float b = clamp(palette.b * (1.0 / safeScale),      0.0, 1.0);

    // Keep the original alpha
    finalColor = vec4(r, g, b, srcColor.a);
}