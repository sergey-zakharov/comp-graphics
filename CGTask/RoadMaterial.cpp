#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "RoadMaterial.h"

RoadMaterial::RoadMaterial()
{}

void RoadMaterial::initialize()
{

	std::string vertFilename = "../shaders/CGTask/road.vert";
	std::string fragFilename = "../shaders/CGTask/road.frag";

	_programId = makeShaderProgram( vertFilename, fragFilename );

	//=========================================================
	//»нициализаци€ uniform-переменных дл€ преобразовани€ координат	
	_modelMatrixUniform = glGetUniformLocation( _programId, "modelMatrix" );
	_viewMatrixUniform = glGetUniformLocation( _programId, "viewMatrix" );
	_projMatrixUniform = glGetUniformLocation( _programId, "projectionMatrix" );
	_normalToCameraMatrixUniform = glGetUniformLocation( _programId, "normalToCameraMatrix" );

	//=========================================================
	//»нициализаци€ uniform-переменных дл€ освещени€		
	_lightPosUniform = glGetUniformLocation( _programId, "lightPos" );
	_ambientColorUniform = glGetUniformLocation( _programId, "ambientColor" );
	_diffuseColorUniform = glGetUniformLocation( _programId, "diffuseColor" );
	_specularColorUniform = glGetUniformLocation( _programId, "specularColor" );

	//=========================================================
	//»нициализаци€ uniform-переменных свойств матириалов
	_shininessUniform = glGetUniformLocation( _programId, "shininessFactor" );

	//=========================================================
	//»нициализаци€ uniform-переменных дл€ текстурировани€
	_diffuseTexUniform = glGetUniformLocation( _programId, "diffuseTex" );

	//=========================================================
	//»нициализаци€ прочих uniform-переменных
	_timeUniform = glGetUniformLocation( _programId, "time" );
}

void RoadMaterial::applyCommonUniforms()
{
	glUniform1f( _timeUniform, _time );
	glUniformMatrix4fv( _viewMatrixUniform, 1, GL_FALSE, glm::value_ptr( _viewMatrix ) );
	glUniformMatrix4fv( _projMatrixUniform, 1, GL_FALSE, glm::value_ptr( _projMatrix ) );

	glUniform4fv( _lightPosUniform, 1, glm::value_ptr( _lightPos ) );
	glUniform3fv( _ambientColorUniform, 1, glm::value_ptr( _ambientColor ) );
	glUniform3fv( _diffuseColorUniform, 1, glm::value_ptr( _diffuseColor ) );
	glUniform3fv( _specularColorUniform, 1, glm::value_ptr( _specularColor ) );
}

void RoadMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv( _modelMatrixUniform, 1, GL_FALSE, glm::value_ptr( _modelMatrix ) );

	glm::mat3 normalToCameraMatrix = glm::transpose( glm::inverse( glm::mat3( _viewMatrix * _modelMatrix ) ) );
	glUniformMatrix3fv( _normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr( normalToCameraMatrix ) );

	glUniform1f( _shininessUniform, _shininess );

	glUniform1i( _diffuseTexUniform, _diffuseTexUnit );
}
