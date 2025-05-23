// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2025
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2025.05.10

#pragma once

#include <Applications/Window3.h>
#include <Mathematics/RigidBody.h>
#include <Mathematics/Timer.h>
#include "PhysicsModule.h"
using namespace gte;

// The PhysicsModule is an implementation of the collision detection and
// impulse-based collision response described in "Game Physics, 2nd edition."
// The code comments have relevant equation numbers from the book. However,
// the DoCollisionResponse function uses a variation for computing impulses,
// described in
//   https://www.geometrictools.com/Documentation/ComputingImpulsiveForces.pdf

class BouncingSpheresWindow3 : public Window3
{
public:
    BouncingSpheresWindow3(Parameters& parameters);

    virtual void OnIdle() override;
    virtual bool OnCharPress(uint8_t key, int32_t x, int32_t y) override;

private:
    struct VertexPT
    {
        VertexPT()
            :
            position(Vector3<float>::Zero()),
            tcoord(Vector2<float>::Zero())
        {
        }

        Vector3<float> position;
        Vector2<float> tcoord;
    };

    struct VertexPC
    {
        VertexPC()
            :
            position(Vector3<float>::Zero()),
            color(Vector4<float>::Zero())
        {
        }

        Vector3<float> position;
        Vector4<float> color;
    };

    bool SetEnvironment();
    void CreateScene();
    void CreatePhysicsObjects();
    void CreateGraphicsObjects();
    void CreateWall(size_t index, VertexFormat const& vformat,
        Vector3<float> const& pos0, Vector3<float> const& pos1,
        Vector3<float> const& pos2, Vector3<float> const& pos3,
        Vector4<float> const& color);

    void PhysicsTick();
    void GraphicsTick();

    enum { NUM_SPHERES = 16 };
    std::unique_ptr<PhysicsModule> mModule;

    std::shared_ptr<RasterizerState> mNoCullState;
    std::shared_ptr<RasterizerState> mNoCullWireState;
    std::shared_ptr<Node> mScene;
    std::array<std::shared_ptr<Visual>, 4> mPlaneMesh;
    std::array<std::shared_ptr<Visual>, NUM_SPHERES> mSphereMesh;

    Timer mPhysicsTimer, mGraphicsTimer;
    double mLastPhysicsTime, mCurrPhysicsTime;
    double mSimulationTime, mSimulationDeltaTime;
    double mLastGraphicsTime, mCurrGraphicsTime;
    bool mSingleStep;
};

