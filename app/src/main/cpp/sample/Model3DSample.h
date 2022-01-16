//
// Created by ByteFlow on 2019/7/30.
//

#ifndef NDK_OPENGLES_3_0_MODEL3DSample_H
#define NDK_OPENGLES_3_0_MODEL3DSample_H


#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <shader.h>
#include <model.h>
#include "GLSampleBase.h"
#include "TextRenderSample.h"

class LineSample
{
public:
	LineSample();
	virtual ~LineSample();
	virtual void Init();
	virtual void Draw(int screenW, int screenH);
	virtual void Destroy();
	virtual void setMVPMatrix(glm::mat4 mvpMatrix);
	virtual void SetColor(float r, float g, float b);

private:
	GLuint m_ProgramObj;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLfloat color[4]{
			1.0f,1.0f,1.0f,1.0f,
	};

	int m_AngleX;
	int m_AngleY;
	float m_ScaleX;
	float m_ScaleY;
	glm::mat4 m_MVPMatrix;

};

class Model3DSample : public GLSampleBase
{
public:
	Model3DSample();

	virtual ~Model3DSample();

	virtual void LoadImage(NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int screenW, int screenH);

	virtual void Destroy();

	virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

	void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

	virtual void SetColor(int index, float r, float g, float b);

private:
	glm::mat4 m_MVPMatrix;
	glm::mat4 m_ModelMatrix;
	Shader *m_pShader;
	Model *m_pModel;

	int m_AngleX;
	int m_AngleY;
	float m_ScaleX;
	float m_ScaleY;

	LineSample mLineSample;
	TextRenderSample mTextSample;
};


#endif //NDK_OPENGLES_3_0_MODEL3DSample_H
