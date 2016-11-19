#pragma once

#ifdef _WIN32
    #ifndef usdiStaticLink
        #ifdef usdiImpl
            #define usdiAPI __declspec(dllexport)
        #else
            #define usdiAPI __declspec(dllimport)
        #endif
    #else
        #define usdiAPI
    #endif
#else
    #define usdiAPI
#endif

namespace usdi {

typedef unsigned char byte;
typedef unsigned int uint;
#ifndef usdiImpl
    // force make convertible
    class Context {};
    class Attribute {};
    class Schema {};
    class Xform : public Schema  {};
    class Camera : public Xform {};
    class Mesh : public Xform {};
    class Points : public Xform {};

    struct float2 { float x, y; };
    struct float3 { float x, y, z; };
    struct float4 { float x, y, z, w; };
    struct quatf { float x, y, z, w; };
    struct float3x3 { float3 v[3]; };
    struct float4x4 { float4 v[4]; };
#endif
    struct AABB
    {
        float3 center, extents;
    };


enum class InterpolationType
{
    None,
    Linear,
};

enum class NormalCalculationType
{
    Never,
    WhenMissing,
    Always,
};

enum class AttributeType
{
    Unknown,
    Bool,
    Byte,
    Int,
    UInt,
    Float,
    Float2,
    Float3,
    Float4,
    Quaternion,
    Token,
    String,
    Asset,
    UnknownArray = 0x100,
    BoolArray,
    ByteArray,
    IntArray,
    UIntArray,
    FloatArray,
    Float2Array,
    Float3Array,
    Float4Array,
    QuaternionArray,
    TokenArray,
    StringArray,
    AssetArray,
};

enum class TopologyVariance
{
    Constant, // both vertices and topologies are constant
    Homogenous, // vertices are not constant (= animated). topologies are constant.
    Heterogenous, // both vertices and topologies are not constant
};

typedef double Time;

struct ImportConfig
{
    InterpolationType interpolation = InterpolationType::Linear;
    NormalCalculationType normal_calculation = NormalCalculationType::WhenMissing;
    float scale = 1.0f;
    bool triangulate = true;
    bool swap_handedness = true;
    bool swap_faces = true;
    bool split_mesh = false;
    bool double_buffering = false;
};

struct ExportConfig
{
    bool instanceable_by_default = true;
    float scale = 1.0f;
    bool swap_handedness = true;
    bool swap_faces = true;
};


struct XformSummary
{
    enum class Type {
        Unknown,
        TRS,
        Matrix,
    };

    Time start = 0.0, end = 0.0;
    Type type = Type::Unknown;
};

struct XformData
{
    enum class Flags {
        UpdatedMask     = 0xf,
        UpdatedPosition = 0x1,
        UpdatedRotation = 0x2,
        UpdatedScale    = 0x4,
    };

    int flags = 0;
    float3 position = { 0.0f, 0.0f, 0.0f};
    quatf rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    float3 scale = { 1.0f, 1.0f, 1.0f };
    float4x4 transform = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
};


struct CameraSummary
{
    Time start = 0.0, end = 0.0;
};

struct CameraData
{
    float near_clipping_plane = 0.3f;
    float far_clipping_plane = 1000.0f;
    float field_of_view = 60.0f;        // in degree. vertical one
    float aspect_ratio = 16.0f / 9.0f;

    float focus_distance = 5.0f;        // in cm
    float focal_length = 0.0f;          // in mm
    float aperture = 35.0f;             // in mm. vertical one
};


struct MeshSummary
{
    Time                start = 0.0, end = 0.0;
    TopologyVariance    topology_variance = TopologyVariance::Constant;
    bool                has_normals = false;
    bool                has_uvs = false;
    bool                has_velocities = false;
};

struct SubmeshData
{
    float3  *points = nullptr;
    float3  *normals = nullptr;
    float2  *uvs = nullptr;
    int     *indices = nullptr;
    uint    num_points = 0;

    float3  center = { 0.0f, 0.0f, 0.0f };
    float3  extents = { 0.0f, 0.0f, 0.0f };
};

struct MeshData
{
    // these pointers can be null (in this case, just be ignored).
    // otherwise, if you pass to usdiMeshSampleReadData(), pointers must point valid memory block to store data.
    float3  *points = nullptr;
    float3  *velocities = nullptr;
    float3  *normals = nullptr;
    float2  *uvs = nullptr;
    int     *counts = nullptr;
    int     *indices = nullptr;
    int     *indices_triangulated = nullptr;

    uint    num_points = 0;
    uint    num_counts = 0;
    uint    num_indices = 0;
    uint    num_indices_triangulated = 0;

    float3  center = { 0.0f, 0.0f, 0.0f };
    float3  extents = { 0.0f, 0.0f, 0.0f };

    SubmeshData *submeshes = nullptr;
    uint    num_submeshes = 0;
};


struct PointsSummary
{
    Time    start = 0.0, end = 0.0;
    uint    peak_num_points = 0;
    bool    has_velocities = false;
};

struct PointsData
{
    // these pointers can be null (in this case, just be ignored).
    // otherwise, if you pass to usdiMeshSampleReadData(), pointers must point valid memory block to store data.
    float3  *points = nullptr;
    float3  *velocities = nullptr;

    uint    num_points = 0;
};


struct AttributeSummary
{
    Time            start = 0.0, end = 0.0;
    AttributeType   type = usdi::AttributeType::Unknown;
    int             num_samples = 0;
};

struct AttributeData
{
    void    *data = nullptr;
    int     num_elements = 0;
};

} // namespace usdi

extern "C" {

usdiAPI void             usdiSetDebugLevel(int l);
usdiAPI usdi::Time       usdiDefaultTime();

usdiAPI void             usdiSetPluginPath(const char *path);

usdiAPI void             usdiInitialize();
usdiAPI void             usdiFinalize();


// Context interface
usdiAPI usdi::Context*   usdiCreateContext();
usdiAPI void             usdiDestroyContext(usdi::Context *ctx);
usdiAPI bool             usdiOpen(usdi::Context *ctx, const char *path);
usdiAPI bool             usdiCreateStage(usdi::Context *ctx, const char *path);
usdiAPI void             usdiFlatten(usdi::Context *ctx);
usdiAPI bool             usdiSave(usdi::Context *ctx);
// path must *not* be same as identifier (parameter of usdiOpen() or usdiCreateStage())
usdiAPI bool             usdiSaveAs(usdi::Context *ctx, const char *path);
usdiAPI void             usdiSetImportConfig(usdi::Context *ctx, const usdi::ImportConfig *conf);
usdiAPI void             usdiGetImportConfig(usdi::Context *ctx, usdi::ImportConfig *conf);
usdiAPI void             usdiSetExportConfig(usdi::Context *ctx, const usdi::ExportConfig *conf);
usdiAPI void             usdiGetExportConfig(usdi::Context *ctx, usdi::ExportConfig *conf);

usdiAPI usdi::Xform*     usdiCreateXform(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Camera*    usdiCreateCamera(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Mesh*      usdiCreateMesh(usdi::Context *ctx, usdi::Schema *parent, const char *name);
usdiAPI usdi::Points*    usdiCreatePoints(usdi::Context *ctx, usdi::Schema *parent, const char *name);
// create external reference if assetpath is valid, otherwise create internal reference
usdiAPI usdi::Schema*    usdiCreateReference(usdi::Context *ctx, const char *dstprim, const char *assetpath, const char *srcprim);
usdiAPI usdi::Schema*    usdiGetRoot(usdi::Context *ctx);
usdiAPI usdi::Schema*    usdiFindSchema(usdi::Context *ctx, const char *path);

usdiAPI void             usdiUpdateAllSamples(usdi::Context *ctx, usdi::Time t);
usdiAPI void             usdiInvalidateAllSamples(usdi::Context *ctx);

// Prim interface
usdiAPI int              usdiPrimGetID(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetPath(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetName(usdi::Schema *schema);
usdiAPI const char*      usdiPrimGetTypeName(usdi::Schema *schema);

usdiAPI usdi::Schema*    usdiPrimGetMaster(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsInstance(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsInstanceable(usdi::Schema *schema);
usdiAPI bool             usdiPrimIsMaster(usdi::Schema *schema);
usdiAPI void             usdiPrimSetInstanceable(usdi::Schema *schema, bool v);

usdiAPI usdi::Schema*    usdiPrimGetParent(usdi::Schema *schema);
usdiAPI int              usdiPrimGetNumChildren(usdi::Schema *schema);
usdiAPI usdi::Schema*    usdiPrimGetChild(usdi::Schema *schema, int i);

usdiAPI int              usdiPrimGetNumAttributes(usdi::Schema *schema);
usdiAPI usdi::Attribute* usdiPrimGetAttribute(usdi::Schema *schema, int i);
usdiAPI usdi::Attribute* usdiPrimFindAttribute(usdi::Schema *schema, const char *name);
usdiAPI usdi::Attribute* usdiPrimCreateAttribute(usdi::Schema *schema, const char *name, usdi::AttributeType type);

usdiAPI int              usdiPrimGetNumVariantSets(usdi::Schema *schema);
usdiAPI int              usdiPrimGetNumVariants(usdi::Schema *schema, int iset);
usdiAPI const char*      usdiPrimGetVariantName(usdi::Schema *schema, int iset, int ival);
usdiAPI bool             usdiPrimSetVariantSelection(usdi::Schema *schema, int iset, int ival);
// return -1 if fail
usdiAPI int              usdiPrimFindVariantSet(usdi::Schema *schema, const char *name);
// return -1 if fail
usdiAPI int              usdiPrimFindVariant(usdi::Schema *schema, int iset, const char *name);
// return index of added set. -1 if fail
usdiAPI int              usdiPrimCreateVariantSet(usdi::Schema *schema, const char *name);
// return index of added variant. -1 if fail
usdiAPI int              usdiPrimCreateVariant(usdi::Schema *schema, int iset, const char *name);

usdiAPI bool             usdiPrimNeedsUpdate(usdi::Schema *schema);

// Xform interface
usdiAPI usdi::Xform*     usdiAsXform(usdi::Schema *schema); // dynamic cast to Xform
usdiAPI void             usdiXformGetSummary(usdi::Xform *xf, usdi::XformSummary *dst);
usdiAPI bool             usdiXformReadSample(usdi::Xform *xf, usdi::XformData *dst, usdi::Time t);
usdiAPI bool             usdiXformWriteSample(usdi::Xform *xf, const usdi::XformData *src, usdi::Time t);

// Camera interface
usdiAPI usdi::Camera*    usdiAsCamera(usdi::Schema *schema); // dynamic cast to Camera
usdiAPI void             usdiCameraGetSummary(usdi::Camera *cam, usdi::CameraSummary *dst);
usdiAPI bool             usdiCameraReadSample(usdi::Camera *cam, usdi::CameraData *dst, usdi::Time t);
usdiAPI bool             usdiCameraWriteSample(usdi::Camera *cam, const usdi::CameraData *src, usdi::Time t);

// Mesh interface
usdiAPI usdi::Mesh*      usdiAsMesh(usdi::Schema *schema); // dynamic cast to Mesh
usdiAPI void             usdiMeshGetSummary(usdi::Mesh *mesh, usdi::MeshSummary *dst);
usdiAPI bool             usdiMeshReadSample(usdi::Mesh *mesh, usdi::MeshData *dst, usdi::Time t, bool copy);
usdiAPI bool             usdiMeshWriteSample(usdi::Mesh *mesh, const usdi::MeshData *src, usdi::Time t);

// Points interface
usdiAPI usdi::Points*    usdiAsPoints(usdi::Schema *schema); // dynamic cast to Points
usdiAPI void             usdiPointsGetSummary(usdi::Points *points, usdi::PointsSummary *dst);
usdiAPI bool             usdiPointsReadSample(usdi::Points *points, usdi::PointsData *dst, usdi::Time t, bool copy);
usdiAPI bool             usdiPointsWriteSample(usdi::Points *points, const usdi::PointsData *src, usdi::Time t);

// Attribute interface
usdiAPI usdi::Schema*    usdiAttrGetParent(usdi::Attribute *attr);
usdiAPI const char*      usdiAttrGetName(usdi::Attribute *attr);
usdiAPI const char*      usdiAttrGetTypeName(usdi::Attribute *attr);
usdiAPI void             usdiAttrGetSummary(usdi::Attribute *attr, usdi::AttributeSummary *dst);
usdiAPI bool             usdiAttrReadSample(usdi::Attribute *attr, usdi::AttributeData *dst, usdi::Time t, bool copy);
usdiAPI bool             usdiAttrWriteSample(usdi::Attribute *attr, const usdi::AttributeData *src, usdi::Time t);

} // extern "C"


