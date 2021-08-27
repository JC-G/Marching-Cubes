#ifndef SPHERE_H
#define SPHERE_H

#include <SDF.h>


class Sphere : public SDF
{
    public:
        Sphere(glm::vec3 center, float radius);
        virtual ~Sphere();

        float density(glm::vec3 inPos) override;
        glm::vec3 normal(glm::vec3 inPos) override;

        std::string getShaderCode() override;

    protected:

    private:
        glm::vec3 center;
        float radius;
};

#endif // SPHERE_H
