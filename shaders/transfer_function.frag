#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform int numberOfColorPoints;
uniform vec2 transfer_function_color[100];
uniform int numberOfDensityPoints;
uniform vec2 transfer_function_density[100];

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


void main(){
	if(TexCoords.y >= 0.5){
        FragColor = vec4(transferFunctionColor(TexCoords.x),1.f);
    }else{
        FragColor = vec4(vec3(transferFunctionDensity(TexCoords.x)),1.f);
    }
}