//
// Created by ByteFlow on 2019/12/26.
//

#ifndef NDK_OPENGLES_3_0_PBOSAMPLE_H
#define NDK_OPENGLES_3_0_PBOSAMPLE_H


#include <glm/detail/type_mat.hpp>
#include <Components/Canvas.h>
#include "GLSampleBase.h"
#include "../util/ImageDef.h"
#include "../Components/Renderer/IRenderer.h"

class PBOSample : public GLSampleBase
{
public:
	PBOSample();

	virtual ~PBOSample();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);
	void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) override;

	virtual void Destroy();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio, float transx = 0, float transy = 0, float transz = 0);

private:
	GLuint m_ImageTextureId;
	GLuint m_VaoIds[2] = {GL_NONE};
	GLuint m_VboIds[4] = {GL_NONE};;
	GLint m_SamplerLoc;
	NativeImage m_RenderImage;

	IRenderer * m_renderer = nullptr;
	Canvas * m_canvas = nullptr;

	GLint m_MVPMatrixLoc;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
    glm::mat4 m_MVPMatrix;

    int m_FrameIndex;

};


#endif //NDK_OPENGLES_3_0_PBOSAMPLE_H
