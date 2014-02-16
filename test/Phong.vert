attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uColor;

uniform float uMaterial;

varying vec3 vColor;
varying vec3 vNormal;
varying vec4 vPosition;

void main() {
    vPosition = uModelMatrix * vec4(aPosition.xyz, 1);   
    vNormal = normalize(uModelMatrix * vec4(aNormal, 0)).xyz;
    vColor = uColor;

    gl_Position = uProjMatrix * uViewMatrix * vPosition;
}
