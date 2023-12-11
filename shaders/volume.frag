#version 330 core
out vec4 FragColor;
  

in vec3 modelPos; //intersection with cube in model position

uniform sampler3D volumeTexture; //texture of the volume
uniform float isovalue; //treshold for the density
uniform mat4 model; //model matrix
uniform mat4 view; //view matrix

vec4 rayMarch()
{
    //mat3 rotation = mat3(model);
    vec3 worldPos = vec3(model * vec4(modelPos, 1.0));
    mat4 inverseView = inverse(view);
    mat4 inverseModel = inverse(model);
    vec3 cameraPosition = vec3(inverseView[3]);
    float dist = length(worldPos - cameraPosition);
    vec3 cameraPositionInModel = vec3(inverseModel * vec4(cameraPosition,1.f) ); //eine von beiden Mult wird schon passen---> kamearapos in Modll space  
    vec3 direction = normalize(modelPos - cameraPositionInModel);

//    vec3 scale;
//    scale.x = length(vec3(model[0]));
//    scale.y = length(vec3(model[1]));
//    scale.z = length(vec3(model[2]));


    float stepSize = 0.001f; // Schrittweite
    float maxDistance = 1.8; // Maximaler Distanz für den Marsch = 1.8 weil wurzel 3 =diagonale vom 3dim Würfel 

    vec3 currentPosition = modelPos;

    vec3 color = vec3(0.0f, 0.0f, 0.0f);


    for (float i = 0.0; i <= maxDistance; i += stepSize)
    {
        // Ray-Box-Intersection und Voxel-Wert-Abfrage hier implementieren
        vec3 texCoord = currentPosition+vec3(0.5f);
        vec3 reversedTexCoord = vec3(1.0 - texCoord.x, texCoord.z, texCoord.y);
        float density = texture(volumeTexture, texCoord).r;
        if(density > isovalue){
            dist += distance(model * vec4(currentPosition, 1.f), model * vec4(modelPos,1.f));
            vec3 color = texCoord;
            return vec4(color, 1.f);
        }

        currentPosition += direction * stepSize;
    }

    return vec4(color, 0.f);
}
vec4 monteCarloIntegrationStep(){
return vec4(1.f,1.f,1.f,1.f);
}
void main()
{
    //vec3 reversedTexCoord = vec3(1.0 - TexCoord.x, 1.0 - TexCoord.y, TexCoord.z);
    //vec3 direction = normalize(vec3(0.5f, 0.5f, 0.5f) - reversedTexCoord);
    FragColor = rayMarch();
    //FragColor = vec4(modelPos, 1.f);
}
