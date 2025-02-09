//
// Created by ByteFlow on 2019/12/26.
//

#define EGL_EGLEXT_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
#define __ANDROID_API__ 29
#undef EGL_ANDROID_get_native_client_buffer
#include <EGL/egl.h>

#define LOG_TAG "ByteFlow"
#define FUN_PRINT(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#include "handycpp/logging.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLUtils.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <happly.h>
#include <Components/PBOCanvas.h>
#include "PBOSample.h"
#include "Components/Renderer/WireFrameRenderer.h"
#include "Components/Renderer/TexturedMeshRenderer.h"

//#define PBO_UPLOAD
#define PBO_DOWNLOAD

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

PBOSample::PBOSample()
{
    m_ImageTextureId = GL_NONE;
	m_SamplerLoc = GL_NONE;
	m_MVPMatrixLoc = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_FrameIndex = 0;
}

PBOSample::~PBOSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void PBOSample::LoadImage(NativeImage *pImage)
{
	LOGCATE("PBOSample::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
		NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}
}

void PBOSample::Init()
{
    if(m_ProgramObj)
        return;

	//顶点坐标
	GLfloat vVertices[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
	};

	//正常纹理坐标
	GLfloat vTexCoors[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

	GLushort indices[] = { 0, 1, 2, 1, 3, 2 };

	char vShaderStr[] =
			"#version 300 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec2 a_texCoord;   \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out vec2 v_texCoord;                       \n"
			"void main()                                \n"
			"{                                          \n"
			"   gl_Position = u_MVPMatrix * a_position; \n"
			"   v_texCoord = a_texCoord;                \n"
			"}                                          \n";

	// 用于普通渲染的片段着色器脚本，简单纹理映射
	char fShaderStr[] =
			"#version 300 es\n"
			"precision mediump float;\n"
			"in vec2 v_texCoord;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"uniform sampler2D s_TextureMap;\n"
			"void main()\n"
			"{\n"
			"    outColor = texture(s_TextureMap, v_texCoord);\n"
			"}";

	// 编译链接用于普通渲染的着色器程序
	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

	ObjLoader *objLoader = new ObjLoader();
	objLoader->LoadObjFile();
	auto renderer = new TexturedMeshRenderer();
	renderer->Init();
	renderer->LoadTexturedMesh(objLoader);
	delete objLoader;

	auto renderer2 = new WireFrameRenderer();
	renderer2->Init();
	renderer2->LoadLines([](LinesType &lines, int & numElements) -> int {
		happly::PLYData plyIn("/sdcard/Android/data/com.byteflow.app/files/Download/model/poly/bun_zipper.ply");
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
		numElements = lines.size() * 2;

		return lines.size();
	});

	auto pboCanvas = std::make_shared<PBOCanvas>(m_RenderImage.width, m_RenderImage.height);
	pboCanvas->Init(PBOCanvas::TEXTURE);
	auto rootGameObject = std::make_shared<GameObject>();
	rootGameObject->SetCanvas(pboCanvas);
	rootGameObject->transform()->translation = { 0.0f, 0.0f, 0.0f};
	rootGameObject->transform()->rotation = { 0.0f, 0.0f, 0.0f};
	rootGameObject->transform()->scale = { 1.0f, 1.0f, 1.0f};

	auto rabbitGameObject = std::make_shared<GameObject>();
	rabbitGameObject->SetRenderer(renderer2);
	rabbitGameObject->transform()->scale = {8.0f, 8.0f, 8.0f};

	auto pearGameObject = std::make_shared<GameObject>();
	pearGameObject->SetRenderer(renderer);
	pearGameObject->transform()->translation = {0.0f, -1.0f, 2.0f};
	pearGameObject->transform()->scale = {0.2f, 0.2f, 0.2f};

	rootGameObject->AddChild(rabbitGameObject);
	rootGameObject->AddChild(pearGameObject);

	m_gameObject = rootGameObject;

//	auto translate = glm::vec3{ 0.0f, 0.0f, 0.0f};
//	auto rotation = glm::vec3{ 0.0f, 0.0f, 0.0f};
//	auto scale = glm::vec3{ 1.0f, 1.0f, 1.0f};
//	m_gameObject->AddParentTransform({.translation=translate, .scale=scale, .rotation = rotation});


	if (m_ProgramObj == GL_NONE)
	{
		LOGCATE("PBOSample::Init m_ProgramObj == GL_NONE");
		return;
	}
	m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	m_MVPMatrixLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

	// 生成 VBO ，加载顶点数据和索引数据
	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(4, m_VboIds);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GO_CHECK_GL_ERROR();

	// Generate VAO Ids
	glGenVertexArrays(1, m_VaoIds);
    // 初始化用于普通渲染的 VAO
	// Normal rendering VAO
	glBindVertexArray(m_VaoIds[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glEnableVertexAttribArray(TEXTURE_POS_INDX);
	glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(GL_NONE);
}

void PBOSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	GLSampleBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
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

	FUN_INFO("angle:(%d, %d), scale:(%f, %f), real:(%f, %f, %f), (%f, %f, %f)",
		  m_AngleX, m_AngleY, m_ScaleX, m_ScaleY,
		  rotation.x, rotation.y, rotation.z, scale.x,scale.y, scale.z
		  );
}


void PBOSample::Draw(int screenW, int screenH)
{

	m_gameObject->Draw();


	// 普通渲染
	// Do normal rendering
	glViewport(0, 0, screenW, screenH);
    UpdateMVPMatrix(m_MVPMatrix, 180.0, 0.0, (float)screenW / screenH);
    glUseProgram(m_ProgramObj);
	GO_CHECK_GL_ERROR();
	glBindVertexArray(m_VaoIds[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gameObject->GetCanvasTexture());
//	glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glUniformMatrix4fv(m_MVPMatrixLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
	glUniform1i(m_SamplerLoc, 0);
	GO_CHECK_GL_ERROR();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
	GO_CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glBindVertexArray(GL_NONE);

	m_FrameIndex++;

}

void PBOSample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		m_ProgramObj = GL_NONE;
	}

	if (m_ImageTextureId)
	{
		glDeleteTextures(1, &m_ImageTextureId);
	}

	if (m_VboIds[0])
	{
		glDeleteBuffers(4, m_VboIds);
	}

	if (m_VaoIds[0])
	{
		glDeleteVertexArrays(2, m_VaoIds);
	}

}


void PBOSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio, float transx, float transy, float transz)
{
    LOGCATE("PBOSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(transx, transy, transz));
    mvpMatrix = Projection * View * Model;
}

