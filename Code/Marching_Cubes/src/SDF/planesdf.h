#ifndef PLANESDF_H
#define PLANESDF_H



#include <SDF.h>


class PlaneSDF : public SDF
{
    public:
        PlaneSDF();
        virtual ~PlaneSDF();

        float density(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;

    protected:

    private:
};

#endif // PLANESDF_H
