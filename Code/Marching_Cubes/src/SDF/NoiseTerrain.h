#ifndef NOISETERRAIN_H
#define NOISETERRAIN_H

#include <SDF.h>


class NoiseTerrain : public SDF
{
    public:
        NoiseTerrain();
        virtual ~NoiseTerrain();

        float density(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;

    protected:

    private:
};

#endif // NOISETERRAIN_H
