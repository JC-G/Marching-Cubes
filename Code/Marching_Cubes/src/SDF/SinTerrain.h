#ifndef SINTERRAIN_H
#define SINTERRAIN_H

#include <SDF.h>


class SinTerrain : public SDF
{
    public:
        SinTerrain();
        virtual ~SinTerrain();


        float distance(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;

    protected:

    private:
};

#endif // SINTERRAIN_H
