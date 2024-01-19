#version 330 core

out vec4 FragColor;

in vec3 modelPos; //intersection with cube in model position

uniform mat4 model; //model matrix
uniform mat4 view; //view matrix

uniform sampler3D volume; //texture of the volume
uniform int numberOfColorPoints;
uniform vec2 transfer_function_color[100];
uniform int numberOfDensityPoints;
uniform vec2 transfer_function_density[100];

uniform float stepSize;



vec3 transferFunction(float value){
    if(false){
        return vec3(value);
    }
    value -= 0.5;
    float red = value > 0 ? value * 2 : 0;
    float green = 1.f - 2* abs(value);
    float blue = value < 0 ? value * -2 : 0;;
    return vec3(red, green, blue);
}

void main()
{
    vec3 worldPos = vec3(model * vec4(modelPos, 1.0));
    mat4 inverseView = inverse(view);
    mat4 inverseModel = inverse(model);
    vec3 cameraPosition = vec3(inverseView[3]);
    vec3 cameraPositionInModel = vec3(inverseModel * vec4(cameraPosition,1.f) );
    vec3 direction = normalize(modelPos - cameraPositionInModel);

//    vec3 scale;
//    scale.x = length(vec3(model[0]));
//    scale.y = length(vec3(model[1]));
//    scale.z = length(vec3(model[2]));

    float maxDistance = 1.8; // Maximaler Distanz für den Marsch = 1.8 weil wurzel 3 =diagonale vom 3dim Würfel 

    vec3 currentPosition = modelPos;

    vec4 rgba = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 last_Lx = vec4(0.f);
//    vec3 last_Lx = vec3(0.f);

    float last_density = 0.f;

    for (float i = 0.0; i <= maxDistance; i += stepSize)
    {
        vec3 texCoord = currentPosition+vec3(0.5f);

        float density = texture(volume, texCoord).r;

        vec3 unit_emission = transferFunction(density);;
        float unit_opacity = density;
        float unit_transmittance = 1.0 - unit_opacity;
        float eps = 1e-8;
        float mu_t = -log(clamp(unit_transmittance, eps, 1.0 - eps));
        vec4 le = (unit_opacity > eps) ? vec4(unit_emission,unit_opacity) : vec4(0.f);
        float t = exp(-mu_t*stepSize*density);
        vec4 lx = t * mu_t * le;
        vec4 l = stepSize * 0.5 * (lx + last_Lx);
        
        rgba += l;
        last_Lx = lx;

//        vec3 unit_emission = vec3(density);
//        float unit_opacity = density;
//        float unit_transmittance = 1.0 - unit_opacity;
//        float eps = 1e-8;
//        float mu_t = -log(clamp(unit_transmittance, eps, 1.0 - eps));
//        vec3 Le = (unit_opacity > eps) ? (unit_emission / unit_opacity) : vec3(0.0);
//        float T = exp(-mu_t*stepSize*density);
//        vec3 Lx = T * mu_t * Le;
//        vec3 L = stepSize * 0.5 * (Lx + last_Lx);
//        vec4 L4 =  vec4(L, unit_opacity * stepSize);
//
//        rgba += L4;
//        last_Lx = Lx;

        currentPosition += direction * stepSize;
    }

    FragColor = rgba;
}