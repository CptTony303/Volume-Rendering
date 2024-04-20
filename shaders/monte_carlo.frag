#version 330 core

out vec4 FragColor;

in vec3 modelPos; //intersection with cube in model position
in vec2 lastFrameCoord;

uniform mat4 model; //model matrix
uniform mat4 view; //view matrix

uniform sampler3D volume; //texture of the volume
uniform int numberOfColorPoints;
uniform vec2 transfer_function_color[100];
uniform int numberOfDensityPoints;
uniform vec2 transfer_function_density[100];

uniform int lastNumberOfColorPoints;
uniform vec2 transfer_function_control_color[100];
uniform int lastNumberOfDensityPoints;
uniform vec2 transfer_function_control_density[100];
uniform sampler2D convergedFrame;
uniform bool useControlVariate;

uniform float brightness;
uniform int samplesPerFrame;
uniform float randomizer;

vec3 color_f1;
vec3 color_h1;
vec3 variance_f1;
vec3 variance_h1;
vec3 covariance_f1_h1;


/*BEGIN simple substitution for rng*/
/// 1 out, 3 in
float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}
///  3 out, 3 in...
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}
vec3 seed = vec3(0.f);
float get_random_float(){
    //get random number with hash
    float random_number = hash13(seed);
    //increment seed
    seed = random_number * 1000 * seed;
    seed += 5.36256;
    seed = hash33(seed);
    return random_number;
}
void init_seed(vec3 init_seed){
    seed = init_seed;
    //randomize once
    get_random_float();
}
/*END simple substitution for rng*/

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
float transferFunctionControlDensity(float value){
    for(int i = 0; i < lastNumberOfDensityPoints; i++){
        if(value<=transfer_function_control_density[i].x){
            //check for out of bounds
            if(i == 0){
                return transfer_function_control_density[i].y;
            }else{
            //linear interpolation
                float y0 = transfer_function_control_density[i-1].y;
                float y1 = transfer_function_control_density[i].y;
                float x0 = transfer_function_control_density[i-1].x;
                float x1 = transfer_function_control_density[i].x;
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
vec3 transferFunctionControlColor(float value){
    float color = -1;
    for(int i = 0; i < lastNumberOfColorPoints; i++){
        if(value<=transfer_function_control_color[i].x && color < 0){
            //check for out of bounds
            if(i == 0){
                value = transfer_function_control_color[i].y;
            }else{
            //linear interpolation
                float y0 = transfer_function_control_color[i-1].y;
                float y1 = transfer_function_control_color[i].y;
                float x0 = transfer_function_control_color[i-1].x;
                float x1 = transfer_function_control_color[i].x;
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

void deltaTracking(vec3 w){
    vec3 x = modelPos;
    float mu = brightness;
    float weight = 1.f;
    float weightControl = 1.f;

    color_f1 = vec3(0.f);
    color_h1 = vec3(0.f);
    variance_f1 = vec3(0.f);
    variance_h1 = vec3(0.f);
    covariance_f1_h1 = vec3(0.f);
    vec3 color_f1h1 = vec3(0.f);
    vec3 mean_f1 = vec3(0.f);
    vec3 mean_h1 = vec3(0.f);
    float sumW = 0.f;
    float sumV = 0.f;
    float sumVW = 0.f;
    vec3 M_f1 = vec3(0.f);
    vec3 M_h1 = vec3(0.f);
    int n = 0;

    while(true){
        n++;
        float rng1 = get_random_float();
        float rng2 = get_random_float();

        float t = - log (1 - rng1)/mu;
        x += t * w;

        vec3 texCoord = x+vec3(0.5f);
        float volumeData = texture(volume, texCoord).r;
        float density = transferFunctionDensity(volumeData)*mu;
        float densityControl = 0.f;
        vec3 fi = transferFunctionColor(volumeData);
        float wi = weight * (density / mu);

        if(useControlVariate){
        vec3 hi = transferFunctionControlColor(volumeData);
        densityControl = transferFunctionControlDensity(volumeData)*mu;
        float vi = weightControl * (densityControl/mu);
        vec3 mean_f1_inc = mean_f1;
        vec3 mean_h1_inc = mean_h1;
        sumW += wi;
        sumV += vi;
        sumVW += vi*wi;

        if(abs(sumW) > 0){
            mean_f1_inc += wi*(fi - mean_f1)/sumW;
        }
        if(abs(sumV) > 0){
            mean_h1_inc += vi * (hi - mean_h1) / sumV;
        }
        M_f1 += wi * (fi - mean_f1) * (fi - mean_f1_inc);
        M_h1 += vi * (hi - mean_h1) * (hi - mean_h1_inc);

        if(abs(sumVW) > 0){
            covariance_f1_h1 += wi*vi/sumVW * ((fi - mean_f1_inc) * (hi - mean_h1_inc) - covariance_f1_h1);
        }

        mean_f1 = mean_f1_inc;
        mean_h1 = mean_h1_inc;

        color_h1 += vi * hi;
        color_f1h1 += vi*wi*fi*hi;
        }
        color_f1 += wi * fi;


        float mu_n = mu - density;
        float Pa = density / (density + abs(mu_n));
        float Pn = abs(mu_n)/(density + abs(mu_n));
        weight = abs( weight * mu_n/(mu));
        if(rng2 < Pa || distance(x,modelPos) > 1.8){
            if(useControlVariate){
//            color_h1 /= sumV;
            variance_f1 = M_f1 / sumW;
            variance_h1 = M_h1 / sumV;
//            covariance_f1_h1 = color_f1h1 - color_f1 * color_h1;
            }
//            color_f1 /= sumW;
            return;
        }
        if(useControlVariate){
        float mu_n_control = mu - densityControl;
        weightControl = weightControl * mu_n_control/(mu);
        }
    }
}


void main(){
    init_seed(randomizer*modelPos+randomizer);
    vec3 worldPos = vec3(model * vec4(modelPos, 1.0));
    mat4 inverseView = inverse(view);
    mat4 inverseModel = inverse(model);
    vec3 cameraPosition = vec3(inverseView[3]);
    //float dist = length(worldPos - cameraPosition);
    vec3 cameraPositionInModel = vec3(inverseModel * vec4(cameraPosition,1.f) ); //eine von beiden Mult wird schon passen---> kamearapos in Modll space  
    vec3 direction = normalize(modelPos - cameraPositionInModel);

    vec3 f1 = vec3(0.f);
    vec3 h1 = vec3(0.f);
    vec3 var_f1 = vec3(0.f);
    vec3 var_h1 = vec3(0.f);
    vec3 cov_f1_h1 = vec3(0.f);
    int sample_nr = 0;
    while(sample_nr <samplesPerFrame){
        deltaTracking(direction);
        sample_nr++;
        f1 += color_f1;
        if(useControlVariate){
            h1 += color_h1;
            var_f1 += variance_f1;
            var_h1 += variance_h1;
            cov_f1_h1 += covariance_f1_h1;
        }
    }
    f1 /= samplesPerFrame;
    FragColor = vec4(f1,1.f);
    if(useControlVariate){
        vec3 h0 = texture(convergedFrame, lastFrameCoord).rgb;
        h1 /= samplesPerFrame;
        var_f1 /= samplesPerFrame;
        var_h1 /= samplesPerFrame;
        cov_f1_h1 /= samplesPerFrame;

        vec3 f_star = f1 - h1 + h0;
       
        vec3 var_f_star = var_f1 + var_h1 - 2 * cov_f1_h1;
        vec3 cov_f_star_f1 = var_f1 - cov_f1_h1;

        mat2 covMat_f1_f_star_x_inverse = inverse(mat2(var_f1.x, cov_f_star_f1.x, cov_f_star_f1.x, var_f_star.x));
        mat2 covMat_f1_f_star_y_inverse = inverse(mat2(var_f1.y, cov_f_star_f1.y, cov_f_star_f1.y, var_f_star.y));
        mat2 covMat_f1_f_star_z_inverse = inverse(mat2(var_f1.z, cov_f_star_f1.z, cov_f_star_f1.z, var_f_star.z));

        vec2 e = vec2(1.0);
        vec2 weights_x = (e * covMat_f1_f_star_x_inverse) / dot(e * covMat_f1_f_star_x_inverse, e);
        vec2 weights_y = (e * covMat_f1_f_star_y_inverse) / dot(e * covMat_f1_f_star_y_inverse, e);
        vec2 weights_z = (e * covMat_f1_f_star_z_inverse) / dot(e * covMat_f1_f_star_z_inverse, e);
        vec3 weight_f1 = vec3(weights_x.x,weights_y.x,weights_z.x);
        vec3 weight_f_star = vec3(weights_x.y,weights_y.y,weights_z.y);
        vec3 F = weight_f1 * f1 + weight_f_star * f_star;

        FragColor = clamp(vec4(F, 1.f), 0.0, 1.0);
        return;

        FragColor = vec4(cov_f_star_f1.x, cov_f_star_f1.y,cov_f_star_f1.z,0.01)*100;
    }
}