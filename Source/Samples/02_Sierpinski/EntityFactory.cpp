#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

#include "Mover.h"
#include "Particle.h"
#include "KochFactory.h"
#include "SierpinskiFactory.h"
#include "EntityFactory.h"

EntityFactory::EntityFactory(Application* app) : app_(app)
{
    colShader_ = std::make_shared<Shader>("../Shaders/Col.vs", "../Shaders/Col.fs");
    vColShader_ = std::make_shared<Shader>("../Shaders/VCol.vs", "../Shaders/VCol.fs");
    SetColor(MAGENTA);

    auto points = KochFactory::Snowflake(3);
    snowflakeMesh_ = new Mesh();
    snowflakeMesh_->mode_ = GL_LINE_LOOP;
    snowflakeMesh_->GenArrayBuffer(points);
}

void EntityFactory::SetColor(const vec3 &color)
{
    colShader_->Use();
    colShader_->SetVec4("uColor", vec4(color, 1.0));    
}

std::shared_ptr<Entity> EntityFactory::CreateEntity(EntityType type, bool vCol)
{
    std::shared_ptr<Entity> entity;
    switch (type) {
    case MOVER_ENTITY: entity = std::make_shared<Mover>();
        break;
    case PARTICLE_ENTITY: entity = std::make_shared<Particle>();
        break;
    default: entity = std::make_shared<Entity>();
        break;
    }
    
    entity->material_ = std::make_shared<Material>();
    app_->entities_.push_back(entity);
    entity->material_->shader_ = colShader_;
    if (vCol) {
        entity->material_->shader_ = vColShader_;
    }
    return entity;
}

std::shared_ptr<Entity> EntityFactory::CreateLineGasket(int numDivisions, const vec2& varRange, bool threeD, bool vCol)
{
    auto entity = CreateEntity(MOVER_ENTITY, vCol);
    
    SierpinskiFactory::varRange_ = varRange;
    auto points = threeD ? SierpinskiFactory::Sierpinski3DDet(numDivisions) :
        SierpinskiFactory::Sierpinski2DDet(numDivisions);
    std::vector<vec3> line;
    entity->mesh_->mode_ = GL_LINES;
    for (int i = 0; i < points.size(); i += 3)
    {
        line.push_back(points.at(i));
        line.push_back(points.at(i+1));
        line.push_back(points.at(i+1));
        line.push_back(points.at(i+2));
        line.push_back(points.at(i+2));
        line.push_back(points.at(i));
    }
    entity->mesh_->GenArrayBuffer(line);
    
    if (!vCol) return entity;
    
    std::vector<vec3> colors;
    for (int i = 0; i < line.size(); i += 6)
    {
        colors.push_back(color1_); colors.push_back(color1_);
        colors.push_back(color2_); colors.push_back(color2_);
        colors.push_back(color3_); colors.push_back(color3_);
    }
    entity->mesh_->GenArrayBuffer(colors);

    auto mover = dynamic_cast<Mover*>(entity.get());
    mover->aVelocity_.y = quarter_pi<float>();
    
    return entity;
}

std::shared_ptr<Entity> EntityFactory::CreateTriGasket(int numDivisions, const vec2& varRange, bool threeD, bool vCol)
{
    auto entity = CreateEntity(BASE_ENTITY, vCol);
    
    SierpinskiFactory::varRange_ = varRange;
    auto points = threeD ? SierpinskiFactory::Sierpinski3DDet(numDivisions) :
        SierpinskiFactory::Sierpinski2DDet(numDivisions);
    entity->mesh_->GenArrayBuffer(points);

    entity->mesh_->mode_ = GL_TRIANGLES;
    
    if (!vCol) return entity;
    
    std::vector<vec3> colors;
    for (int i = 0; i < points.size(); i += 3)
    {
        int idx = (i / 3) % 3;
        vec3 color = color1_;
        if (idx == 1) {
            color = color2_;
        } else if (idx == 2) {
            color = color3_;
        }
        for (int j = 0; j < 3; ++j) colors.push_back(color);
    }
    
    entity->mesh_->GenArrayBuffer(colors);
    
    return entity;
}

std::shared_ptr<Entity> EntityFactory::CreateSnowflake(int numDivisions)
{
    auto entity = CreateEntity(PARTICLE_ENTITY, false);

    auto points = KochFactory::Snowflake(numDivisions);

    entity->mesh_->mode_ = GL_LINE_LOOP;
    entity->mesh_->GenArrayBuffer(points);

    // entity->mesh_ = std::shared_ptr<Mesh>(snowflakeMesh_);
    SetColor(color3_);
    Particle* particle = dynamic_cast<Particle*>(entity.get());
    particle->velocity_ = linearRand(LEFT/2.f, RIGHT/2.f);
    particle->velocity_.y = linearRand(0.8f, 1.2f);
    particle->mass_ = linearRand(0.5f, 2.f);
    particle->gravity_.y = -0.75f;
    particle->SetScale(linearRand(0.02f, 0.1f));
    particle->lifespan_ = linearRand(2.f, 3.f);
    particle->Translate(UP);

    // particle->velocity_.x = -.5f;
    // particle->dragC_ = 3.f;
    

    return entity;
}
