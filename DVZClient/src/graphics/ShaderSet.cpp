#include "client/graphics/ShaderSet.hpp"

#include <optional>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <GL/glew.h>

using namespace DVZ::Graphics;

std::optional<std::string> read_file(const std::string& filename) {
	std::ifstream reader(RES_PATH"shaders/" + filename);
	if (reader) {
		std::stringstream buffer;
		buffer << reader.rdbuf();
		std::string file = buffer.str();
		return file;
	}
	else {
		spdlog::warn("Could not find file: {}", RES_PATH"shaders/" + filename);
		return {};
	}
}


std::string get_program_name(const std::vector<std::string>& shaders) {
	std::stringstream stream;
	stream << "{";

	for (int i = 0; i < shaders.size(); i++) {
		if (i != 0) {
			stream << ",";
		}

		stream << shaders[i];
	}

	stream << "}";

	return stream.str();
}

std::optional<std::string> preprocessor(std::string& source) {
	const std::string INCLUDE_STRING = "#include ";
	const size_t MAX_SHADER_CODE_LEN = 10000;

	if (source.length() > MAX_SHADER_CODE_LEN) {
		return {};
	}

	std::size_t found = source.find(INCLUDE_STRING);

	//if no include statements were found return the unmodified source
	if (found == std::string::npos) {
		return source;
	}

	std::size_t index = found + INCLUDE_STRING.length();

	//include filename will be placed here
	std::string include_file = "";
	while (source[index] != '\n') {
		include_file += source[index];
		index++;
	}

	std::size_t replace_length = INCLUDE_STRING.length() + include_file.length();

	if (include_file.length() == 0 || include_file[0] != '"' || include_file[include_file.length() - 1] != '"') {
		//LOG_ERROR("Include is invalid for %s", include_file.c_str());
		source.replace(found, replace_length, "\n");
	}

	include_file.erase(0, 1);						//erase first "
	include_file.erase(include_file.size() - 1);	//erase last  "

	auto replace_string = read_file(include_file);
	if (replace_string.has_value()) {
		const auto iter = std::find(replace_string->begin(), replace_string->end(), '\0');
		replace_string->erase(iter, replace_string->end());
		replace_string->append("\n");
		source.replace(found, replace_length, replace_string.value());
		source += '\n';
		return preprocessor(source);
	}
	return replace_string;

}

GLuint compile_shader(const std::string& source, GLenum shader_type) {
	const char* src = source.c_str();

	GLuint shaderID = glCreateShader(shader_type);
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	int compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		//Getting shader compulation Error
		int infologLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, (GLint*)&infologLength);
		char* infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(shaderID, infologLength, NULL, infoLog); // will include terminate char
		spdlog::warn("(S) Shader compilation error:\n{}", infoLog);
		free(infoLog);

		glDeleteShader(shaderID);
		return 0;
	}
	spdlog::info("(S) OK - Shader ID : [{}]", shaderID);
	return shaderID;
}


GLuint link_program(GLuint vertex, GLuint fragment) {
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	GLint is_linked = 0;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
	if (is_linked == GL_FALSE) {
		//todo maybe detach shader?
		GLint info_log_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
		char* info_log = (char*)malloc(info_log_len);
		glGetProgramInfoLog(program, info_log_len, NULL, info_log); // will include terminate char
		spdlog::warn("P) Program linking error:\n{}", info_log);
		free(info_log);
		glDeleteProgram(program);
		return 0;
	}
	spdlog::info("(P) Linked program {}", program);
	return program;
}

GLenum get_shader_type(const std::string& file) {
	const size_t pos = file.find_last_of('.');

	if (pos == std::string::npos) {
		spdlog::warn("(S) Invalid filename [{}]", file);
		return 0;
	}

	const std::string ext = file.substr(pos + 1);
	if (ext == "vert") {
		return GL_VERTEX_SHADER;
	}
	else if (ext == "frag") {
		return GL_FRAGMENT_SHADER;
	}
	else {
		spdlog::warn("(S) Invalid filetype [{}]", file);
		return 0;
	}
}

GLuint create_shader(const std::string& shader) {
	GLuint shader_id = 0;
	GLenum shader_type = get_shader_type(shader);
	if (shader_type == 0) {
		return 0;
	}

	auto source = read_file(shader);
	if (source) {
		const auto processedSource = preprocessor(source.value());
		if (processedSource) {
			shader_id = compile_shader(processedSource.value(), shader_type);
		}
	}

	return shader_id;
}

std::shared_ptr<GLSLProgram>
create_program(const std::string& vert, const std::string& frag) {
	GLSLProgram::VertexID vertex = create_shader(vert);
	GLSLProgram::FragmentID fragment = create_shader(frag);

	if (vertex == 0 || fragment == 0) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return nullptr;
	}

	GLSLProgram::ProgramID program = link_program(vertex, fragment);
	if (program == 0) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return nullptr;
	}

	return std::make_shared<GLSLProgram>(program, vertex, fragment);
}

std::shared_ptr<GLSLProgram>
ShaderLoader::load(const std::string& vert, const std::string& frag) const {

	spdlog::info("Creating program: {} | {}", vert, frag);
	auto program = create_program(vert, frag);
	if (!program) {
		spdlog::warn("Failed to create program");
		return nullptr;
	}

	return program;
}

std::shared_ptr<GLSLProgram>
ShaderLoader::load(std::shared_ptr<GLSLProgram> program) const {
	return program;
}

entt::resource_handle<GLSLProgram>
ShaderSet::get(entt::id_type id) {
	return cache.handle(id);
}

entt::resource_handle<GLSLProgram>
ShaderSet::load(entt::id_type id, std::string vert, std::string frag) {
	auto handle = cache.load<ShaderLoader>(id, vert, frag);
	if (handle) {
		loadedFiles.insert(vert);
		loadedFiles.insert(frag);
		loadedShaders[id] = std::make_pair(vert, frag);
	}

	return handle;
}

void ShaderSet::reload(std::string shaderFile) {
	for (auto& key_value : loadedShaders) {
		const auto shaderID = key_value.first;
		const auto& value = key_value.second;

		const auto& vert = value.first;
		const auto& frag = value.second;
		if (vert == shaderFile || frag == shaderFile) {
			auto program = create_program(vert, frag);
			if (program) {
				spdlog::info("Reload");
				cache.reload<ShaderLoader>(shaderID, program);
			}
		}
	}
}
