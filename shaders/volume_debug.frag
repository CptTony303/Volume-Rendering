#version 330 core

out vec4 FragColor;

in vec3 modelPos; //intersection with cube in model position

uniform float randomizer;
uniform int methode;

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

void main()
{
    switch(methode){
    case 2:
        init_seed(randomizer*modelPos+randomizer);
        FragColor = vec4(get_random_float(), get_random_float(), get_random_float(), 1.f);
        break;
    case 4:
        FragColor = vec4(modelPos+vec3(0.5f), 1.f);
    }
}
