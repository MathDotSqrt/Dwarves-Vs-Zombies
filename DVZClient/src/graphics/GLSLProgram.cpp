#include "client/graphics/GLSLProgram.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

using namespace DVZ::Graphics;

GLSLProgram::GLSLProgram(ProgramID program, VertexID vertex, FragmentID fragment)
	: program(program), vertex(vertex), fragment(fragment) {

}

GLSLProgram::GLSLProgram(GLSLProgram&& other)
	: program(other.program),
	vertex(other.vertex),
	fragment(other.fragment),
	uniforms(std::move(other.uniforms)) {

	other.program = 0;
	other.vertex = 0;
	other.fragment = 0;
}

GLSLProgram::~GLSLProgram() {
	dispose();
}

GLSLProgram& GLSLProgram::operator=(GLSLProgram&& other) {
	if (this != &other) {
		this->dispose();

		std::swap(program, other.program);
		std::swap(vertex, other.vertex);
		std::swap(fragment, other.fragment);
		std::swap(uniforms, other.uniforms);
	}

	return *this;
}

void GLSLProgram::start() {
	if (isValid()) {
		glUseProgram(program);
	}
}

void GLSLProgram::end() {
	if (isValid()) {
		glUseProgram(0);
	}
}

i32 GLSLProgram::getUniformLocation(const std::string& uniformName) {
	if (!isValid()) {
		return -1;
	}

	const auto it = uniforms.find(uniformName);
	if (it == uniforms.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		uniforms[uniformName] = location;
		return location;
	}
	else {
		return it->second;
	}
}

void GLSLProgram::setUniform1i(const std::string& uniform, int i) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniform1i(loc, i);
	}
}

void GLSLProgram::setUniform1f(const std::string& uniform, float f) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniform1f(loc, f);
	}
}

void GLSLProgram::setUniform2f(const std::string& uniform, const glm::vec2& v) {
	setUniform2f(uniform, v.x, v.y);
}

void GLSLProgram::setUniform2f(const std::string& uniform, float x, float y) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniform2f(loc, x, y);
	}
}

void GLSLProgram::setUniform3f(const std::string& uniform, const glm::vec3& vec3) {
	setUniform3f(uniform, vec3.x, vec3.y, vec3.z);
}

void GLSLProgram::setUniform3f(const std::string& uniform, float vec3[3]) {
	setUniform3f(uniform, vec3[0], vec3[1], vec3[2]);
}

void GLSLProgram::setUniform3f(const std::string& uniform, float x, float y, float z) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniform3f(loc, x, y, z);
	}
}

void GLSLProgram::setUniformMat3(const std::string& uniform, const glm::mat3& mat3, bool transpose) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniformMatrix3fv(loc, 1, transpose, glm::value_ptr(mat3));
	}
}

void GLSLProgram::setUniformMat4(const std::string& uniform, const glm::mat4& mat4, bool transpose) {
	const auto loc = getUniformLocation(uniform);
	if (loc != -1) {
		glUniformMatrix4fv(loc, 1, transpose, glm::value_ptr(mat4));
	}
}

bool GLSLProgram::isValid() {
	return program != 0 && vertex != 0 && fragment != 0;
}

void GLSLProgram::dispose() {
	if (isValid()) {
		glDetachShader(program, vertex);
		glDeleteShader(vertex);

		glDetachShader(program, fragment);
		glDeleteShader(fragment);

		glDeleteProgram(program);

		program = 0;
		vertex = 0;
		fragment = 0;

		uniforms.clear();
	}
}