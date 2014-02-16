varying vec3 vColor;
varying vec3 vNormal;
varying vec4 vPosition;

uniform float uMaterial;

uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform int uHUDItem;

#define I_aL vec3(0.3,0.3,0.3)

void main() {
    vec3 fragColor;
    vec3 light;
    vec3 reflection;
    vec3 normNormal = normalize(vNormal);
    
    light = normalize(uLightPos.xyz - vPosition.xyz);
    reflection = -light + 2.0 * (max(dot(normNormal, light), 0.0)) * normNormal;
    
    fragColor = vColor * I_aL +
                vColor * max(dot(normNormal, light), 0.0) * uLightColor +
                vColor * pow(max(dot(normalize(vec3(0,0, 2.5) - vPosition.xyz), reflection), 0.0), uMaterial) * uLightColor;
    
    gl_FragColor = vec4(fragColor.r, fragColor.g, fragColor.b, 1.0);
}

