#include "TransvoxelGenerator.h"

TransvoxelGenerator::TransvoxelGenerator(SDF* densityFunction)
    :densityFunction(densityFunction),
    generateShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.txt"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.txt"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_generate.txt")})
    ),
    countShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.txt"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.txt"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_count.txt")})
    ),
    polygonizeShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.txt"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.txt"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_polygonize.txt")})
    )
{
    /*algorithm shape:
    generate density function, with additional values where edgeIndex requires (shader 1)
    count polygons and get case index (shader 2) - here is where the cell splitting occurs
    actually generate the polygons (shader 3)
    */
    glGenBuffers(1,&densityValuesBuffer);

    glGenBuffers(1,&marchableCounter);
    glGenBuffers(1,&pointCounter);
    glGenBuffers(1,&marchableList);
    glGenBuffers(1,&triangleCounter);

    glGenBuffers(1,&regularCellClassBuffer);
    glGenBuffers(1,&regularCellDataBuffer);
    glGenBuffers(1,&regularVertexDataBuffer);
    glGenBuffers(1,&regularTotalTableBuffer);

    glGenBuffers(1,&transitionCellClassBuffer);
    glGenBuffers(1,&transitionCellDataBuffer);
    glGenBuffers(1,&transitionVertexDataBuffer);
    glGenBuffers(1,&transitionTotalTableBuffer);



}

TransvoxelGenerator::~TransvoxelGenerator()
{
    //dtor
}


void TransvoxelGenerator::GenerateGeometry(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GLuint* vertexBuffer, GLuint* normalBuffer, GLuint* geometrySize, int edgeIndex)
{
    glGenBuffers(1,vertexBuffer);
    glGenBuffers(1,normalBuffer);

    //transvoxel algorithm tables
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,8,regularCellClassBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int),TransvoxelTables::regularCellClass,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,9,regularCellDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int),TransvoxelTables::regularCellData,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,10,regularVertexDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,3072*sizeof(int),TransvoxelTables::regularVertexData,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,11,regularTotalTableBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,16*sizeof(int),TransvoxelTables::regularTotalTable,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,12,transitionCellClassBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,512*sizeof(int),TransvoxelTables::transitionCellClass,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,13,transitionCellDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,2072*sizeof(int),TransvoxelTables::transitionCellData,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,14,transitionVertexDataBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,6144*sizeof(int),TransvoxelTables::transitionVertexData,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,15,transitionTotalTableBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,56*sizeof(int),TransvoxelTables::transitionTotalTable,GL_DYNAMIC_DRAW);

    //Stage 1

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,densityValuesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,getDensityBufferSize(chunkSize,edgeIndex) * sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,densityValuesBuffer);

    glUseProgram(generateShader.getID());
    glUniform3fv(generateShader.getUniform("chunkPosition"),1,&chunkLocation[0]);
    glUniform3fv(generateShader.getUniform("chunkStride"),1,&chunkStride[0]);
    glUniform3uiv(generateShader.getUniform("chunkSize"),1,&chunkSize[0]);
    glUniform1i(generateShader.getUniform("edgeIndex"),edgeIndex);

    glDispatchCompute(1+chunkSize.x/8, 1+chunkSize.y/8, 1+chunkSize.z/8);

    //Stage 2

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    GLuint zero = 0;

    glUseProgram(countShader.getID());


    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,2,marchableCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint),&zero,GL_DYNAMIC_READ);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,3,pointCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint),&zero,GL_DYNAMIC_READ);

    //TODO - the size will increase based on edges - calculate this when we come to transition cells
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,marchableList);
    glBufferData(GL_SHADER_STORAGE_BUFFER,chunkSize.x * chunkSize.y * chunkSize.z * sizeof(glm::uvec4),NULL,GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,marchableList);

    glUniform3uiv(countShader.getUniform("chunkSize"),1,&chunkSize[0]);
    glUniform1i(countShader.getUniform("edgeIndex"),edgeIndex);

    glDispatchCompute(1+chunkSize.x/8,1+chunkSize.y/8,1+chunkSize.z/8);

    //Stage 3

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    GLuint pointCount;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,pointCounter);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER,0,sizeof(GLuint),&pointCount);

    GLuint marchCount;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,marchableCounter);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER,0,sizeof(GLuint),&marchCount);

    int jobSize = 256; //TODO - make consistent with shaders
    int jobCount = 1 + marchCount/jobSize;

    glUseProgram(polygonizeShader.getID());

    glUniform3fv(polygonizeShader.getUniform("chunkPosition"),1,&chunkLocation[0]);
    glUniform3fv(polygonizeShader.getUniform("chunkStride"),1,&chunkStride[0]);
    glUniform1ui(polygonizeShader.getUniform("marchableCount"),marchCount);
    glUniform3uiv(polygonizeShader.getUniform("chunkSize"),1,&chunkSize[0]);

    glUniform1i(polygonizeShader.getUniform("edgeIndex"),edgeIndex);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,7,triangleCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint),&zero,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,5,*vertexBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,pointCount * sizeof(glm::vec4),NULL,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,6,*normalBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,pointCount*sizeof(glm::vec4),NULL,GL_DYNAMIC_DRAW);

    glDispatchCompute(jobCount,1,1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    *geometrySize = pointCount;

}
int TransvoxelGenerator::getDensityBufferSize(glm::uvec3 chunkSize, int edgeIndex)
{
    int total = (chunkSize.x - 1) * (chunkSize.y - 1) * (chunkSize.z - 1);
    if (edgeIndex & 1) {
        total += (2 * chunkSize.y + 1) * (2 * chunkSize.z + 1);
    } else {
        total += (chunkSize.y + 1) * (chunkSize.z + 1);
    }
    if (edgeIndex & 2) {
        total += (2 * chunkSize.y + 1) * (2 * chunkSize.z + 1);
    } else {
        total += (chunkSize.y + 1) * (chunkSize.z + 1);
    }
    if (edgeIndex & 4) {
        total += (2 * chunkSize.x + 1) * (2 * chunkSize.z + 1);
    } else {
        total += (chunkSize.x + 1) * (chunkSize.z + 1);
    }
    if (edgeIndex & 8) {
        total += (2 * chunkSize.x + 1) * (2 * chunkSize.z + 1);
    } else {
        total += (chunkSize.x + 1) * (chunkSize.z + 1);
    }
    if (edgeIndex & 16) {
        total += (2 * chunkSize.y + 1) * (2 * chunkSize.x + 1);
    } else {
        total += (chunkSize.y + 1) * (chunkSize.x + 1);
    }
    if (edgeIndex & 32) {
        total += (2 * chunkSize.y + 1) * (2 * chunkSize.x + 1);
    } else {
        total += (chunkSize.y + 1) * (chunkSize.x + 1);
    }
    return total;
}
