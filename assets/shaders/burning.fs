// burning.fs - Fragment shader
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform float scale;           // The blade scale (e.g. burning grass when < 1.0)
uniform vec3 palette;          // RGB values in [0,1] (e.g. palette/255.0)
uniform float burningIntensity;// A value between 0.0 and 1.0

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);

    // Compute the burning color based on your algorithm:
    // Replicating: min(255, palette[0]*(1.8)*(1/scale*6)) etc.
    // Here we assume palette is normalized so that multiplying by factors gives values in [0,1]
    float factor = 6.0 / scale;  // similar to (1/scale*6)
    float r = min(1.0, palette.r * 1.8 * factor);
    float g = min(1.0, palette.g * factor);
    float b = min(1.0, palette.b * factor);
    vec3 burnColor = vec3(r, g, b);

    // Mix original texture color and burnColor based on burning intensity
    vec3 mixedColor = mix(texColor.rgb, burnColor, burningIntensity);

    finalColor = vec4(mixedColor, texColor.a);
}
