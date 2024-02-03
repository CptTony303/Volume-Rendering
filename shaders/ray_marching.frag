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


//transfer functions
float transferFunctionDensity(float value){
    for(int i = 0; i < numberOfDensityPoints; i++){
        if(value<=transfer_function_density[i].x){
            //check for out of bounds
            if(i == 0){
                return transfer_function_density[i].y;
            }else{
            //linear interpolation
                float y0 = transfer_function_density[i-1].y;
                float y1 = transfer_function_density[i].y;
                float x0 = transfer_function_density[i-1].x;
                float x1 = transfer_function_density[i].x;
                float x = value;

                return (y0*(x1-x)+y1*(x-x0))/(x1-x0);
            }
        }
    }
    return 0; //should never reach this
}
vec3 transferFunctionColor(float value){
    float color = -1;
    for(int i = 0; i < numberOfColorPoints; i++){
        if(value<=transfer_function_color[i].x && color < 0){
            //check for out of bounds
            if(i == 0){
                value = transfer_function_color[i].y;
            }else{
            //linear interpolation
                float y0 = transfer_function_color[i-1].y;
                float y1 = transfer_function_color[i].y;
                float x0 = transfer_function_color[i-1].x;
                float x1 = transfer_function_color[i].x;
                float x = value;
                color = (y0*(x1-x)+y1*(x-x0))/(x1-x0);
            }
        }
    }
    color -= 0.5;
    float red = color > 0 ? color * 2.f : 0;
    float green = 1.f - 2.f* abs(color);
    float blue = color < 0 ? color * -2.f : 0;;
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
    vec3 last_Lx = vec3(0.f);
//    vec3 last_Lx = vec3(0.f);

    float last_density = 0.f;

    for (float i = 0.0; i <= maxDistance; i += stepSize)
    {
        vec3 texCoord = currentPosition+vec3(0.5f);

        float volumeData = texture(volume, texCoord).r;

        vec3 unit_emission = transferFunctionColor(volumeData);;
        float unit_opacity = transferFunctionDensity(volumeData);
        float unit_transmittance = 1.0 - unit_opacity;
        float eps = 1e-8;
        float mu_t = -log(clamp(unit_transmittance, eps, 1.0 - eps));
        vec3 Le = (unit_opacity > eps) ? vec3(unit_emission/unit_opacity) : vec3(0.f);
        float t = exp(-mu_t*stepSize*unit_opacity);
        vec3 Lx = t * mu_t * Le;
        vec3 L = stepSize * 0.5 * (Lx + last_Lx);
        
        rgba += vec4(L,t);
        last_Lx = Lx;

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