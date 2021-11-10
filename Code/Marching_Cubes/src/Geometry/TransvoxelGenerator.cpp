#include "TransvoxelGenerator.h"

TransvoxelGenerator::TransvoxelGenerator(SDF* densityFunction)
    :densityFunction(densityFunction),
    generateShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.glsl"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/brush_functions.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/terrain_modification.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_generate.glsl")})
    ),
    countShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.glsl"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/brush_functions.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/terrain_modification.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_count.glsl")})
    ),
    polygonizeShader(Shader::ComputeShaderFromVector(std::vector<std::string>{
        Shader::ReadShaderFile("Shaders/Compute/shadertop.glsl"),
        densityFunction->getShaderCode(),
        Shader::ReadShaderFile("Shaders/Compute/brush_functions.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/terrain_modification.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_common.glsl"),
        Shader::ReadShaderFile("Shaders/Compute/transvoxel_polygonize.glsl")})
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
    //BEGIN DEBUG REGION


//    int debugCellIndex = 16;
//    std::cout << "Cell Index: " << debugCellIndex << std::endl;
//    int debugCellClass = TransvoxelTables::transitionCellClass[debugCellIndex];
//    std::cout << "Cell Class: " << debugCellClass << std::endl;
//    debugCellClass = debugCellClass & 0x7F;
//    std::cout << "0x7F transformation: " << debugCellClass << std::endl;
//    int debugPointCount = TransvoxelTables::transitionTotalTable[debugCellClass];
//    std::cout << "Point Count: " << debugPointCount << std::endl;
//    for (int i = 0; i < debugPointCount; i++) {
//        int debugVertexIndex = TransvoxelTables::transitionCellData[debugCellClass * 37 + i + 1];
//        std::cout << "Vertex index " << i << ": " << debugVertexIndex << std::endl;
//        int debugVertexPosition = TransvoxelTables::transitionVertexData[debugCellIndex * 12 + debugVertexIndex];
//        std::cout << "Vertex position data " << i << ": " << std::hex << debugVertexPosition << std::dec << std::endl;
//    }



    //END DEBUG REGION
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

    //Terrain Modification buffer

    //Temporary test objects:
    TestBrushes::generateRandomSpheres();
    TestBrushes::generateRandomCylinders();
    GLuint brushBuffer;

    BrushBoundingBox myBox = BrushBoundingBox::getChunkBox(chunkLocation,chunkSize,chunkStride);

    std::vector<BrushParams> brushList;
    // for (Brush* b : TestBrushes::randomSpheres) {
    //     if (b->getBoundingBox().intersects(myBox)) {
    //         brushList.push_back(b->getBrushParams());
    //     }
    // }
    // for (Brush* b : TestBrushes::randomCylinders) {
    //     if (b->getBoundingBox().intersects(myBox)) {
    //         brushList.push_back(b->getBrushParams());
    //     }
    // }

    for (Brush* b : Editing::allBrushes) {
        if (b->getBoundingBox().intersects(myBox)) {
            brushList.push_back(b->getBrushParams());
        }
    }
    glGenBuffers(1,&brushBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,16,brushBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,brushList.size() * sizeof(BrushParams),&brushList[0],GL_DYNAMIC_DRAW);

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

    glUniform1i(generateShader.getUniform("brushCount"),brushList.size());

    glDispatchCompute(1+chunkSize.x/8, 1+chunkSize.y/8, 1+chunkSize.z/8);

    //BEGIN DEBUG REGION
//
//    int dbsize = getDensityBufferSize(chunkSize,edgeIndex);
//    std::cout << "Densities:" << dbsize<< std::endl;
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER,densityValuesBuffer);
//    float* mappedDensities;
//    mappedDensities = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,dbsize,GL_MAP_READ_BIT);
//    for (int i = 0; i < dbsize; i++) {
//        std::cout << i << ": " << mappedDensities[i] << std::endl;
//    }
//    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //END DEBUG REGION

    //Stage 2

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    GLuint zero = 0;

    glUseProgram(countShader.getID());


    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,2,marchableCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint),&zero,GL_DYNAMIC_READ);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,3,pointCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER,sizeof(GLuint),&zero,GL_DYNAMIC_READ);

    GLuint maxMarchableSize = chunkSize.x * chunkSize.y * chunkSize.z;
    if (edgeIndex & 1) { //-x
        maxMarchableSize += (1+chunkSize.y) * (1+chunkSize.z);
    }
    if (edgeIndex & 2) { //+x
        maxMarchableSize += (1+chunkSize.y) * (1+chunkSize.z);
    }
    if (edgeIndex & 4) { //-y
        maxMarchableSize += (1+chunkSize.x) * (1+chunkSize.z);
    }
    if (edgeIndex & 8) { //+y
        maxMarchableSize += (1+chunkSize.x) * (1+chunkSize.z);
    }
    if (edgeIndex & 16) { //-z
        maxMarchableSize += (1+chunkSize.x) * (1+chunkSize.y);
    }
    if (edgeIndex & 32) { //+z
        maxMarchableSize += (1+chunkSize.x) * (1+chunkSize.y);
    }
    maxMarchableSize *= sizeof(glm::uvec4);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,marchableList);
    glBufferData(GL_SHADER_STORAGE_BUFFER,maxMarchableSize,NULL,GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,marchableList);

    glUniform3uiv(countShader.getUniform("chunkSize"),1,&chunkSize[0]);
    glUniform1i(countShader.getUniform("edgeIndex"),edgeIndex);

    glUniform1i(countShader.getUniform("generateTransitionCells"),Config::get<bool>("generate_transition_cells"));
    glUniform1i(countShader.getUniform("generateRegularCells"),Config::get<bool>("generate_regular_cells"));

    glDispatchCompute(1+chunkSize.x/8,1+chunkSize.y/8,1+chunkSize.z/8);

    //BEGIN DEBUG REGION


//    GLint sz;
//
//    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,marchableCounter);
//    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER,0,sizeof(GLuint),&sz);
//
//    if (sz > 0) {
//        std::cout << "Marchables:" << sz << std::endl;
//        glBindBuffer(GL_SHADER_STORAGE_BUFFER,marchableList);
//        glm::uvec4* mappedMarchables;
//        mappedMarchables = (glm::uvec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,sz,GL_MAP_READ_BIT);
//        for (int i = 0; i < sz; i++) {
//            std::cout << mappedMarchables[i].x << ", " << mappedMarchables[i].y << ", " << mappedMarchables[i].z << ": " << std::bitset<16>(mappedMarchables[i].w) << "(" << (mappedMarchables[i].w & 511) << ")" <<std::endl;
//        }
//        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//    }


    //END DEBUG REGION

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

    glUniform1f(polygonizeShader.getUniform("transitionWidth"),Config::get<float>("transition_width"));

    glUniform1i(polygonizeShader.getUniform("generateTransitionCells"),Config::get<bool>("generate_transition_cells"));
    glUniform1i(polygonizeShader.getUniform("generateRegularCells"),Config::get<bool>("generate_regular_cells"));
    glUniform1i(polygonizeShader.getUniform("interpolate"), Config::get<bool>("interpolate"));

    glUniform1i(polygonizeShader.getUniform("brushCount"),brushList.size());

    glDispatchCompute(jobCount,1,1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    *geometrySize = pointCount;

    //BEGIN DEBUG REGION
    // if (pointCount > 0) {
    //     std::cout << "Total points in mesh: " << pointCount << std::endl;
    // }

    // if (chunkLocation == glm::vec3(1,0,1)) {
    //     glBindBuffer(GL_SHADER_STORAGE_BUFFER,*vertexBuffer);
    //     glm::vec4* mappedVertices;
    //     mappedVertices = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,pointCount,GL_MAP_READ_BIT);
    //     for (int i = 0; i < pointCount; i++) {

    //         std::cout << mappedVertices[i].x << ", " << mappedVertices[i].y << ", " << mappedVertices[i].z << " : " << std::bitset<16>(mappedVertices[i].w)  << std::endl;
    //     }
    //     glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    // }
    //END DEBUG REGION

}

//Changes here may affect getArrID in transvoxel_common
int TransvoxelGenerator::getDensityBufferSize(glm::uvec3 chunkSize, int edgeIndex)
{
    //TODO - remove when getArrID is fixed
    return (2*chunkSize.x + 1) * (2*chunkSize.y + 1) * (2*chunkSize.z + 1);

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
