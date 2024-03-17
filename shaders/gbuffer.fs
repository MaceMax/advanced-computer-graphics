#version 410 core
layout (location = 0) out vec4 gPositionAmbient; // xyz = position, w = ambient r value
layout (location = 1) out vec4 gNormalAmbient; // xyz = normal, w = ambient g value
layout (location = 2) out vec4 gAlbedoAmbient; // rgb = albedo, a = ambient b value
layout (location = 3) out vec4 gAoMetallicRoughness; // x = ambient occlusion, yz metallic and roughness factors

in vec4 position;
in vec3 normal;
in vec2 texCoord;
in mat3 TBN;

const int MAX_MATERIAL_TEXTURES=8;
const int MAX_TEXTURES=2;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;

    float shininess;
    bool activeTextures[MAX_MATERIAL_TEXTURES];
    sampler2D textures[MAX_MATERIAL_TEXTURES];
};

struct Textures
{
  bool procedural;
  bool animated; 
  float time;
  int proceduralType;
  bool activeTextures[MAX_TEXTURES];
  sampler2D textures[MAX_TEXTURES];
};

// The front surface material
uniform Material material;
uniform Textures textureLayers;


// I borrowed this code just to make procedural textures.
//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+10.0)*x);
}

float aastep ( float threshold , float value ) {
    float afwidth = 0.7 * length ( vec2 ( dFdx ( value ) , dFdy ( value ) ) ) ;
    // GLSL ' s fwidth ( value ) is abs ( dFdx ( value ) ) + abs ( dFdy ( value ) )
    return smoothstep ( threshold - afwidth , threshold + afwidth , value ) ;
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


vec3 calculateProceduralTexture(vec2 texCoord, int proceduralType, bool isAnimated, float time)
{
    vec3 color = vec3(0.0);
    if (proceduralType == 0) // Checkerboard
    {
        float scale = 10.0;
        if (isAnimated) {
            scale = 10.0 + sin(time) * 5.0;
        }
        vec2 uv = texCoord * scale;
        vec2 fuv = fract(uv);
        float pattern = (mod(floor(uv.x) + floor(uv.y), 2.0));
        color = mix(vec3(0.0), vec3(1.0), pattern);
    }
    else if (proceduralType == 1) // perlin noise
    {
        float scale = 10.0;
        vec2 uv = texCoord * scale;
        if (isAnimated) {
            uv += vec2(time);
        }

        float perlin = 0.5 + 0.5 * snoise(uv);
        color = vec3(perlin);
    } else if (proceduralType == 2) // cow pattern
    {
        float scale = 100.0;
        vec2 uv = texCoord * scale;
        if (isAnimated) {
            uv += vec2(time);
        }

        float cow = snoise(uv);
        cow += 0.5 * snoise(2.0 * uv);
        cow = aastep(0.05, cow);
        color = vec3(cow);
    }
    return color;
}

void main()
{
    gPositionAmbient.xyz = position.xyz;

    gNormalAmbient.xyz = normalize(normal);
    if (material.activeTextures[3]) // Normal map
    {
        vec3 norm = texture(material.textures[3], texCoord).rgb;
        norm = norm * 2.0 - 1.0;
        gNormalAmbient.xyz = normalize(TBN * norm);
    }

    vec3 ambient = material.ambient.rgb;
    if (material.activeTextures[5]) { // Emissive map
        ambient = texture(material.textures[5], texCoord).rgb * 10;
    } else if (material.emission.r > 0.0 || material.emission.g > 0.0 || material.emission.b > 0.0) {
        ambient = material.emission.rgb * 10;
    }

    gPositionAmbient.w = ambient.r; // ambient factor
    gNormalAmbient.w = ambient.g; // ambient factor
    gAlbedoAmbient.w = ambient.b; // ambient factor

    gAlbedoAmbient.rgb = material.diffuse.rgb;
    if (material.activeTextures[0]) // Diffuse map
    {
        gAlbedoAmbient.rgb = texture(material.textures[0], texCoord).rgb;
    }

    vec4 blendedColor;
    int activeTextureCount = 0;

    // I am disappointed of this code. 
    for (int index = 0; index < MAX_TEXTURES; index++) {
        if (textureLayers.activeTextures[index]) {
        if (activeTextureCount == 0) {
            if (textureLayers.procedural) {
                blendedColor = vec4(calculateProceduralTexture(texCoord, textureLayers.proceduralType, textureLayers.animated ,textureLayers.time), 1.0);
            } else {
                blendedColor = texture(textureLayers.textures[index], texCoord);
            }
        } else {
            if (textureLayers.procedural) {
                vec4 layerColor = vec4(calculateProceduralTexture(texCoord, textureLayers.proceduralType, textureLayers.animated ,textureLayers.time), 1.0);
                blendedColor = blendedColor * layerColor;
            } else { 
                vec4 layerColor = texture(textureLayers.textures[index], texCoord);
                blendedColor = blendedColor * layerColor;
            }
        }
        activeTextureCount += 1;
        }
    }

    if (activeTextureCount > 0) {
        gAlbedoAmbient.rgb *= blendedColor.rgb;
    }

    gAoMetallicRoughness.y = material.specular.r; // No metallic or specular map, use specular color
    if (material.activeTextures[6]) // Metallic 
    {
        gAoMetallicRoughness.y = texture(material.textures[6], texCoord).r;
    } else if (material.activeTextures[1]) // Specular map
    {
        //vec3 specularColor = texture(material.textures[1], texCoord).rgb;
        //float specularGray = dot(specularColor, vec3(0.299, 0.587, 0.114)); // Convert to grayscale
        gAoMetallicRoughness.y = texture(material.textures[1], texCoord).r; // Use grayscale value as metallic factor
    }
        
    gAoMetallicRoughness.z = material.shininess;
    if (material.activeTextures[7]) // Roughness
    {
        gAoMetallicRoughness.z = texture(material.textures[7], texCoord).r;
    } 

    if (material.activeTextures[4]) // Ambient occlusion
    {
        gAoMetallicRoughness.x = texture(material.textures[4], texCoord).r;
    } else {
        gAoMetallicRoughness.x = 0.0;
    }

    

}
