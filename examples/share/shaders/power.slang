RWStructuredBuffer<float> values;

StructuredBuffer<float> factors;



[shader("compute")]
[numthreads(64, 1, 1)]
void computeMain(uint3 dispatchThreadId : SV_DispatchThreadID) {
    uint index = dispatchThreadId.x;
    float value = values[index];
    values[index] = value * value * factors[0];
}
