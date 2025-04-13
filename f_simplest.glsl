#version 330

uniform sampler2D textureMap0; 
uniform sampler2D textureMap1;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec2 iTexCoord0;
in vec2 iTexCoord1;
in vec4 ic; 
in vec4 n;
in vec4 l1; // wektor do pierwszego światła
in vec4 l2; // wektor do drugiego światła
in vec4 v;

void main(void) {
    // Znormalizowane interpolowane wektory
    vec4 ml1 = normalize(l1);
    vec4 ml2 = normalize(l2);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);
    
    // Parametry powierzchni
    vec4 kd = mix(texture(textureMap0, iTexCoord0), texture(textureMap1, iTexCoord1), 0.3);
    
    // Obliczenie modelu oświetlenia
    float nl1 = clamp(dot(mn, ml1), 0, 1);
    float nl2 = clamp(dot(mn, ml2), 0, 1);

    // Kolor piksela jest sumą wpływu obu źródeł światła
    pixelColor = vec4(kd.rgb * (nl1 + nl2), kd.a);
}
