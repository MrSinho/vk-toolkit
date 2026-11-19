struct PushConstants {
    float4x4 projection;
    float4x4 view;
};

struct Light {
    float4 lightPosition;
    float4 lightColor;
};

ParameterBlock<Light> light;








struct VertexInput { // fields
    //[[vk::location(0)]] // Better garantee some locations (TODO: OR USE BY NAME)
    float3 position : POSITION;  // User defined input semantic, takes a vertex input location

    //[[vk::location(1)]]
    float4x4 model  : TEXCOORD1; // Per instance
    
    uint instanceID : SV_InstanceID; // System defined semantic, does not consume a location
};

struct VertexOutput {
    float4 position      : SV_Position; // = gl_Position, interpolated for the fragment shader
    float4 fragPosition  : TEXCOORD0; // also interpolated

    nointerpolation float instanceFactor : TEXCOORD1;
};

[[vk::push_constant]]
PushConstants pConst;

[shader("vertex")] // otherwise the main entry point will not be visible to the slang compiling API
VertexOutput vertexMain(VertexInput input) { // `input` is an entry point parameter
    VertexOutput output;

    output.instanceFactor = 1.0 / (float(input.instanceID) + 1.0); // equivalent to gl_InstanceIndex
    output.fragPosition   = mul(input.model, float4(input.position, 1.0));
    output.position       = mul(pConst.projection, mul(pConst.view, output.fragPosition));
    
    //output.instanceFactor = 1.0 / (float(input.instanceID) + 1.0); // equivalent to gl_InstanceIndex
    //output.fragPosition   = float4(input.position, 1.0);
    //output.position       = output.fragPosition;
    
    //output.position = mul(output.position, light.lightColor); // To show merging descriptor sets

    return output;
}









struct FragmentInput {
    float4 fragPosition   : TEXCOORD0;
    float  instanceFactor : TEXCOORD1;
};

struct FragmentOutput {
    float4 color : SV_Target0; // slang semantic, define the first render target, straight to the first rendering attachment (color attachment)
};

[[vk::constant_id(1)]] // Specialization constant, typed compile time parameter. Specialization constants require an ID
const float LIGHT_INTENSITY = 0.0; // TODO: OR ACCESS BY NAME

[shader("fragment")]
FragmentOutput fragmentMain(FragmentInput input) {

    FragmentOutput output;

    float4 dir = input.fragPosition - light.lightPosition;

    float3 result = float3(0.01) +
                    LIGHT_INTENSITY * normalize(float3(input.instanceFactor, light.lightColor.y, light.lightColor.z)) 
                    //normalize(float3(input.instanceFactor, light.lightColor.y, light.lightColor.z)) 
                        / dot(dir.xyz, dir.xyz);

    output.color = float4(result, 0.75);

    return output;
}