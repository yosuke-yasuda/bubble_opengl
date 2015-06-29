// vertex shader of bump & cube environment mapping

attribute vec3 tangent;

/* 視点座標系 */
varying vec4 position;
varying vec3 normal;

void main(void)
{
    position = gl_ModelViewMatrix * gl_Vertex;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = ftransform();
}