#ifndef TESTSDF_H
#define TESTSDF_H

#include "SDF.h"


class TestSDF : public SDF
{
    public:
        TestSDF(float scale, float iso);
        virtual ~TestSDF();

        float distance(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;
    protected:

    private:
        float hash13(glm::vec3 p3);
        float noise3( glm::vec3 x);
        float perlin3(glm::vec3 x);
        const float HASHSCALE1 = .1031;
        float scale;
        float iso;
};

#endif // TESTSDF_H
