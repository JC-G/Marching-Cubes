#ifndef SCALEDTERRAIN_H
#define SCALEDTERRAIN_H

#include <SDF.h>

class ScaledTerrain : public SDF
{
    public:
        ScaledTerrain();
        virtual ~ScaledTerrain();

        float distance(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;

    protected:

    private:
};

#endif