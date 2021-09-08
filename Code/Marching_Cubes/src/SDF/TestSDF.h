#ifndef TESTSDF_H
#define TESTSDF_H

#include "SDF.h"


class TestSDF : public SDF
{
    public:
        TestSDF();
        virtual ~TestSDF();

        float density(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;
    protected:

    private:
};

#endif // TESTSDF_H
