#version 330 core
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform vec4 grassParams;

vec2 quickHash(float p)
{
    vec2 r = vec2(
        dot(vec2(p), vec2(17.43267, 23.8934543)),
        dot(vec2(p), vec2(13.98342, 37.2435232))
    );

    return fract(sin(r) * 1743.54892229);
}

void main()
{
    int GRASS_SEGMENTS = int(grassParams.x);
    int GRASS_VERTICES = int(grassParams.y);
    float GRASS_WIDTH = grassParams.z;
    float GRASS_HEIGHT = grassParams.w;

    // Figure out grass offset
    vec2 hashedInstanceID = quickHash(float(gl_InstanceID)) * 2.0 - 1.0;
    vec3 grassOffset = vec3(hashedInstanceID.x, 0.0, hashedInstanceID.y) * 10.0;

    // Find out vertex id, > GRASS_VERTICES is other side
    int vertFB_ID = gl_VertexID % (GRASS_VERTICES * 2);
    int vertID = vertFB_ID % GRASS_VERTICES;

    // 0 - left, 1 = right
    int xTest = vertID & 0x1;
    int zTest = (vertFB_ID >= GRASS_VERTICES) ? 1 : -1;
    float xSide = float(xTest);
    float zSide = float(zTest);
    float heightPercent = float(vertID - xTest) / (float(GRASS_SEGMENTS) * 2.0);

    float width = GRASS_WIDTH;
    float height = GRASS_HEIGHT;

    // Calculate the vertex position;
    float x = (xSide - 0.5) * width;
    float y = heightPercent * height;
    float z = 0.0;

    vec3 grassLocalPosition = vec3(x, y, z) + grassOffset;

    gl_Position = projection * view * model * vec4(grassLocalPosition, 1.0);
}