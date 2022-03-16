//
// Created by zhangfuwen on 2022/3/15.
//

#include <Components/Renderer/UIRectRenderer.h>
#include <Components/Renderer/PointRenderer.h>
#include <Components/Renderer/TexturedMeshRenderer.h>
#include <Components/Renderer/LightedMeshRenderer.h>
#include <Components/Renderer/WireFrameRenderer.h>
#include <happly.h>
#include "Scene.h"
#include "handycpp/file.h"
#include "handycpp/string.h"
#include <handycpp/dyntype.h>
using namespace handycpp::dyntype::arithmetic;
#ifdef ANDROID
#define DEFAULT_OGL_ASSETS_DIR "/sdcard/Android/data/com.byteflow.app/files/Download/"
#else
#define DEFAULT_OGL_ASSETS_DIR "" RENDERER_SOURCE "/../assets/"
#endif

std::vector<UIRectRenderer::Rect> loadChart() {
    std::vector<UIRectRenderer::Rect> rects;
    std::string baseDir = DEFAULT_OGL_ASSETS_DIR;
    auto csvPath = baseDir + "/other/1.csv";

    // load records
    struct record {
        int context_id;
        int start_tick;
        int end_tick;
    };
    std::map<int, std::vector<record>> record_map;
    int max_tick = 0 , min_tick = INT_MAX;
    handycpp::file::for_each_line(csvPath, [&](int n, std::string line) {
        auto tokens = handycpp::string::split(line, ",");
        int context_id = 0 + tokens[0];
        auto & records = record_map[context_id];
        int start_tick = 0 + tokens[1];
        int end_tick = 0 + tokens[2];

        max_tick = std::max(max_tick, start_tick);
        max_tick = std::max(max_tick, end_tick);
        min_tick = std::min(min_tick, start_tick);
        min_tick = std::min(min_tick, end_tick);
        records.push_back({context_id, start_tick, end_tick});
    });


    // normalize to -1, 1
    int distance = max_tick - min_tick;
    float y_start = -1.0f;

    int i = 0;
    for(auto & [context_id, records] : record_map) {
        i++;
        if(!(i ==1 || i== 5)){
            continue;
        }
        y_start += 0.13;
        float y_end = 0.1f + y_start;
        for(const auto & record : records) {
            float start = (float)(record.start_tick - min_tick)/distance;
            start = start * 2 - 1.0f;
            float end = (float)(record.end_tick - min_tick)/distance;
            end = end * 2 - 1.0f;
            //make rects
            rects.push_back(UIRectRenderer::Rect::MakeRect({{start, y_start}, {end, y_end}}));
        }
    }

    return rects;
}

void Scene::Init() {
    m_lights.emplace_back(glm::vec3{ 0.0f, 0.5f, 0.5f},
                          glm::vec3{0.9f, 0.9f, 0.0f},
                          glm::vec3{ 0.9f, 0.9f, 0.9f},
                          glm::vec3 { 0.9f, 0.9f, 0.9f}
    );
    m_camera.emplace_back(false);

    ObjLoader *objLoader = new ObjLoader();
#ifdef ANDROID
    objLoader->LoadObjFile(DEFAULT_OGL_ASSETS_DIR "/model/poly/Apricot_02_hi_poly.obj");
#else
    objLoader->LoadObjFile(DEFAULT_OGL_ASSETS_DIR "/poly/Apricot_02_hi_poly.obj");
#endif
    auto renderer = new TexturedMeshRenderer();
    renderer->Init();
    renderer->LoadTexturedMesh(objLoader);
    auto lightRenderer = new LightedMeshRenderer();
    lightRenderer->Init();
    lightRenderer->LoadTexturedMesh(objLoader);
    delete objLoader;

    auto bunnyWireframeRenderer = new WireFrameRenderer();
    bunnyWireframeRenderer->Init();
    bunnyWireframeRenderer->LoadLines([](LinesType &lines) -> int {
#ifdef ANDROID
        happly::PLYData plyIn(DEFAULT_OGL_ASSETS_DIR "/model/poly/bun_zipper.ply");
#else
        happly::PLYData plyIn(DEFAULT_OGL_ASSETS_DIR "/poly/bun_zipper.ply");
#endif
        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();
        FUN_INFO("fInd.size %d", fInd.size());

        lines.reserve(fInd.size()*3);
        for(const auto & face : fInd) {
            auto p0 = vPos[face[0]];
            auto p1 = vPos[face[1]];
            auto p2 = vPos[face[2]];
            lines.push_back({glm::vec3(p0[0], p0[1], p0[2]), glm::vec3(p1[0], p1[1], p1[2])});
            lines.push_back({glm::vec3(p1[0], p1[1], p1[2]), glm::vec3(p2[0], p2[1], p2[2])});
            lines.push_back({glm::vec3(p2[0], p2[1], p2[2]), glm::vec3(p0[0], p0[1], p0[2])});
        }

        return lines.size();
    });

    auto lightSourceRenderer = new WireFrameRenderer();
    lightSourceRenderer->Init();
    lightSourceRenderer->LoadLines([&](LinesType &lines) -> int {
        lines.push_back({m_lights[0].getLightPos(), m_lights[0].getLightPos()});
        return lines.size();
    });

    auto pointRenderer = new PointRenderer();
    pointRenderer->Init();
    pointRenderer->LoadPoints([&](std::vector<Point> &points, int & numElements) -> int {
        for(const auto & light : m_lights) {
            points.push_back(Point{light.getLightPos(), glm::vec4(light.getAmbientColor(), 1.0f), 30.0f});
        }
        points.push_back(Point{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 30.0f});
        numElements = points.size();
        return points.size();
    });


//    auto pboCanvas = std::make_shared<PBOCanvas>(m_RenderImage.width, m_RenderImage.height);
//    pboCanvas->Init(PBOCanvas::TEXTURE);
    auto rootGameObject = std::make_shared<GameObject>();
//    rootGameObject->SetCanvas(pboCanvas);
    rootGameObject->transform()->translation = { 0.0f, 0.0f, 0.0f};
    rootGameObject->transform()->rotation = { 0.0f, 0.0f, 0.0f};
    rootGameObject->transform()->scale = { 1.0f, 1.0f, 1.0f};

    auto rabbitGameObject = std::make_shared<GameObject>();
    rabbitGameObject->SetRenderer(bunnyWireframeRenderer);
    rabbitGameObject->transform()->scale = {1.0f, 1.0f, 1.0f};

    auto pearGameObject = std::make_shared<GameObject>();
    pearGameObject->SetRenderer(renderer);
    pearGameObject->transform()->translation = {0.0f, -1.0f, 2.0f};
    pearGameObject->transform()->scale = {0.2f, 0.2f, 0.2f};

    auto pear2GameObject = std::make_shared<GameObject>();
    pear2GameObject->SetRenderer(lightRenderer);
    pear2GameObject->transform()->translation = {0.0f, -1.0f, 2.0f};
    pear2GameObject->transform()->scale = {0.02f, 0.02f, 0.02f};

    auto lightGameObject = std::make_shared<GameObject>();
    lightGameObject->SetRenderer(pointRenderer);
    lightGameObject->transform()->translation = {0.0f, -1.0f, 2.0f};
    lightGameObject->transform()->scale = {0.6f, 0.6f, 0.6f};
//	lightGameObject->transform()->translation = {0.0f, -1.0f, 2.0f};
//	lightGameObject->transform()->scale = {0.2f, 0.2f, 0.2f};

    auto uiGameObject = std::make_shared<GameObject>();
    auto uiRenderer = new UIRectRenderer();
    auto rects = loadChart();
    for(auto rect : rects) {
        uiRenderer->AddRect(rect);
    }
//	uiRenderer->AddRect(UIRectRenderer::Rect::MakeRectNoColor(UIRectRenderer::RectGeo{
//		.start = {-1.0f, -1.0f},
//		.end = {1.0f, 1.0f},
//	}));
    if(uiRenderer->Init() < 0) {
        FUN_ERROR("failed to initialize uiRenderer");
        return;
    }

    uiGameObject->SetRenderer(uiRenderer);

    rootGameObject->AddChild(uiGameObject);

    rootGameObject->AddChild(lightGameObject);

    rootGameObject->AddChild(rabbitGameObject);
//	rootGameObject->AddChild(pearGameObject);
    rootGameObject->AddChild(pear2GameObject);


    m_gameObject = rootGameObject;

    auto coordRenderer = std::make_shared<WireFrameRenderer>();
    coordRenderer->Init();
    using LineLoader = std::function<int(std::vector<std::array<glm::vec3, 2>> &, int &)>;
    coordRenderer->LoadLines([](std::vector<std::array<glm::vec3, 2>> & lines) -> int {
        lines.push_back({glm::vec3{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
        lines.push_back({glm::vec3{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
        lines.push_back({glm::vec3{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});

        return lines.size();
    });
    m_coordRenderer = coordRenderer;
}

void Scene::Draw() {
    Transform transform1;
    m_coordRenderer->Draw(transform1, m_camera[0], m_lights);
    for(const auto & cam : m_camera) {
        GO_CHECK_GL_ERROR();
        m_gameObject->Draw(cam, m_lights);
        GO_CHECK_GL_ERROR();
    }
}
void Scene::Update() {
    for(const auto & cam : m_camera) {
        GO_CHECK_GL_ERROR();
        m_gameObject->Update();
        GO_CHECK_GL_ERROR();
    }
}


void Scene::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
    m_AngleX = static_cast<int>(rotateX) / 5;
    m_AngleY = static_cast<int>(rotateY) / 5;
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
    auto angleX = m_AngleX % 360;
    auto angleY = m_AngleY % 360;

    float radiansX = static_cast<float>( angleX);
    float radiansY = static_cast<float>( angleY);
    auto translate = glm::vec3{ 0.0f, 0.0f, 0.0f};
    auto rotation = glm::vec3{ radiansX, radiansY, 0.0f};
    auto scale = glm::vec3{ m_ScaleX, m_ScaleX, m_ScaleX};
    m_gameObject->AddParentTransform({.translation=translate, .scale=scale, .rotation = rotation});

//    FUN_INFO("angle:(%d, %d), scale:(%f, %f), real:(%f, %f, %f), (%f, %f, %f)",
//             m_AngleX, m_AngleY, m_ScaleX, m_ScaleY,
//             rotation.x, rotation.y, rotation.z, scale.x,scale.y, scale.z
//    );
}
void Scene::CameraMove(float x, float y, float z) {
    m_camera[0].TryMove(x,y,z);
}

void Scene::CameraRotate(float x, float y, float z) {
    m_camera[0].Rotate(x,y,z);
}
