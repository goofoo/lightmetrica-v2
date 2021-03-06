/*
    Lightmetrica - A modern, research-oriented renderer

    Copyright (c) 2015 Hisanari Otsu

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <pch.h>
#include <lightmetrica/light.h>
#include <lightmetrica/property.h>
#include <lightmetrica/trianglemesh.h>
#include <lightmetrica/dist.h>
#include <lightmetrica/primitive.h>
#include <lightmetrica/sampler.h>
#include <lightmetrica/surfacegeometry.h>
#include <lightmetrica/triangleutils.h>

LM_NAMESPACE_BEGIN

class Light_Point final : public Light
{
public:

    LM_IMPL_CLASS(Light_Point, Light);

public:

    LM_IMPL_F(Load) = [this](const PropertyNode* prop, Assets* assets, const Primitive* primitive) -> bool
    {
        Le_ = SPD::FromRGB(prop->ChildAs<Vec3>("Le", Vec3()));
        const auto p = prop->ChildAs<Vec3>("position", Vec3());
        position_ = Vec3(primitive->transform * Vec4(p.x, p.y, p.z, 1_f));
        return true;
    };

public:

    LM_IMPL_F(Type) = [this]() -> int
    {
        return SurfaceInteractionType::L;
    };

    LM_IMPL_F(SamplePositionGivenPreviousPosition) = [this](const Vec2& u, const SurfaceGeometry& geomPrev, SurfaceGeometry& geom) -> void
    {
        geom.degenerated = true;
        geom.p = position_;
    };

    LM_IMPL_F(SamplePositionAndDirection) = [this](const Vec2& u, const Vec2& u2, SurfaceGeometry& geom, Vec3& wo) -> void
    {
        // Position
        geom.degenerated = true;
        geom.p = position_;

        // Direction
        wo = Sampler::UniformSampleSphere(u);
    };

    LM_IMPL_F(EvaluateDirectionPDF) = [this](const SurfaceGeometry& geom, int queryType, const Vec3& wi, const Vec3& wo, bool evalDelta) -> PDFVal
    {
        return Sampler::UniformSampleSpherePDFSA();
    };

    LM_IMPL_F(EvaluatePositionGivenDirectionPDF) = [this](const SurfaceGeometry& geom, const Vec3& wo, bool evalDelta) -> PDFVal
    {
        return PDFVal(PDFMeasure::Area, evalDelta ? 0_f : 1_f);
    };

    LM_IMPL_F(EvaluatePositionGivenPreviousPositionPDF) = [this](const SurfaceGeometry& geom, const SurfaceGeometry& geomPrev, bool evalDelta) -> PDFVal
    {
        return PDFVal(PDFMeasure::Area, evalDelta ? 0_f : 1_f);
    };

    LM_IMPL_F(EvaluateDirection) = [this](const SurfaceGeometry& geom, int types, const Vec3& wi, const Vec3& wo, TransportDirection transDir, bool evalDelta) -> SPD
    {
        return Le_;
    };

    LM_IMPL_F(EvaluatePosition) = [this](const SurfaceGeometry& geom, bool evalDelta) -> SPD
    {
        return evalDelta ? SPD() : SPD(1_f);
    };

    LM_IMPL_F(IsDeltaDirection) = [this](int type) -> bool
    {
        return false;
    };

    LM_IMPL_F(IsDeltaPosition) = [this](int type) -> bool
    {
        return true;
    };

public:

    SPD Le_;
    Vec3 position_;

};

LM_COMPONENT_REGISTER_IMPL(Light_Point, "light::point");

LM_NAMESPACE_END
