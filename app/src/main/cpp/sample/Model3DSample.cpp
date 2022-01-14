//
// Created by ByteFlow on 2019/7/30.
//

#include <gtc/matrix_transform.hpp>
#include "Model3DSample.h"
#include "../util/GLUtils.h"

Model3DSample::Model3DSample()
{
	m_AngleX = 0;
	m_AngleY = 0;

	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;

	m_pModel = nullptr;
	m_pShader = nullptr;
}

Model3DSample::~Model3DSample()
{

}

void Model3DSample::Init()
{
	if(m_pModel != nullptr && m_pShader != nullptr)
		return;

	char vShaderStr[] =
			"#version 300 es\n"
            "precision mediump float;\n"
			"layout (location = 0) in vec3 a_position;\n"
			"layout (location = 1) in vec3 a_normal;\n"
			"layout (location = 2) in vec2 a_texCoord;\n"
			"out vec2 v_texCoord;\n"
			"uniform mat4 u_MVPMatrix;\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform vec3 lightPos;\n"
            "uniform vec3 lightColor;\n"
            "uniform vec3 viewPos;\n"
            "out vec3 ambient;\n"
            "out vec3 diffuse;\n"
            "out vec3 specular;\n"
			"void main()\n"
			"{\n"
			"    v_texCoord = a_texCoord;    \n"
            "    vec4 position = vec4(a_position, 1.0);\n"
            "    gl_Position = u_MVPMatrix * position;\n"
            "    vec3 fragPos = vec3(u_ModelMatrix * position);\n"
            "\n"
            "    // Ambient\n"
            "    float ambientStrength = 0.25;\n"
            "    ambient = ambientStrength * lightColor;\n"
            "\n"
            "    // Diffuse\n"
            "    float diffuseStrength = 0.5;\n"
            "    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));\n"
            "    vec3 lightDir = normalize(lightPos - fragPos);\n"
            "    float diff = max(dot(unitNormal, lightDir), 0.0);\n"
            "    diffuse = diffuseStrength * diff * lightColor;\n"
            "\n"
            "    // Specular\n"
            "    float specularStrength = 0.3;\n"
            "    vec3 viewDir = normalize(viewPos - fragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, unitNormal);\n"
            "    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);\n"
            "    specular = specularStrength * spec * lightColor;\n"
			"}";

	char fShaderStr[] =
			"#version 300 es\n"
            "precision mediump float;"
			"out vec4 outColor;\n"
			"in vec2 v_texCoord;\n"
            "in vec3 ambient;\n"
            "in vec3 diffuse;\n"
            "in vec3 specular;\n"
			"uniform sampler2D texture_diffuse1;\n"
			"void main()\n"
			"{    \n"
            "    vec4 objectColor = texture(texture_diffuse1, v_texCoord);\n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    outColor = vec4(finalColor, 1.0);\n"
			"}";

    char fNoTextureShaderStr[] =
            "#version 300 es\n"
            "precision highp float;\n"
            "out vec4 outColor;\n"
            "in vec3 ambient;\n"
            "in vec3 diffuse;\n"
            "in vec3 specular;\n"
            "void main()\n"
            "{    \n"
            "    vec4 objectColor = vec4(0.6, 0.6, 0.6, 1.0);\n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    outColor = vec4(finalColor, 1.0);\n"
            "}";
    //TODO 先把 model 文件夹拷贝到 /sdcard/Android/data/com.byteflow.app/files/Download 路径下，然后可以选择你要加载的模型
	//m_pModel = new Model(path + "/model/nanosuit/nanosuit.obj");
	std::string path(DEFAULT_OGL_ASSETS_DIR);
    m_pModel = new Model(path + "/model/finishingMill/machine.obj");
    //m_pModel = new Model(path + "/model/poly/Apricot_02_hi_poly.obj");
    //m_pModel = new Model(path + "/model/tank/Abrams_BF3.obj");
    //m_pModel = new Model(path + "/model/girl/091_W_Aya_10K.obj");//2000 1500
    //m_pModel = new Model(path + "/model/new/camaro.obj");
    //m_pModel = new Model(path + "/model/bird/12214_Bird_v1max_l3.obj");

    if (m_pModel->ContainsTextures())
    {
        m_pShader = new Shader(vShaderStr, fShaderStr);
    }
    else
    {
        m_pShader = new Shader(vShaderStr, fNoTextureShaderStr);
    }
    mLineSample.Init();
}

void Model3DSample::LoadImage(NativeImage *pImage)
{
	LOGCATE("Model3DSample::LoadImage pImage = %p", pImage->ppPlane[0]);

}

void Model3DSample::Draw(int screenW, int screenH)
{
	if(m_pModel == nullptr || m_pShader == nullptr) return;
    LOGCATE("Model3DSample::Draw()");
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

	UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)screenW / screenH);

    m_pShader->use();
    m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
    m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
    m_pShader->setVec3("lightPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShader->setVec3("viewPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pModel->Draw((*m_pShader));

    mLineSample.setMVPMatrix(m_MVPMatrix);
    mLineSample.Draw(screenW, screenH);
}

void Model3DSample::Destroy()
{
    LOGCATE("Model3DSample::Destroy");
    if (m_pModel != nullptr) {
        m_pModel->Destroy();
        delete m_pModel;
        m_pModel = nullptr;
    }

    if (m_pShader != nullptr) {
        m_pShader->Destroy();
        delete m_pShader;
        m_pShader = nullptr;
    }
    mLineSample.Destroy();
}

void Model3DSample::SetColor(int index, float r, float g, float b){
    mLineSample.SetColor(index,r,g,b);
}

void Model3DSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
	LOGCATE("Model3DSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
	angleX = angleX % 360;
	angleY = angleY % 360;

	//转化为弧度角
	float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
	float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


	// Projection matrix
	//glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
	//glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

	// View matrix
	glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, m_pModel->GetMaxViewDistance() * 1.8f), // Camera is at (0,0,1), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
	Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::translate(Model, -m_pModel->GetAdjustModelPosVec());
    m_ModelMatrix = Model;
	mvpMatrix = Projection * View * Model;

}

void Model3DSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{
	GLSampleBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
	m_AngleX = static_cast<int>(rotateX);
	m_AngleY = static_cast<int>(rotateY);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
}

LineSample::LineSample()
{
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

LineSample::~LineSample()
{
}

void LineSample::Init()
{
    char vShaderStr_line[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            "void main()                              \n"
            "{                                        \n"
            "    gl_Position = u_MVPMatrix * vPosition;\n"
            "    v_texCoord = vPosition.zw;\n"
            "}                                        \n";

    char fShaderStr_line[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "uniform vec4 vColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vColor;                       \n"
            "}                                            \n";
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr_line, fShaderStr_line, m_VertexShader, m_FragmentShader);
}

void LineSample::Draw(int screenW, int screenH)
{
    GLfloat vVertices[] = {
        0.0f, 500.0f, 100.0f,
        0.0f, 250.0f, 100.0f,
        2.0f, 250.0f, 100.0f,
        0.0f, 500.0f, 100.0f,
        2.0f, 250.0f, 100.0f,
        2.0f, 500.0f, 100.0f,

        0.0f, 500.0f, 500.0f,
        0.0f, 250.0f, 500.0f,
        2.0f, 250.0f, 500.0f,
        0.0f, 500.0f, 500.0f,
        2.0f, 250.0f, 500.0f,
        2.0f, 500.0f, 500.0f,

        0.0f, 500.0f, 800.0f,
        0.0f, 250.0f, 800.0f,
        2.0f, 250.0f, 800.0f,
        0.0f, 500.0f, 800.0f,
        2.0f, 250.0f, 800.0f,
        2.0f, 500.0f, 800.0f,

        0.0f, 500.0f, 1400.0f,
        0.0f, 250.0f, 1400.0f,
        2.0f, 250.0f, 1400.0f,
        0.0f, 500.0f, 1400.0f,
        2.0f, 250.0f, 1400.0f,
        2.0f, 500.0f, 1400.0f,
    };

    if(m_ProgramObj == 0)
        return;

    // Use the program object
    glUseProgram (m_ProgramObj);

    // Load the vertex data
    int handler = glGetAttribLocation(m_ProgramObj, "vPosition");
    int handler_max = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    int handler_color = glGetUniformLocation(m_ProgramObj, "vColor");
    glVertexAttribPointer (handler, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glUniformMatrix4fv (handler_max, 1, GL_FALSE, &m_MVPMatrix[0][0] );
    glUniform4fv (handler_color, 1, colors[0]);
    glEnableVertexAttribArray (handler);
    glDrawArrays (GL_TRIANGLES, 0, 24);
    glDisableVertexAttribArray(handler);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用byte-alignment限制
    glEnable(GL_BLEND);

    glUseProgram (GL_NONE);
}

void LineSample::Destroy()
{
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = GL_NONE;
    }
}

void LineSample::setMVPMatrix(glm::mat4 &mvpMatrix)
{
    m_MVPMatrix = mvpMatrix;
}

void LineSample::SetColor(int index, float r, float g, float b){
    colors[index][0] = r;
    colors[index][1] = g;
    colors[index][2] = b;
}