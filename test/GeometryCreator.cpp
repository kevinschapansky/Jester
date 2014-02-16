#include "GeometryCreator.h"
using namespace glm;


Mesh::Mesh()
    : PositionHandle(0), NormalHandle(0), IndexHandle(0), IndexBufferLength(0)
{}

Mesh::Mesh(std::vector<float> const & Positions, std::vector<float> const & Normals, std::vector<unsigned short> const & Indices)
    : PositionHandle(0), NormalHandle(0), IndexHandle(0), IndexBufferLength(0)
{
    IndexBufferLength = Indices.size();
    
    glGenBuffers(1, & PositionHandle);
    glBindBuffer(GL_ARRAY_BUFFER, PositionHandle);
    glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(float), & Positions.front(), GL_STATIC_DRAW);

    glGenBuffers(1, & NormalHandle);
    glBindBuffer(GL_ARRAY_BUFFER, NormalHandle);
    glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(float), & Normals.front(), GL_STATIC_DRAW);

    glGenBuffers(1, & IndexHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned short), & Indices.front(), GL_STATIC_DRAW);
}

Mesh * GeometryCreator::CreateCube(vec3 const & Size)
{
    std::vector<float> Positions, Normals;
    std::vector<unsigned short> Indices;

    static float const CubePositions[] =
    {
        -0.5, -0.5, -0.5, // back face verts [0-3]
        -0.5,  0.5, -0.5,
         0.5,  0.5, -0.5,
         0.5, -0.5, -0.5,
         
        -0.5, -0.5,  0.5, // front face verts [4-7]
        -0.5,  0.5,  0.5,
         0.5,  0.5,  0.5,
         0.5, -0.5,  0.5,
         
        -0.5, -0.5,  0.5, // left face verts [8-11]
        -0.5, -0.5, -0.5,
        -0.5,  0.5, -0.5,
        -0.5,  0.5,  0.5,
        
         0.5, -0.5,  0.5, // right face verts [12-15]
         0.5, -0.5, -0.5,
         0.5,  0.5, -0.5,
         0.5,  0.5,  0.5,
         
        -0.5,  0.5,  0.5, // top face verts [16-19]
        -0.5,  0.5, -0.5,
         0.5,  0.5, -0.5,
         0.5,  0.5,  0.5,
        
        -0.5, -0.5,  0.5, // bottom face verts [20-23]
        -0.5, -0.5, -0.5,
         0.5, -0.5, -0.5,
         0.5, -0.5,  0.5
    };
    Positions = std::vector<float>(CubePositions, CubePositions + 24 * 3);
    int i = 0;
    for (std::vector<float>::iterator it = Positions.begin(); it != Positions.end(); ++ it, ++ i)
        * it *= Size[i %= 3];

    static float const CubeNormals[] =
    {
         0,  0, -1, // back face verts [0-3]
         0,  0, -1,
         0,  0, -1,
         0,  0, -1,
         
         0,  0,  1, // front face verts [4-7]
         0,  0,  1,
         0,  0,  1,
         0,  0,  1,
         
        -1,  0,  0, // left face verts [8-11]
        -1,  0,  0,
        -1,  0,  0,
        -1,  0,  0,
        
         1,  0,  0, // right face verts [12-15]
         1,  0,  0,
         1,  0,  0,
         1,  0,  0,
        
         0,  1,  0, // top face verts [16-19]
         0,  1,  0,
         0,  1,  0,
         0,  1,  0,
        
         0, -1,  0, // bottom face verts [20-23]
         0, -1,  0,
         0, -1,  0,
         0, -1,  0
    };
    Normals = std::vector<float>(CubeNormals, CubeNormals + 24 * 3);

    static unsigned short const CubeIndices[] =
    {
         0,  1,  2, // back face verts [0-3]
         2,  3,  0,
         
         4,  7,  6, // front face verts [4-7]
         6,  5,  4,
         
         8, 11, 10, // left face verts [8-11]
        10,  9,  8,
         
        12, 13, 14, // right face verts [12-15]
        14, 15, 12,
         
        16, 19, 18, // top face verts [16-19]
        18, 17, 16,
         
        20, 21, 22, // bottom face verts [20-23]
        22, 23, 20
    };
    Indices = std::vector<unsigned short>(CubeIndices, CubeIndices + 12 * 3);

    return new Mesh(Positions, Normals, Indices);
}

Mesh * GeometryCreator::CreateCylinder(
    float const baseRadius, 
    float const topRadius, 
    float const height, 
    unsigned int const slices, 
    unsigned int const stacks)
{
    std::vector<float> Positions, Normals;
    std::vector<unsigned short> Indices;

    // Make bottom disc
    Positions.push_back(0.f); Positions.push_back(0.f); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);

    Positions.push_back(baseRadius); Positions.push_back(0.f); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);

    for (unsigned int i = 1; i <= slices; ++ i)
    {
        float const Angle = (float) i * 2.f * 3.14159f / (slices);
        Positions.push_back(cos<float>(Angle)*baseRadius);
        Positions.push_back(sin<float>(Angle)*baseRadius);
        Positions.push_back(0.f);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);
        Indices.push_back(0);
        Indices.push_back(i+1);
        Indices.push_back(i+0);
    }

    // Make top disc
    unsigned short const TopStart = Positions.size() / 3;
    Positions.push_back(0.f); Positions.push_back(0.f); Positions.push_back(height);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);

    Positions.push_back(topRadius); Positions.push_back(0.f); Positions.push_back(height);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);

    for (unsigned int i = 1; i <= slices; ++ i)
    {
        float const Angle = (float) i * 2.f * 3.14159f / slices;
        Positions.push_back(cos<float>(Angle)*topRadius);
        Positions.push_back(sin<float>(Angle)*topRadius);
        Positions.push_back(height);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);
        Indices.push_back(TopStart);
        Indices.push_back(TopStart + i+0);
        Indices.push_back(TopStart + i+1);
    }

    // Make sides
    unsigned int SideStart1 = 0, SideStart2 = 0;
    for (unsigned int j = 0; j <= stacks; ++ j)
    {
        float Interpolation = (float) j / stacks;
        float Radius = Interpolation * topRadius + (1.f - Interpolation) * baseRadius;

        SideStart2 = Positions.size() / 3;
        for (unsigned int k = 0; k <= slices; ++ k)
        {
            float const Angle = (float) k * 2.f * 3.14159f / slices;
            Positions.push_back(cos<float>(Angle)*Radius);
            Positions.push_back(sin<float>(Angle)*Radius);
            Positions.push_back(Interpolation * height);
            Normals.push_back(cos<float>(Angle)); Normals.push_back(sin<float>(Angle)); Normals.push_back(0.f);
        }

        if (j)
        {
            for (unsigned int k = 0; k < slices; ++ k)
            {
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart1 + k + 1);
                Indices.push_back(SideStart2 + k + 1);
            
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart2 + k + 1);
                Indices.push_back(SideStart2 + k);
            }
        }
        SideStart1 = SideStart2;
    }

    return new Mesh(Positions, Normals, Indices);
}

Mesh * GeometryCreator::CreateDisc(
    float const innerRadius, 
    float const outerRadius, 
    float const height, 
    unsigned int const slices, 
    unsigned int const stacks)
{
    std::vector<float> Positions, Normals;
    std::vector<unsigned short> Indices;

    // Make bottom disc
    Positions.push_back(innerRadius); Positions.push_back(0.f); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);

    Positions.push_back(outerRadius); Positions.push_back(0.f); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);

    for (unsigned int i = 1; i <= slices; ++ i)
    {
        float const Angle = (float) i * 2.f * 3.14159f / (slices);
        unsigned int const Current = Positions.size() / 3;
        Positions.push_back(cos<float>(Angle)*innerRadius);
        Positions.push_back(sin<float>(Angle)*innerRadius);
        Positions.push_back(0.f);
        Positions.push_back(cos<float>(Angle)*outerRadius);
        Positions.push_back(sin<float>(Angle)*outerRadius);
        Positions.push_back(0.f);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(-1.f);
        Indices.push_back(Current - 2);
        Indices.push_back(Current - 0);
        Indices.push_back(Current - 1);
        Indices.push_back(Current - 0);
        Indices.push_back(Current + 1);
        Indices.push_back(Current - 1);
    }

    // Make top disc
    Positions.push_back(innerRadius); Positions.push_back(0.f); Positions.push_back(height);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);

    Positions.push_back(outerRadius); Positions.push_back(0.f); Positions.push_back(height);
    Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);

    for (unsigned int i = 1; i <= slices; ++ i)
    {
        float const Angle = (float) i * 2.f * 3.14159f / slices;
        unsigned int const Current = Positions.size() / 3;
        Positions.push_back(cos<float>(Angle)*innerRadius);
        Positions.push_back(sin<float>(Angle)*innerRadius);
        Positions.push_back(height);
        Positions.push_back(cos<float>(Angle)*outerRadius);
        Positions.push_back(sin<float>(Angle)*outerRadius);
        Positions.push_back(height);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);
        Normals.push_back(0.f); Normals.push_back(0.f); Normals.push_back(1.f);
        Indices.push_back(Current - 2);
        Indices.push_back(Current - 1);
        Indices.push_back(Current - 0);
        Indices.push_back(Current - 0);
        Indices.push_back(Current - 1);
        Indices.push_back(Current + 1);
    }

    // Make outer sides
    unsigned int SideStart1 = 0, SideStart2 = 0;
    for (unsigned int j = 0; j <= stacks; ++ j)
    {
        float Interpolation = (float) j / stacks;

        SideStart2 = Positions.size() / 3;
        for (unsigned int k = 0; k <= slices; ++ k)
        {
            float const Angle = (float) k * 2.f * 3.14159f / slices;
            Positions.push_back(cos<float>(Angle)*outerRadius);
            Positions.push_back(sin<float>(Angle)*outerRadius);
            Positions.push_back(Interpolation * height);
            Normals.push_back(cos<float>(Angle)); Normals.push_back(sin<float>(Angle)); Normals.push_back(0.f);
        }

        if (j)
        {
            for (unsigned int k = 0; k < slices; ++ k)
            {
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart1 + k + 1);
                Indices.push_back(SideStart2 + k + 1);
            
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart2 + k + 1);
                Indices.push_back(SideStart2 + k);
            }
        }
        SideStart1 = SideStart2;
    }

    // Make inner sides
    SideStart1 = 0, SideStart2 = 0;
    for (unsigned int j = 0; j <= stacks; ++ j)
    {
        float Interpolation = (float) j / stacks;

        SideStart2 = Positions.size() / 3;
        for (unsigned int k = 0; k <= slices; ++ k)
        {
            float const Angle = (float) k * 2.f * 3.14159f / slices;
            Positions.push_back(cos<float>(Angle)*innerRadius);
            Positions.push_back(sin<float>(Angle)*innerRadius);
            Positions.push_back(Interpolation * height);
            Normals.push_back(-cos<float>(Angle)); Normals.push_back(-sin<float>(Angle)); Normals.push_back(0.f);
        }

        if (j)
        {
            for (unsigned int k = 0; k < slices; ++ k)
            {
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart2 + k + 1);
                Indices.push_back(SideStart1 + k + 1);
            
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart2 + k);
                Indices.push_back(SideStart2 + k + 1);
            }
        }
        SideStart1 = SideStart2;
    }

    return new Mesh(Positions, Normals, Indices);
}

Mesh * GeometryCreator::CreateSphere(glm::vec3 const & Radii, unsigned int const Slices, unsigned int const Stacks)
{
    std::vector<float> Positions, Normals;
    std::vector<unsigned short> Indices;

    // Make top and bottom points
    Positions.push_back(0.f); Positions.push_back(Radii.y); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(1.f); Normals.push_back(0.f);

    Positions.push_back(0.f); Positions.push_back(-Radii.y); Positions.push_back(0.f);
    Normals.push_back(0.f); Normals.push_back(-1.f); Normals.push_back(0.f);

    for (unsigned int i = 1; i <= Stacks; ++ i)
    {
        float const AngleV = (float) i * 3.14159f / Stacks;
        for (unsigned int j = 0; j <= Slices; ++ j)
        {
            float const AngleH = (float) j * 2.f * 3.14159f / Slices;
            vec3 Radial = vec3(
                cos<float>(AngleH)*sin<float>(AngleV),
                cos<float>(AngleV),
                sin<float>(AngleH)*sin<float>(AngleV));
            unsigned int const Start = Positions.size() / 3;
            Positions.push_back(Radial.x*Radii.x);
            Positions.push_back(Radial.y*Radii.y);
            Positions.push_back(Radial.z*Radii.z);
            Normals.push_back(Radial.x); Normals.push_back(Radial.y); Normals.push_back(Radial.z);
            
            if (j)
            {
                if (i == 1)
                {
                    Indices.push_back(Start);
                    Indices.push_back(Start - 1);
                    Indices.push_back(0);
                }
                else
                {
                    Indices.push_back(Start);
                    Indices.push_back(Start - 1);
                    Indices.push_back(Start - Slices - 2);

                    Indices.push_back(Start);
                    Indices.push_back(Start - Slices - 2);
                    Indices.push_back(Start - Slices - 1);
                }

                if (i == Stacks)
                {
                    Indices.push_back(Start);
                    Indices.push_back(1);
                    Indices.push_back(Start - 1);
                }
            }
        }
    }

    return new Mesh(Positions, Normals, Indices);
}


Mesh * GeometryCreator::CreateTorus(
    float const innerRadius, 
    float const outerRadius, 
    unsigned int const sides, 
    unsigned int const rings)
{
    std::vector<float> Positions, Normals;
    std::vector<unsigned short> Indices;

    float Radius = (outerRadius - innerRadius) / 2.f;

    // Make sides
    unsigned int SideStart1 = 0, SideStart2 = 0;
    for (unsigned int j = 0; j <= rings; ++ j)
    {
        float Interpolation = (float) j / rings;
        float const RadialAngle = Interpolation * 2.f * 3.14159f;

        SideStart2 = Positions.size() / 3;
        for (unsigned int k = 0; k <= sides; ++ k)
        {
            float const RingsAngle = (float) k * 2.f * 3.14159f / sides;
            float RadialExtent = sin<float>(RingsAngle) * Radius + (innerRadius + outerRadius) / 2.f;
            Positions.push_back(cos<float>(RadialAngle)*RadialExtent);
            Positions.push_back(sin<float>(RadialAngle)*RadialExtent);
            Positions.push_back(cos<float>(RingsAngle)*Radius);
            Normals.push_back(sin<float>(RingsAngle)*cos<float>(RadialAngle)); Normals.push_back(sin<float>(RingsAngle)*sin<float>(RadialAngle)); Normals.push_back(cos<float>(RingsAngle));
        }

        if (j)
        {
            for (unsigned int k = 0; k < sides; ++ k)
            {
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart1 + k + 1);
                Indices.push_back(SideStart2 + k + 1);
            
                Indices.push_back(SideStart1 + k);
                Indices.push_back(SideStart2 + k + 1);
                Indices.push_back(SideStart2 + k);
            }
        }
        SideStart1 = SideStart2;
    }

    return new Mesh(Positions, Normals, Indices);
}
