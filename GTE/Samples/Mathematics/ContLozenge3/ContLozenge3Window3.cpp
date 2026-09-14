// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2026
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2026.09.12

#include "ContLozenge3Window3.h"
#include <Graphics/ConstantColorEffect.h>
#include <Mathematics/Cylinder3.h>
#include <Mathematics/ContLozenge3.h>
#include <Mathematics/DistPointRectangle.h>
#include <Mathematics/Matrix3x3.h>
#include <Mathematics/Rotation.h>
#include <random>

ContLozenge3Window3::ContLozenge3Window3(Parameters& parameters)
    :
    Window3(parameters),
    mNoCullWireframeState{},
    mVFormat{},
    mMeshFactory{},
    mLozenge{},
    mPerpendicular{},
    mPoints{},
    mFaces{},
    mCylinders{},
    mSpheres{},
    mDrawPoints(true),
    mDrawFaces(true),
    mDrawCylinders(true),
    mDrawSpheres(true)
{
    InitializeCamera(60.0f, GetAspectRatio(), 0.01f, 1000.0f, 0.01f, 0.0001f,
        { 0.0f, 0.0f, -12.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });

    mNoCullWireframeState = std::make_shared<RasterizerState>();
    mNoCullWireframeState->fill = RasterizerState::WIREFRAME;
    mNoCullWireframeState->cull = RasterizerState::NONE;
    mEngine->SetRasterizerState(mNoCullWireframeState);

    mVFormat.Bind(VASemantic::POSITION, DF_R32G32B32_FLOAT, 0);
    mMeshFactory.SetVertexFormat(mVFormat);

    CreateScene();

    mPVWMatrices.Update();
}

bool ContLozenge3Window3::OnCharPress(uint8_t key, int32_t x, int32_t y)
{
    switch (key)
    {
    case 'p':
    case 'P':
        mDrawPoints = !mDrawPoints;
        return true;
    case 'f':
    case 'F':
        mDrawFaces = !mDrawFaces;
        return true;
    case 'c':
    case 'C':
        mDrawCylinders = !mDrawCylinders;
        return true;
    case 's':
    case 'S':
        mDrawSpheres = !mDrawSpheres;
        return true;
    }
    return Window3::OnCharPress(key, x, y);
}

void ContLozenge3Window3::OnIdle()
{
    mTimer.Measure();

    if (mCameraRig.Move())
    {
        mPVWMatrices.Update();
    }

    mEngine->ClearBuffers();

    if (mDrawFaces)
    {
        for (auto const& face : mFaces)
        {
            mEngine->Draw(face);
        }
    }

    if (mDrawCylinders)
    {
        for (auto const& cylinder : mCylinders)
        {
            mEngine->Draw(cylinder);
        }
    }

    if (mDrawSpheres)
    {
        for (auto const& sphere : mSpheres)
        {
            mEngine->Draw(sphere);
        }
    }

    if (mDrawPoints)
    {
        for (auto const& visual : mPoints)
        {
            mEngine->Draw(visual);
        }
    }

    mEngine->Draw(8, mYSize - 8, { 0.0f, 0.0f, 0.0f, 1.0f }, mTimer.GetFPS());
    mEngine->DisplayColorBuffer(0);

    mTimer.UpdateFrameCount();
}

void ContLozenge3Window3::CreateScene()
{
#if 0
    // Create a sample point dataset.
    std::default_random_engine dre{};
    std::uniform_real_distribution<float> urd(-1.0f, 1.0f);
    std::size_t numPoints = 128;
    std::vector<Vector3<float>> points(numPoints);
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        points[i][0] = 4.0f * urd(dre);
        points[i][1] = 2.0f * urd(dre);
        points[i][2] = urd(dre);
    }

    std::ofstream output("Data/points128.binary", std::ios::binary);
    std::size_t sNumPoints = points.size();
    output.write(reinterpret_cast<char const*>(&sNumPoints), sizeof(sNumPoints));
    output.write(reinterpret_cast<char const*>(points.data()), points.size() * sizeof(Vector3<float>));
    output.close();
#endif
    std::ifstream input("Data/points128.binary", std::ios::binary);
    std::size_t numPoints{};
    input.read(reinterpret_cast<char*>(&numPoints), sizeof(numPoints));
    std::vector<Vector3<float>> points(numPoints);
    input.read(reinterpret_cast<char*>(points.data()), points.size() * sizeof(Vector3<float>));

    GetContainer(static_cast<std::int32_t>(points.size()), points.data(), mLozenge);
    mPerpendicular = Cross(mLozenge.rectangle.axis[0], mLozenge.rectangle.axis[1]);

    CreatePoints(points);
    CreateFaces();
    CreateCylinders();
    CreateSpheres();
    mTrackBall.Update();
}

void ContLozenge3Window3::CreatePoints(std::vector<Vector3<float>> const& points)
{
    auto ibuffer = std::make_shared<IndexBuffer>(IP_POLYPOINT, static_cast<std::uint32_t>(points.size()));
    std::shared_ptr<ConstantColorEffect> effect{};

    mPoints.resize(points.size());
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        auto& visual = mPoints[i];
        visual = mMeshFactory.CreateSphere(8, 8, 0.02f);
        visual->localTransform.SetTranslation(points[i]);
        effect = std::make_shared<ConstantColorEffect>(mProgramFactory, Vector4<float>{ 0.0f, 0.0f, 0.0f, 1.0f });
        visual->SetEffect(effect);
        mPVWMatrices.Subscribe(visual);
        mTrackBall.Attach(visual);
    }
}

void ContLozenge3Window3::CreateFaces()
{
    std::uint32_t constexpr numFacial = 64;
    Matrix3x3<float> rotate{};
    Vector3<float> translate{};
    auto const& center = mLozenge.rectangle.center;
    auto const& axis = mLozenge.rectangle.axis;
    auto const& extent = mLozenge.rectangle.extent;
    auto const& radius = mLozenge.radius;
    std::shared_ptr<ConstantColorEffect> effect{};

    Vector4<float> color{ 1.0f, 0.5f, 1.0f, 1.0f };

    mFaces[0] = mMeshFactory.CreateRectangle(numFacial, numFacial, extent[0], extent[1]);
    translate = center + radius * mPerpendicular;
    rotate.SetCol(2, mPerpendicular);
    rotate.SetCol(0, axis[0]);
    rotate.SetCol(1, axis[1]);
    mFaces[0]->localTransform.SetTranslation(translate);
    mFaces[0]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, color);
    mFaces[0]->SetEffect(effect);
    mPVWMatrices.Subscribe(mFaces[0]);
    mTrackBall.Attach(mFaces[0]);

    mFaces[1] = mMeshFactory.CreateRectangle(numFacial, numFacial, extent[0], extent[1]);
    translate = center - radius * mPerpendicular;
    mFaces[1]->localTransform.SetTranslation(translate);
    mFaces[1]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, color);
    mFaces[1]->SetEffect(effect);
    mPVWMatrices.Subscribe(mFaces[1]);
    mTrackBall.Attach(mFaces[1]);
}

void ContLozenge3Window3::CreateCylinders()
{
    std::uint32_t constexpr numAxial = 64;
    std::uint32_t constexpr numRadial = 64;
    Matrix3x3<float> rotate{};
    Vector3<float> translate{};
    auto const& center = mLozenge.rectangle.center;
    auto const& axis = mLozenge.rectangle.axis;
    auto const& extent = mLozenge.rectangle.extent;
    auto const& radius = mLozenge.radius;
    std::shared_ptr<ConstantColorEffect> effect{};

    std::array<Vector4<float>, 4> colors
    {
        Vector4<float>{ 1.0f, 0.0f, 0.0f, 1.0f },
        Vector4<float>{ 0.0f, 0.0f, 0.0f, 1.0f },
        Vector4<float>{ 0.0f, 0.0f, 1.0f, 1.0f },
        Vector4<float>{ 0.0f, 1.0f, 1.0f, 1.0f }
    };

    for (std::size_t i = 0; i < colors.size(); ++i)
    {
        colors[i] = 0.5f * (colors[i] + Vector4<float>::Ones());
    }

    mCylinders[0] = mMeshFactory.CreateCylinderOpen(numAxial, numRadial, radius, 2.0f * extent[1]);
    translate = center - extent[0] * axis[0];
    rotate.SetCol(0, mPerpendicular);
    rotate.SetCol(1, axis[0]);
    rotate.SetCol(2, axis[1]);
    mCylinders[0]->localTransform.SetTranslation(translate);
    mCylinders[0]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[0]);
    mCylinders[0]->SetEffect(effect);
    mPVWMatrices.Subscribe(mCylinders[0]);
    mTrackBall.Attach(mCylinders[0]);

    mCylinders[1] = mMeshFactory.CreateCylinderOpen(numAxial, numRadial, radius, 2.0f * extent[1]);
    translate = center + extent[0] * axis[0];
    mCylinders[1]->localTransform.SetTranslation(translate);
    mCylinders[1]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[1]);
    mCylinders[1]->SetEffect(effect);
    mPVWMatrices.Subscribe(mCylinders[1]);
    mTrackBall.Attach(mCylinders[1]);

    mCylinders[2] = mMeshFactory.CreateCylinderOpen(numAxial, numRadial, radius, 2.0f * extent[0]);
    translate = center - extent[1] * axis[1];
    rotate.SetCol(0, axis[1]);
    rotate.SetCol(1, mPerpendicular);
    rotate.SetCol(2, axis[0]);
    mCylinders[2]->localTransform.SetTranslation(translate);
    mCylinders[2]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[2]);
    mCylinders[2]->SetEffect(effect);
    mPVWMatrices.Subscribe(mCylinders[2]);
    mTrackBall.Attach(mCylinders[2]);

    mCylinders[3] = mMeshFactory.CreateCylinderOpen(numAxial, numRadial, radius, 2.0f * extent[0]);
    translate = center + extent[1] * axis[1];
    mCylinders[3]->localTransform.SetTranslation(translate);
    mCylinders[3]->localTransform.SetRotation(rotate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[3]);
    mCylinders[3]->SetEffect(effect);
    mPVWMatrices.Subscribe(mCylinders[3]);
    mTrackBall.Attach(mCylinders[3]);
}

void ContLozenge3Window3::CreateSpheres()
{
    std::uint32_t constexpr numZSamples = 64;
    std::uint32_t constexpr numRadial = 64;
    Vector3<float> translate{};
    auto const& center = mLozenge.rectangle.center;
    auto const& axis = mLozenge.rectangle.axis;
    auto const& extent = mLozenge.rectangle.extent;
    auto const& radius = mLozenge.radius;
    std::shared_ptr<ConstantColorEffect> effect{};

    std::array<Vector4<float>, 4> colors
    {
        Vector4<float>{ 55.0f, 125.0f, 34.0f, 256.0f } / 256.0f,
        Vector4<float>{ 239.0f, 136.0f, 190.0f, 256.0f } / 256.0f,
        Vector4<float>{ 142.0f, 64.0f, 58.0f, 256.0f } / 256.0f,
        Vector4<float>{ 240.0f, 134.0f, 80.0f, 256.0f } / 256.0f
    };

    for (std::size_t i = 0; i < colors.size(); ++i)
    {
        colors[i] = 0.5f * (colors[i] + Vector4<float>::Ones());
    }

    mSpheres[0] = mMeshFactory.CreateSphere(numZSamples, numRadial, radius);
    translate = center - extent[0] * axis[0] - extent[1] * axis[1];
    mSpheres[0]->localTransform.SetTranslation(translate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[0]);
    mSpheres[0]->SetEffect(effect);
    mPVWMatrices.Subscribe(mSpheres[0]);
    mTrackBall.Attach(mSpheres[0]);

    mSpheres[1] = mMeshFactory.CreateSphere(numZSamples, numRadial, radius);
    translate = center - extent[0] * axis[0] + extent[1] * axis[1];
    mSpheres[1]->localTransform.SetTranslation(translate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[1]);
    mSpheres[1]->SetEffect(effect);
    mPVWMatrices.Subscribe(mSpheres[1]);
    mTrackBall.Attach(mSpheres[1]);

    mSpheres[2] = mMeshFactory.CreateSphere(numZSamples, numRadial, radius);
    translate = center + extent[0] * axis[0] - extent[1] * axis[1];
    mSpheres[2]->localTransform.SetTranslation(translate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[2]);
    mSpheres[2]->SetEffect(effect);
    mPVWMatrices.Subscribe(mSpheres[2]);
    mTrackBall.Attach(mSpheres[2]);

    mSpheres[3] = mMeshFactory.CreateSphere(numZSamples, numRadial, radius);
    translate = center + extent[0] * axis[0] + extent[1] * axis[1];
    mSpheres[3]->localTransform.SetTranslation(translate);
    effect = std::make_shared<ConstantColorEffect>(mProgramFactory, colors[3]);
    mSpheres[3]->SetEffect(effect);
    mPVWMatrices.Subscribe(mSpheres[3]);
    mTrackBall.Attach(mSpheres[3]);
}
