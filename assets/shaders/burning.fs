#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform float burningIntensity; // Value between 0.0 and 1.0
uniform vec3 palette;           // Pre-computed burning color

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);
    
    // Use the provided palette as the burn color
    vec3 burnColor = palette;
    //vec3 burnColor = vec3(palette.x, 0.0, 0.0);
    
    // Mix original texture color and burnColor based on burningIntensity
    //vec3 mixedColor = mix(texColor.rgb, burnColor, burningIntensity);
    vec3 mixedColor = burnColor;

    finalColor = vec4(mixedColor, texColor.a);
    //finalColor = vec4(1.0, 0.0, 0.0, texColor.a);
}
